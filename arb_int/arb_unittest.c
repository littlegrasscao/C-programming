
/*********************************************************
 *  Unit tests for arb_int_t (also used for grading!)
 *
 * To compile, link with cunit and mpz.
 *  (-lgmp -lcunit)
 *
 *  DO NOT MODIFY
 *********************************************************/

#include "arb_int.h"

#include <CUnit/Basic.h>

#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <stdbool.h>
#include <malloc.h>
#include <time.h>
#include <ctype.h>

#include <gmp.h>

// For certain tests, COUNT^2 (binary ops) are tested
#define COUNT 100


// Largest number generated for the test (in digits) *before*
// performing operation.
#define VERY_LARGE 800


#define MAX(a,b) ((a) > (b) ? (a) : (b))


/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
static int init_suite(void)
{
   return 0;
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
static int clean_suite(void)
{
   return 0;
}



void print_arb(const arb_int_t i)
{
   const size_t bufsize = 100*VERY_LARGE;
   char * buf = (char*) malloc(bufsize);
   CU_ASSERT_EQUAL(arb_to_string(i, buf, bufsize), 0);
   puts(buf);
   free(buf);
}

// mpz tends to have leading zeros in string conversion
static void mpz_to_string(const mpz_t i, char * buf, size_t bufsize)
{
   const size_t needed = mpz_sizeinbase(i, 10)+2;
   char * tmp = (char*)malloc (needed);

   mpz_get_str(tmp, 10, i);

   // gmp can emit leading 0 -- strip while copying
   const char * source = tmp;
   char * dest = buf;
   char c;
   bool leading = true;
   size_t remaining = bufsize;
   do
   {
      c = *source++;
      if (isdigit(c) && (c != '0'))
         leading = false;

      // Don't skip 0
      if (leading && (c == '0'))
         continue;

      *dest++ = c;
      --remaining;
      assert(remaining);
      if (!c)
         break;
   } while(1);

   if (!(*buf))
   {
      assert(bufsize >= 2);
      strcpy(buf, "0");
   }
   free(tmp);
}

// Compare mpz against arb_int_t (via string)
int compare_mixed (const arb_int_t i, const mpz_t z)
{
   // Figure out needed space from mpz
   // (we really need a function in arb_int to do the same!)
   // Some extra space in case the numbers are *not* the same
   const size_t bufsize = MAX(
         4*(mpz_sizeinbase(z, 10)+2),
         4*VERY_LARGE);
   char * b1 = (char*) malloc(bufsize);
   char * b2 = (char*) malloc(bufsize);

   CU_ASSERT_EQUAL(arb_to_string(i, b1, bufsize), 0);
   mpz_to_string(z, b2, bufsize);

   int ret = strcmp(b1, b2);

   //printf("==%s arb='%s' mpz='%s'\n", (!ret ? "EQ" : "NE"), b1, b2);

   free(b1);
   free(b2);
   return ret;
}

// Generate random digits (size-1 digits)
// Does only generate forms that would be returned by arb_to_string,
// and so can be used to compare to the output of the latter.
static void random_digits(char * buf, size_t size)
{
   assert(size>1);

   char * dest = buf;
   size_t remaining = size - 1;

   // Only consider negative numbers if we have space for sign + digit
   if ((rand() & 1) && (size > 2))
   {
      *dest++ = '-';
      --remaining;
   }

   for (unsigned int i=0; i<remaining; ++i)
   {
      char c = '0' + (rand() % 10);

      // Don't have a leading zero, but allow real 0
      if (!i && c =='0' && ((i+1) < remaining))
      {
         ++c;
      }
      *dest++ = c;
   }
   *dest++ = 0;
   assert(dest == (buf + size));
}

// assumes not bigger than VERY_LARGE
static int arb_compare_via_string(arb_int_t x, arb_int_t y)
{
   const size_t bufsize = VERY_LARGE+1;
   char * sx = (char *) malloc(bufsize);
   char * sy = (char *) malloc(bufsize);

   CU_ASSERT_EQUAL(arb_to_string(x, sx, bufsize), 0);
   CU_ASSERT_EQUAL(arb_to_string(y, sy, bufsize), 0);

   int ret = strcmp(sx, sy);
   free(sx);
   free(sy);
   return ret;
}

// Generate random digits, from min to max digits (inclusive)
static void random_digits_upto (char * buf, size_t bufsize,
      size_t min, size_t max)
{
   assert(min <= max);
   const size_t realsize = min + rand() % (max-min+1);

   // +1 for 0 termination
   assert(realsize+1 <= bufsize);
   random_digits(buf, realsize+1);
}

static void random_arb_int(size_t min, size_t max, arb_int_t * ret)
{
   const size_t bufsize = max + 1;
   char * buf = (char*) malloc (bufsize);
   random_digits_upto(buf, bufsize, min, max);
   int r= arb_from_string(ret, buf);
   free(buf);
   CU_ASSERT_EQUAL_FATAL(r, 0);
}

// Generate random gmp int and arb_int, up to max digits
static void random_both(size_t min, size_t max, arb_int_t * i,
      mpz_t * i2)
{
   char buf[max+1];
   random_digits_upto(buf, sizeof(buf), min, max);
   mpz_init_set_str(*i2, buf, 10);
   CU_ASSERT_EQUAL_FATAL(arb_from_string(i, buf), 0);
}


static void test_to_int_oversize()
{
   // !!!!! depends on working add, subtract, from_int  !!!!!
   long long int boundaries[] = {LLONG_MIN, LLONG_MAX};
   for (unsigned int v = 0; v<sizeof(boundaries)/sizeof(boundaries[0]); ++v)
   {
      long long int base = boundaries[v];
      for (unsigned int c=0; c<COUNT; ++c)
      {
         arb_int_t x;
         CU_ASSERT_EQUAL(arb_from_int(&x, base), 0);

         arb_int_t step;
         CU_ASSERT_EQUAL(arb_from_int(&step, (long long) 1 + rand()), 0);

         if (base > 0)
         {
            CU_ASSERT_EQUAL(arb_add(x, step), 0);
         }
         else
         {
            CU_ASSERT_EQUAL(arb_subtract(x, step), 0);
         }

         long long int out = 0;
         int ret = arb_to_int(x, &out);
         CU_ASSERT_NOT_EQUAL(ret, 0);

         arb_free(&x);
         arb_free(&step);
      }
   }
}

static void test_from_string_oversize()
{
   const int bufsize = VERY_LARGE+1;
   char * buf = malloc(bufsize);
   char * buf2 = malloc(bufsize);
   for (unsigned int i=0; i<COUNT; ++i)
   {
      random_digits(buf, bufsize);
      arb_int_t x;
      CU_ASSERT_EQUAL(arb_from_string(&x, buf), 0);
      CU_ASSERT_EQUAL(arb_to_string(x, buf2, bufsize), 0);
      CU_ASSERT_EQUAL(strcmp(buf, buf2), 0);
      arb_free(&x);
   }
   free(buf2);
   free(buf);
}

static void test_from_string()
{
   struct E
   {
      const char * s;
      const char * s2;
      long long v;
   };
   struct E tests[] = {
      {"123", "123", 123},
      {"-123", "-123", -123},
      {"+123", "123", 123},
      {"3", "3", 3}
   };

   for (unsigned int c=0; c<sizeof(tests)/sizeof(tests[0]); ++c)
   {
      arb_int_t x;
      CU_ASSERT_EQUAL(arb_from_string(&x, tests[c].s), 0);
      
      char buf[255];
      CU_ASSERT_EQUAL(arb_to_string(x, buf, sizeof(buf)), 0);
      CU_ASSERT_STRING_EQUAL(buf, tests[c].s2);
      //printf("'%s' '%s'\n", buf, tests[c].s2);

      long long r;
      CU_ASSERT_EQUAL(arb_to_int(x, &r), 0);
      CU_ASSERT_EQUAL(r, tests[c].v);
      //printf ("%lli %lli\n", r, tests[c].v);

      arb_free(&x);
   }
}

static void test_from_string_bad()
{
   const char * bad[] = {
     " 2323",
     "++12",
     "+-1",
     "--1",
     "",
     NULL,
     "232-22",
     "avbds",
     "23a023",
     "23 23",
     "a323 ",
     " 12 ",
     "+12+",
     "0012",
     "-00",
     "00",
     "001234",
     "000000002",
     "123+",
     "0+",
     "0+23",
     "231q",
     "232s232",
     "q1",
     "23123s"
   };

   for (unsigned int c=0; c<sizeof(bad)/sizeof(bad[0]); ++c)
   {
      arb_int_t x;
      CU_ASSERT_NOT_EQUAL(arb_from_string(&x, bad[c]), 0);
   }
}

static long long int llrandom()
{
   long long int x;
   char * ptr = (char *) &x;
   for (unsigned int i=0; i<sizeof(x); ++i)
   {
      *ptr++ = rand() & 0xff;
   }
   return x;
}

static void test_to_int_random()
{
   for (unsigned int c=0; c<COUNT; ++c)
   {
      long long rnd = llrandom();

      arb_int_t x;
      CU_ASSERT_EQUAL(arb_from_int(&x, rnd), 0);
      
      long long r;
      CU_ASSERT_EQUAL(arb_to_int(x, &r), 0);
      CU_ASSERT_EQUAL(r, rnd);

      arb_free(&x);
   }
}

static void test_to_int()
{
   struct E
   {
      const char * s;
      long long v;
   };
   struct E tests[] = {
      {"123", 123},
      {"-123", -123},
      {"+123",  123},
      {"3",  3},
    };

   for (unsigned int c=0; c<sizeof(tests)/sizeof(tests[0]); ++c)
   {
      arb_int_t x;
      CU_ASSERT_EQUAL(arb_from_string(&x, tests[c].s), 0);
      
      long long r;
      CU_ASSERT_EQUAL(arb_to_int(x, &r), 0);
      CU_ASSERT_EQUAL(r, tests[c].v);

      arb_free(&x);
   }
}

static void test_to_int_boundary()
{
   long long int boundaries[] = { LLONG_MAX, LLONG_MIN };
   for (unsigned int c=0; c<sizeof(boundaries)/sizeof(boundaries[0]); ++c)
   {
      char buf[80];
      sprintf(buf, "%lli", boundaries[c]);
      arb_int_t x;
      CU_ASSERT_EQUAL(arb_from_string(&x, buf), 0);
      
      long long r = 0;
      CU_ASSERT_EQUAL(arb_to_int(x, &r), 0);
      CU_ASSERT_EQUAL(r, boundaries[c]);

      arb_free(&x);
   }
}

static void test_to_int_boundary1()
{
   long long int boundaries[] = { LLONG_MAX, LLONG_MIN };
   for (unsigned int c=0; c<sizeof(boundaries)/sizeof(boundaries[0]); ++c)
   {
      char buf[80];
      snprintf(buf, sizeof(buf), "%lli1", boundaries[c]);
      arb_int_t x;
      CU_ASSERT_EQUAL_FATAL(arb_from_string(&x, buf), 0);
      
      long long r = 0;
      CU_ASSERT_NOT_EQUAL(arb_to_int(x, &r), 0);
      arb_free(&x);
   }
}


static void test_from_int_boundary()
{
   long long int boundaries[] = { LLONG_MAX, LLONG_MIN };
   for (unsigned int i=0; i<sizeof(boundaries)/sizeof(boundaries[0]); ++i)
   {
      const long long signed int val = boundaries[i];

      arb_int_t x;
      CU_ASSERT_EQUAL(arb_from_int(&x, val), 0);

      long long signed int val2;
      CU_ASSERT_EQUAL(arb_to_int(x, &val2), 0);
      CU_ASSERT_EQUAL(val2, val);

      arb_free(&x);
   }
}

static void test_from_int()
{
   for (unsigned int i=0; i<COUNT; ++i)
   {

      long long signed int val = rand();

      arb_int_t x;
      CU_ASSERT_EQUAL(arb_from_int(&x, val), 0);

      long long signed int val2;
      CU_ASSERT_EQUAL(arb_to_int(x, &val2), 0);
      CU_ASSERT_EQUAL(val2, val);

      arb_free(&x);
   }
}

typedef void (*bin_op_helper_t)(const arb_int_t x1, const arb_int_t x2,
      const mpz_t y1, const mpz_t y2);


static void test_bin_op_helper(arb_int_t * arbs,
      mpz_t * mpz, size_t listsize, bin_op_helper_t func)
{
   for (unsigned int x=0; x<listsize; ++x)
   {
      for (unsigned int y=0; y<listsize; ++y)
      {
         func(arbs[x], arbs[y], mpz[x], mpz[y]);
      }
   }

}

static void test_bin_op_helper_random(bin_op_helper_t func)
{
   const size_t listsize = 100;
   arb_int_t arbs[listsize];
   mpz_t mpz[listsize];

   for (unsigned int i=0; i<listsize; ++i)
   {
      random_both(1, VERY_LARGE, &arbs[i], &mpz[i]);
   }

   test_bin_op_helper(arbs, mpz, listsize, func);

   for (unsigned int i=0; i<listsize; ++i)
   {
      arb_free(&arbs[i]);
      mpz_clear(mpz[i]);
   }

}

typedef struct
{
   long x;
   long y;
} pair;

static void test_bin_op_helper_manual(const pair * numbers,
      size_t count, bin_op_helper_t func)
{
   const size_t listsize = count;

   for (unsigned int i=0; i<listsize; ++i)
   {
      const pair * cur = &numbers[i];

      arb_int_t ix, iy;
      mpz_t zx, zy;

      CU_ASSERT_EQUAL_FATAL(arb_from_int(&ix, cur->x), 0);
      CU_ASSERT_EQUAL_FATAL(arb_from_int(&iy, cur->y), 0);
      mpz_init_set_si(zx, cur->x);
      mpz_init_set_si(zy, cur->y);

      func(ix, iy, zx, zy);

      arb_free(&ix);
      arb_free(&iy);
      mpz_clear(zx);
      mpz_clear(zy);
   }
}

static void compare_bin_op(const arb_int_t x1, const arb_int_t y1,
      const mpz_t x2, const mpz_t y2)
{
   int z = mpz_cmp(x2, y2);
   // fix up  to -1 0 1
   if (z)
      z = z/abs(z);

   const int i = arb_compare(x1, y1);
   //print_arb(x1);
   //print_arb(y1);
   //printf("->%i %i\n\n", i, z);

   CU_ASSERT_TRUE(i == 0 || i == -1 || i == 1);
   CU_ASSERT_EQUAL(z, i);
}

static void test_compare()
{
   test_bin_op_helper_random(compare_bin_op);
}

static void add_bin_op(const arb_int_t i1, const arb_int_t i2,
      const mpz_t z1, const mpz_t z2)
{
   arb_int_t res;
   CU_ASSERT_EQUAL(arb_duplicate(&res, i1), 0);
   CU_ASSERT_EQUAL(arb_add(res, i2), 0);

   mpz_t z;
   mpz_init(z);

   mpz_add(z, z1, z2);

   int ret = compare_mixed(res, z);
   CU_ASSERT_EQUAL(ret, 0);

   arb_free(&res);
   mpz_clear(z);
}

static void test_add_manual()
{
   const pair numbers[] = {
      {-1000, 100},
      {1, 10},
      {100, 1000},
      {1000, 100},
      {0, 0},
      {1000, 0},
      {0, 1000}
   };

   const size_t count = sizeof(numbers)/sizeof(numbers[0]);
   test_bin_op_helper_manual(numbers, count, add_bin_op);
}

static void test_add()
{
   test_bin_op_helper_random(add_bin_op);
}

static void sub_bin_op(const arb_int_t i1, const arb_int_t i2,
      const mpz_t z1, const mpz_t z2)
{
   arb_int_t res;
   CU_ASSERT_EQUAL(arb_duplicate(&res, i1), 0);
   CU_ASSERT_EQUAL(arb_subtract(res, i2), 0);

   mpz_t z;
   mpz_init(z);

   mpz_sub(z, z1, z2);

   CU_ASSERT_EQUAL(compare_mixed(res, z), 0);

   arb_free(&res);
   mpz_clear(z);
}

static void test_subtract_manual()
{
   const pair numbers[] = {
      {-10, -1},
      {-1, -10},
      {0, 0},
      {10, 10},
      {100, 100},
      {10, 1},
      {-10, 1},
      {10, -1},
      {1, 10},
      {-1, 10},
      {1, -10}
   };

   const size_t count = sizeof(numbers)/sizeof(numbers[0]);
   test_bin_op_helper_manual(numbers, count, sub_bin_op);
}


static void test_subtract()
{
   test_bin_op_helper_random(sub_bin_op);
}

static void mul_bin_op(const arb_int_t i1, const arb_int_t i2,
      const mpz_t z1, const mpz_t z2)
{
   arb_int_t res;
   CU_ASSERT_EQUAL(arb_duplicate(&res, i1), 0);
   CU_ASSERT_EQUAL(arb_multiply(res, i2), 0);

   mpz_t z;
   mpz_init(z);

   mpz_mul(z, z1, z2);

   //print_arb(i1);
   //print_arb(i2);
   CU_ASSERT_EQUAL(compare_mixed(res, z), 0);

   arb_free(&res);
   mpz_clear(z);
}

static void test_multiply_manual()
{
   const pair numbers[] = {
      {208, 208},
     {-10, -1},
      {-1, -10},
      {0, 0},
      {10, 10},
      {100, 100},
      {10, 1},
      {-10, 1},
      {10, -1},
      {1, 10},
      {-1, 10},
      {1, -10},
      {1, 11},
      {11, 1}, 
      {111, 11},
      {11, 111}
    };

   const size_t count = sizeof(numbers)/sizeof(numbers[0]);
   test_bin_op_helper_manual(numbers, count, mul_bin_op);
}

static void test_multiply()
{
   test_bin_op_helper_random(mul_bin_op);
}

static void div_bin_op(const arb_int_t i1, const arb_int_t i2,
      const mpz_t z1, const mpz_t z2)
{
   // Don't divide by 0
   if (!mpz_cmp_ui(z2, 0))
      return;

   arb_int_t res;
   CU_ASSERT_EQUAL(arb_duplicate(&res, i1), 0);
   //CU_ASSERT_EQUAL(arb_divide(res, i2), 0);

   mpz_t z;
   mpz_init(z);

   mpz_tdiv_q(z, z1, z2);

   CU_ASSERT_EQUAL(compare_mixed(res, z), 0);

   arb_free(&res);
   mpz_clear(z);
}

void test_divide()
{
   test_bin_op_helper_random(div_bin_op);
}


static void test_assign()
{
   arb_int_t numbers[100];

   const size_t numbercount = sizeof(numbers)/sizeof(numbers[0]);

   // Generate 100 random numbers
   for (unsigned int i=0; i<numbercount; ++i)
   {
      random_arb_int(1, VERY_LARGE, &numbers[i]);
   }

   arb_int_t final;
   arb_from_int (&final, 0);

   int pos;
   for (unsigned int i=0; i<1000; ++i)
   {
      pos = rand() % numbercount;
      CU_ASSERT_EQUAL(arb_assign(final, numbers[pos]), 0);
   }

   for (unsigned int i=0; i<numbercount; ++i)
   {
      if (i == pos)
         continue;
      arb_free(&numbers[i]);
   }

   CU_ASSERT_EQUAL(arb_compare_via_string(final, numbers[pos]), 0);
   arb_free(&numbers[pos]);
   arb_free(&final);
}

static void test_duplicate()
{
   const size_t numbercount = 100;
   arb_int_t numbers[numbercount];
   arb_int_t duplicates[numbercount];


   // Generate 100 random numbers
   for (unsigned int i=0; i<numbercount; ++i)
   {
      random_arb_int(1, VERY_LARGE, &numbers[i]);
   }

   for (unsigned int i=0; i<numbercount; ++i)
   {
      CU_ASSERT_EQUAL(arb_duplicate(&duplicates[i], numbers[i]), 0);
   }

   for (unsigned int i=0; i<numbercount; ++i)
   {
      CU_ASSERT_EQUAL(arb_compare_via_string(duplicates[i], numbers[i]), 0);
   }

   for (unsigned int i=0; i<numbercount; ++i)
   {
      arb_free(&numbers[i]);
      arb_free(&duplicates[i]);
   }
}

static void test_compare_manual()
{
   struct
   {
      long long int x;
      long long int y;
      int res;
   } tests[] = {
                  { -1204030870343478, -6132109214771891, 1 },
                  { 0, 0, 0},
                  { -1, 0, -1},
                  { 1, 0, 1},
                  { 1, 1, 0},
                  { -1, -2, 1},
                  { -2, -1, -1},
                  { 1, 2, -1},
                  { 2, 1, 1}
               };

   for (unsigned int i=0; i<(sizeof(tests)/sizeof(tests[0])); ++i)
   {
      arb_int_t x;
      arb_int_t y;
      CU_ASSERT_EQUAL(arb_from_int(&x, tests[i].x), 0);
      CU_ASSERT_EQUAL(arb_from_int(&y, tests[i].y), 0);

      int res = arb_compare(x, y);
      CU_ASSERT_EQUAL(tests[i].res, res);

      arb_free(&x);
      arb_free(&y);
   }
}

static void test_free()
{
   //struct mallinfo before = mallinfo();


   arb_int_t * x = malloc(sizeof(*x)*COUNT);
   for (unsigned int i=0; i<COUNT; ++i)
   {
      CU_ASSERT_EQUAL(arb_from_int(&x[i], rand()), 0);
   }
   for (unsigned int i=0; i<COUNT; ++i)
   {
      arb_free(&x[i]);
   }
   free(x);

   //struct mallinfo after = mallinfo();

   // this test is potentially too strict/simple
   // if the library does some form of caching
   //CU_ASSERT_EQUAL(before.uordblks, after.uordblks);
   //CU_ASSERT_EQUAL(before.hblkhd, after.hblkhd);
}

/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int main()
{
   srand(time(NULL));
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("Suite_1", init_suite, clean_suite);
   if (NULL == pSuite)
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
   if (
       (NULL == CU_add_test(pSuite, "arb_from_string()", test_from_string))
    || (NULL == CU_add_test(pSuite, "arb_duplicate()", test_duplicate))
    || (NULL == CU_add_test(pSuite, "arb_from_string() bad",
                                       test_from_string_bad))
    || (NULL == CU_add_test(pSuite, "arb_from_string() oversize",
                                       test_from_string_oversize))
    || (NULL == CU_add_test(pSuite, "arb_from_int()", test_from_int))
    || (NULL == CU_add_test(pSuite, "arb_from_int() boundary", test_from_int_boundary))
    || (NULL == CU_add_test(pSuite, "arb_free()", test_free))
    || (NULL == CU_add_test(pSuite, "arb_to_int() random", test_to_int_random))
    || (NULL == CU_add_test(pSuite, "arb_to_int()", test_to_int))
    || (NULL == CU_add_test(pSuite, "arb_to_int() boundary", test_to_int_boundary))
    || (NULL == CU_add_test(pSuite, "arb_to_int() boundary+-1", test_to_int_boundary1))
    || (NULL == CU_add_test(pSuite, "arb_compare() manual", test_compare_manual))
    || (NULL == CU_add_test(pSuite, "arb_compare()", test_compare))
    || (NULL == CU_add_test(pSuite, "arb_assign()", test_assign))
    || (NULL == CU_add_test(pSuite, "arb_add() manual", test_add_manual))
    || (NULL == CU_add_test(pSuite, "arb_add() random", test_add))
    || (NULL == CU_add_test(pSuite, "arb_subtract() manual", test_subtract_manual))
    || (NULL == CU_add_test(pSuite, "arb_subtract() random", test_subtract))
    || (NULL == CU_add_test(pSuite, "arb_to_int() oversize", test_to_int_oversize))
    || (NULL == CU_add_test(pSuite, "arb_multiply() manual", test_multiply_manual))
    || (NULL == CU_add_test(pSuite, "arb_multiply() random", test_multiply))
    // uncomment below if implementing division
    //|| (NULL == CU_add_test(pSuite, "arb_divide()", test_divide))
      )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}


