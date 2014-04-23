/*****************************************************************************/ 
/* sexp_outputter.h for cunit-sexp-outputter                                 */
/* Copyright (c) 2013 Thomas Hartman (rokstar83@gmail.com)                   */
/*                                                                           */
/* This program is free software; you can redistribute it and/or             */
/* modify it under the terms of the GNU General Public License               */
/* as published by the Free Software Foundation; either version 2            */
/* of the License, or the License, or (at your option) any later             */
/* version.                                                                  */
/*                                                                           */
/* This program is distributed in the hope that it will be useful,           */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/* GNU General Public License for more details.                              */
/*****************************************************************************/
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <CUnit/MyMem.h>
#include <CUnit/Util.h>
#include <CUnit/TestRun.h>

#include "sexp_outputter.h"

/* global / static vars */

static CU_pSuite _current_suite = NULL;
static FILE * _output = stdout;
static CU_BOOL _writing_suite = CU_FALSE;

/* Static function forward decls */
static CU_ErrorCode sexp_list_all_tests(CU_pTestRegistry pRegistry, const char* szFilename);
static CU_ErrorCode initialize_result(void);
static CU_ErrorCode uninitialize_result(void);

static void sexp_run_all_tests(CU_pTestRegistry pRegistry);
static void sexp_test_start_message_handler(const CU_pTest pTest, const CU_pSuite pSuite);
static void sexp_test_complete_message_handler(const CU_pTest pTest, const CU_pSuite pSuite, 
                                                    const CU_pFailureRecord pFailure);
static void sexp_all_tests_complete_message_handler(const CU_pFailureRecord pFailure);
static void sexp_suite_init_failure_message_handler(const CU_pSuite pSuite);
static void sexp_suite_cleanup_failure_message_handler(const CU_pSuite pSuite);

/*
  public interface functions
*/

void CU_sexp_run_tests(void)
{
   assert(NULL != CU_get_registry());

   /* Disable io buffering */
   setvbuf(stdout, NULL, _IONBF, 0);
   setvbuf(stderr, NULL, _IONBF, 0);

   initialize_result();   

   /* Setup handlers */
   CU_set_test_start_handler(sexp_test_start_message_handler);
   CU_set_test_complete_handler(sexp_test_complete_message_handler);
   CU_set_all_test_complete_handler(sexp_all_tests_complete_message_handler);
   CU_set_suite_init_failure_handler(sexp_suite_init_failure_message_handler);
   CU_set_suite_cleanup_failure_handler(sexp_suite_cleanup_failure_message_handler);

   sexp_run_all_tests(NULL);

   uninitialize_result();
}

void CU_set_output(FILE * output)
{
   int fd;
   int oflag;

   /* if the file pointer is null set to output to standard out otherwise
      check the that the file pointer is writable and set it to the global var*/   
   _output = stdout;

   if(output != NULL) {
      fd = fileno(output);
      oflag = fcntl(fd, F_GETFL);
      if(oflag | O_WRONLY || oflag | O_RDWR) {
         fprintf(stderr, "%s\n", _("ERROR - output file pointer is not writable."));         
      } else {
         _output = output;
      }
   }
}

CU_ErrorCode CU_list_tests(void)
{
   return automated_list_all_tests(CU_get_registry(), output);
}

static void sexp_list_all_tests(CU_pTestRegistry pRegistry)
{
   CU_pSuite pSuite = NULL;
   CU_pTest pTest = NULL;

   CU_set_error(CUE_SUCCESS);

   if(pRegistry == NULL) {
      CU_set_error(CUE_NOREGISTRY);
   } else {
      fprintf(output, "(:test-suite-count %u :test-count %u :test-suites (",
              pRegistry->uiNumberOfSuites, pRegistry->uiNumberOfTests);
      
      pSuite = pRegistry->pSuite;
      while(pSuite != NULL) {
         fprintf(output, 
                 "(:test-suite-name \"%s\" :init-val %s :cleanup-val %s "
                 ":active-val %s :test-count %u :tests (",
                 pSuite->pName,
                 (pSuite->pInitializeFunc != NULL ? "t" : "nil"),
                 (pSuite->pCleanUpFunc != NULL ? "t" : "nil"),
                 (pSuite->fActive != CU_FALSE ? "t" : "nil"),
                 pSuite->uiNumberOfTests);
         
         pTest = pSuite->pTest;
         while(pTest != NULL) {
            fprintf(output, "(:test-name \"%s\" :active-val %s)",
                    pTest->pName,
                    (pSuite->fActive != CU_FALSE ? "t" : "nil"));                              
            pTest = pTest->pNext;
         }

         fprintf(output,"))");

         pSuite = pSuite->pNext;
      }

      fprintf(output "))");
   }
   
   return CU_get_error();
}

static CU_ErrorCode initialize_result(void)
{
   char* szTime;
   time_t time = 0;

   CU_set_error(CUE_SUCCESS);

   time(&Time);
   szTime = ctime(&time);

   fprintf(output, "(:start-time \"%s\"", szTime);

   return CU_get_error();
}

static CU_ErrorCode uninitialize_result(void)
{
   char * szTime;
   time_t time = 0;
   
   time(&time);
   szTime = ctime(&time);
   fprintf(output, ":end-time \"%s\")", szTime);

   return CU_get_error();
}

static void sexp_run_all_tests(CU_pTestRegistry pRegistry)
{
   CU_pTestRegistry pOldRegistry = NULL;
   
   if(pRegistry != NULL) {
      pOldRegistry = CU_set_registry(pRegistry);   
   }

   fprintf(output, "(:results ");
   CU_run_all_tests();

   if(pRegistry != NULL) {
      CU_set_registry(pOldRegistry);
   }
}

static void sexp_test_start_message_handler(const CU_pTest pTest, const CU_pSuite pSuite)
{
   if(_current_suite == NULL || _current_suite != pSuite) {
      if(_writing_suite == CU_TRUE) {
         fprintf(_output, "))");
      }
            
      fprintf(_output, "(:run-suite-status 'success :suite-name \"%s\"", pSuite->pName);
      _writing_suite = CU_TRUE;
      _current_suite = pSuite;
   }
}

static void sexp_test_complete_message_handler(const CU_pTest pTest, const CU_pSuite pSuite,
                                               const CU_pFailureRecord pFailure)
{
   CU_pFailureRecord curFailure = pFailure

   if(curFailure != NULL) {
      while(curFailure != NULL) {
         fprintf(_output, 
                 "(:test-name \"%s\" :result 'failure :file-name \"%s\" :line-number %u "
                 ":condition \"%s\")", 
                 pTest->pName, 
                 (curFailure->strFileName == NULL ? curFailure->strFileName : ""),
                 curFailure->uiLineNumber,
                 (curFailure->strCondition == NULL ? curFailure->strCondition : ""));
         
         curFailure = curFailure->pNext;
      }      
   } else {
      fprintf(_output, "(:test-name \"%s\" :result 'success)", pTest->pName);
   }
}

static void sexp_all_tests_complete_message_handler(const CU_pFailureRecord pFailure)
{
   if(_current_suite != NULL && _writing_suite == CU_TRUE)
      fprintf(_output, "))");

   fprintf(_output, 
           ":suites-summary (:count %u :run-count %u ",
           ":success-count %u :failed-count %u :skip-count %u)",
           pRegistry->uiNumberOfSuites,
           runSummary->nSuitesRun,
           runSummary->nSuitesFailed,
           runSummary->nSuitesInactive);

   fprintf(_output,
           ":test-summary (:count %u :run-count %u :success-count %u ",
           ":failed-count %u skip-count %u)",
           pRegistry->uiNumberOfTests,
           pRunSummary->nTestsRun,
           pRunSummary->nTestsRun - pRunSummary->nTestsFailed,
           pRunSummary->nTestsFailed,
           pRunSummary->nTestInactive);

   fprintf(_output,
           ":assertion-summary (:count %u :run-count %u :success-count %u ",
           ":failed-count %u)",
           pRunSummary->nAsserts,
           pRunSummary->nAsserts,
           pRunSummary->nAsserts - pRunSummary->nAssertsFailed,
           pRunSummary->nAssertsFailed);   
}

static void sexp_suite_init_failure_message_handler(const CU_pSuite pSuite)
{
   if(_writing_suite == CU_TRUE) {
      fprintf(_output, "))");
   }

   fprintf(_output, "(:run-suite-status 'failed :suite-name \"%s\" "
           ":failure-reason \"Suite Initialization Failed\"",
           pSuite->pName);
           
}

static void sexp_suite_cleanup_failure_message_handler(const CU_pSuite pSuite)
{
   if(_writing_suite == CU_TRUE) {
      fprintf(_output, "))");
   }

   fprintf(_output, "(:run-suite-status 'failed :suite-name \"%s\" "
           ":failure-reason \"Suite Cleanup Failed\"",
           pSuite->pName);
}

