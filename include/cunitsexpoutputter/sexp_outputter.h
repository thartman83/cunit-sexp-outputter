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

/*
 *  Generates s-expression output for CUnit test framework
*/

#ifndef SEXPOUTPUTTER_H_
#define SEXPOUTPUTTER_H_

#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

   CU_EXPORT void CU_sexp_run_tests(void);

   CU_EXPORT CU_ErrorCode CU_list_tests(void);
   
   CU_EXPORT void CU_set_output(FILE * output);

#ifdef __cplusplus
}
#endif/*__cplusplus*/

#endif/*SEXPOUTPUTTER_H_*/
