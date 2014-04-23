#include <stdio.h>
#include <CUnit/CUnit.h>
#include <cunitsexpoutputter/sexp_outputter.h>

void firstGoodTest(void) 
{
   CU_ASSERT(0 == 0);
   CU_ASSERT(1 == 1);
   CU_ASSERT(1 != 2);
}

void secondGoodTest(void)
{
   CU_ASSERT(0 != 10);
   CU_ASSERT(5 == 5);
   CU_ASSERT(6 == 6);
}

void firstBadTest(void)
{
   CU_ASSERT(1 == 1);
   CU_ASSERT(2 == 2);
   CU_ASSERT(2 == 4);
}

int main(int argc, char ** argv)
{
   CU_pSuite pSuite = NULL;
   
   if(CU_initialize_registry() != CUE_SUCCESS)
      return CU_get_error();

   pSuite = CU_add_suite("Test Suite", NULL, NULL);

   if(pSuite == NULL) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if(CU_add_test(pSuite, "first good test", firstGoodTest) == NULL ||
      CU_add_test(pSuite, "second good test", secondGoodTest) == NULL ||
      CU_add_test(pSuite, "first bad test", firstBadTest) == NULL) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   CU_sexp_set_output(stdout);
   CU_sexp_run_tests();
   CU_cleanup_registry();

   return CU_get_error();
}
