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
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <CUnit/MyMem.h>
#include <CUnit/Util.h>
#include <CUnit/TestRun.h>

#include <sexp_outputter.h>

/* global / static vars */

static FILE * _output = NULL;

// #define LOGGING

#ifdef LOGGING
static FILE * _log = NULL;
#define LOG(msg, ...) fprintf(_log, msg, ##__VA_ARGS__)
#else
#define LOG(msg, ...)
#endif/*LOGGING*/

/* Static function forward decls */
static CU_ErrorCode sexp_list_all_tests(CU_pTestRegistry pRegistry);
static CU_ErrorCode initialize_result(void);
static CU_ErrorCode uninitialize_result(void);

static void sexp_run_all_tests(CU_pTestRegistry pRegistry);

static void sexp_suite_start_message_handler(const CU_pSuite pSuite);
static void sexp_suite_complete_message_handler(const CU_pSuite pSuite,
                                                const CU_pFailureRecord pFailure);

static void sexp_test_complete_message_handler(const CU_pTest pTest, const CU_pSuite pSuite, 
                                                    const CU_pFailureRecord pFailure);

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
   CU_set_test_complete_handler(sexp_test_complete_message_handler);

   CU_set_suite_start_handler(sexp_suite_start_message_handler);
   CU_set_suite_complete_handler(sexp_suite_complete_message_handler);

   CU_set_suite_init_failure_handler(sexp_suite_init_failure_message_handler);
   CU_set_suite_cleanup_failure_handler(sexp_suite_cleanup_failure_message_handler);   

   sexp_run_all_tests(NULL);

   uninitialize_result();
}

void CU_sexp_set_output(FILE * output)
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
         _output = output;
      } else {
         fprintf(stderr, "%s\n", "ERROR - output file pointer is not writable.");
      }
   }

#ifdef LOGGING
   _log = stderr;
#endif/*LOG*/

}

CU_ErrorCode CU_sexp_list_tests(void)
{
   return sexp_list_all_tests(CU_get_registry());
}

static CU_ErrorCode sexp_list_all_tests(CU_pTestRegistry pRegistry)
{
   CU_pSuite pSuite = NULL;
   CU_pTest pTest = NULL;

   CU_set_error(CUE_SUCCESS);

   if(pRegistry == NULL) {
      CU_set_error(CUE_NOREGISTRY);
   } else {
      fprintf(_output, "(:test-suite-count %u :test-count %u :test-suites (",
              pRegistry->uiNumberOfSuites, pRegistry->uiNumberOfTests);
      
      pSuite = pRegistry->pSuite;
      while(pSuite != NULL) {
         fprintf(_output, 
                 "(:test-suite-name \"%s\" :init-val %s :cleanup-val %s "
                 ":active-val %s :test-count %u :tests (",
                 pSuite->pName,
                 (pSuite->pInitializeFunc != NULL ? "t" : "nil"),
                 (pSuite->pCleanupFunc != NULL ? "t" : "nil"),
                 (pSuite->fActive != CU_FALSE ? "t" : "nil"),
                 pSuite->uiNumberOfTests);
         
         pTest = pSuite->pTest;
         while(pTest != NULL) {
            fprintf(_output, "(:test-name \"%s\" :active-val %s)",
                    pTest->pName,
                    (pSuite->fActive != CU_FALSE ? "t" : "nil"));                              
            pTest = pTest->pNext;
         }

         fprintf(_output,"))");

         pSuite = pSuite->pNext;
      }

      fprintf(_output, "))");
   }
   
   return CU_get_error();
}

static CU_ErrorCode initialize_result(void)
{
   char szTime[1000];
   time_t t = time(NULL);
   struct tm * p = localtime(&t);

   LOG("Entering initialize_result\n");

   strftime(szTime, 1000, "%F %T", p);

   CU_set_error(CUE_SUCCESS);

   fprintf(_output, "(:start-time \"%s\" ", szTime);

   LOG("Leaving initialize_result\n");
   return CU_get_error();
}

static CU_ErrorCode uninitialize_result(void)
{
   char szTime[1000];
   time_t t = time(NULL);   
   struct tm * p = localtime(&t);

   LOG("Entering uninitialize_result\n");

   strftime(szTime, 1000, "%F %T", p);

   CU_set_error(CUE_SUCCESS);

   fprintf(_output, ":end-time \"%s\")", szTime);

   LOG("Leaving uninitialize_result\n");

   return CU_get_error();
}

static void sexp_run_all_tests(CU_pTestRegistry pRegistry)
{
   CU_pTestRegistry pOldRegistry = NULL;

   LOG("Entering sexp_run_all_tests\n");
   
   if(pRegistry != NULL) {
      pOldRegistry = CU_set_registry(pRegistry);   
   }

   fprintf(_output, ":test-suites (");
   CU_run_all_tests();

   if(pRegistry != NULL) {
      CU_set_registry(pOldRegistry);
   }

   fprintf(_output, ")");
   LOG("Leaving sexp_run_all_tests\n");
}

static void sexp_suite_start_message_handler(const CU_pSuite pSuite)
{
   fprintf(_output, "(:name \"%s\" :tests (", pSuite->pName);
}

static void sexp_suite_complete_message_handler(const CU_pSuite pSuite,
                                                const CU_pFailureRecord pFailure)
{
   fprintf(_output, ") :status %s) ", (pFailure == NULL ? "success" : "failure"));
}

static void sexp_test_complete_message_handler(const CU_pTest pTest, const CU_pSuite pSuite,
                                               const CU_pFailureRecord pFailure)
{
   CU_pFailureRecord curFailure = pFailure;

   LOG("Entering sexp_test_complete_message_handler for `%s'\n", pSuite->pName);

   while(curFailure != NULL) {
      fprintf(_output, "(:name \"%s\" :status failure :file-name \"%s\" :line-number %u "
              ":condition \"%s\")", pTest->pName, 
              (curFailure->strFileName == NULL ? curFailure->strFileName : ""),
              curFailure->uiLineNumber,
              (curFailure->strCondition == NULL ? curFailure->strCondition : ""));      
      curFailure = curFailure->pNext;
   }

   if(pFailure == NULL) {
      fprintf(_output, "(:name \"%s\" :status success)", pTest->pName);
   }
   
   LOG("Leaving sexp_test_complete_message_handler for `%s'\n", pSuite->pName);
}

static void sexp_suite_init_failure_message_handler(const CU_pSuite pSuite)
{
   fprintf(_output, "(:name \"%s\" :status failed :log \"Suite Initialization Failed\")",
           pSuite->pName);           
}

static void sexp_suite_cleanup_failure_message_handler(const CU_pSuite pSuite)
{
   fprintf(_output, "(:name \"%s\" :status failed :log \"Suite Cleanup Failed\")",
           pSuite->pName);
}

