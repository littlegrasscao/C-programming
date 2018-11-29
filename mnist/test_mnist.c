/*********************************************************
 *  Unit tests for varstring (also used for grading!)
 *
 *  DO NOT MODIFY
 *********************************************************/

#include "mnist.h"

#include <CUnit/Basic.h>

#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <stdbool.h>
#include <malloc.h>
#include <time.h>
#include <ctype.h>

#define MNIST_DATASET_INVALID ((mnist_dataset_handle) 0)
#define MNIST_IMAGE_INVALID ((mnist_image_handle) 0)

// For certain tests, COUNT^2 (binary ops) are tested
#define COUNT 10


// Largest number generated for the test (in digits) *before*
// performing operation.
#define VERY_LARGE 80


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

//test to open three cases
static void test_mnist_open()
{
   struct E
   {
      const char * s;
      mnist_dataset_handle v;
   };
   struct E tests[] = {
      {"train", MNIST_DATASET_INVALID},   //valid case
      {"t10k", MNIST_DATASET_INVALID},    //valid case
      {"incorrect", MNIST_DATASET_INVALID},   //invalid case
   };

   //should not equal MNIST_DATASET_INVALID
   mnist_dataset_handle x;
   x = mnist_open(tests[0].s);
   CU_ASSERT_NOT_EQUAL(x, tests[0].v);         

   //should not equal MNIST_DATASET_INVALID
   mnist_dataset_handle y;
   y = mnist_open(tests[1].s);
   CU_ASSERT_NOT_EQUAL(y, tests[1].v);  

   //should equal MNIST_DATASET_INVALID
   mnist_dataset_handle z;
   z = mnist_open(tests[2].s);
   CU_ASSERT_EQUAL(z, tests[2].v);

   //free
   mnist_free(x);
   mnist_free(y);
   mnist_free(z);
}

//test three cases with k = 80
static void test_mnist_open_sample()
{
   struct E
   {
      const char * s;
      unsigned int k;
      mnist_dataset_handle v;
   };
   struct E tests[] = {
      {"train", VERY_LARGE, MNIST_DATASET_INVALID},
      {"t10k", VERY_LARGE, MNIST_DATASET_INVALID},
      {"incorrect", VERY_LARGE, MNIST_DATASET_INVALID},
   };

   //should open
   mnist_dataset_handle x;
   x = mnist_open_sample(tests[0].s, tests[0].k);
   CU_ASSERT_NOT_EQUAL(x, tests[0].v);         

   //should open
   mnist_dataset_handle y;
   y = mnist_open_sample(tests[1].s, tests[1].k);
   CU_ASSERT_NOT_EQUAL(y, tests[1].v);  

   //should open fail 
   mnist_dataset_handle z;
   z = mnist_open_sample(tests[2].s, tests[2].k);
   CU_ASSERT_EQUAL(z, tests[2].v);

   //free
   mnist_free(x);
   mnist_free(y);
   mnist_free(z);
}

static void test_mnist_create()
{
   //test random x and y
   for (unsigned int c=0; c<COUNT; ++c)
   {
      //generate random x and y
      srand(time(NULL));
      unsigned int x = (rand() & 0xff);
      unsigned int y = (rand() & 0xff);

      //check if create works for random x and y
      mnist_dataset_handle h = mnist_create(x,y);
      CU_ASSERT_NOT_EQUAL(h, MNIST_DATASET_INVALID);

      mnist_free(h);
   }

}

//check the image count for train and test data
static void test_mnist_image_count()
{
   struct E
   {
      const char * s;
      int k;
      mnist_dataset_handle v;
   };
   struct E tests[] = {
      {"train", 60000, MNIST_DATASET_INVALID},
      {"t10k", 10000, MNIST_DATASET_INVALID},
   };

   //should not equal MNIST_DATASET_INVALID
   mnist_dataset_handle x;
   x = mnist_open(tests[0].s);
   CU_ASSERT_NOT_EQUAL(x, tests[0].v); 
   CU_ASSERT_EQUAL(mnist_image_count(x), tests[0].k);       //image count should equal    

   //should not equal MNIST_DATASET_INVALID
   mnist_dataset_handle y;
   y = mnist_open(tests[1].s);
   CU_ASSERT_NOT_EQUAL(y, tests[1].v);  
   CU_ASSERT_EQUAL(mnist_image_count(y), tests[1].k);         //image count should equal 


   //free
   mnist_free(x);
   mnist_free(y);  

   //test random k
   for (unsigned int c=0; c<COUNT; ++c)
   {
      //generate random x and y
      srand(time(NULL));
      unsigned int k = (rand()%60000);

      //test with open_sample
      mnist_dataset_handle z;
      z = mnist_open_sample("train", k);   
      
      CU_ASSERT_EQUAL(mnist_image_count(z), k);

      mnist_free(z); 
   }

}

static void test_mnist_image_size()
{
   //test random x and y
   for (unsigned int c=0; c<COUNT; ++c)
   {
      //generate random x and y
      srand(time(NULL));
      unsigned int x = (rand() & 0xff);
      unsigned int y = (rand() & 0xff);

      //check if create works for random x and y
      mnist_dataset_handle handle;
      handle = mnist_create(x,y);
      CU_ASSERT_NOT_EQUAL(handle, MNIST_DATASET_INVALID);

      //check return of image size
      unsigned int a;
      unsigned int b;

      mnist_image_size(handle, &a, &b);
      CU_ASSERT_EQUAL(a, x);
      CU_ASSERT_EQUAL(b, y);

      mnist_free(handle);
   }
}


static void test_mnist_image_begin()
{
   //check invalid dataset
   mnist_dataset_handle x = MNIST_DATASET_INVALID;
   CU_ASSERT_EQUAL(mnist_image_begin(x), MNIST_IMAGE_INVALID);

   //check invalid imageset
   mnist_dataset_handle y = mnist_create(28,28);
   CU_ASSERT_EQUAL(mnist_image_begin(y), MNIST_IMAGE_INVALID);
   mnist_free(y);

   //check valid 
   mnist_dataset_handle z = mnist_open("t10k");
   CU_ASSERT_NOT_EQUAL(z, MNIST_DATASET_INVALID);

   CU_ASSERT_NOT_EQUAL(mnist_image_begin(z), MNIST_IMAGE_INVALID);   
   mnist_free(z);
}


static void test_mnist_image_data()
{
   const unsigned char * imagedata;
   
   //open t10k
   mnist_dataset_handle z = mnist_open("t10k");
   CU_ASSERT_NOT_EQUAL(z, MNIST_DATASET_INVALID);

   //find begin
   mnist_image_handle h;
   h = mnist_image_begin(z);
   CU_ASSERT_NOT_EQUAL(h, MNIST_IMAGE_INVALID);  

   //should be valid
   imagedata = mnist_image_data(h);
   CU_ASSERT_NOT_EQUAL(imagedata, NULL);

   mnist_free(z);

   //test invalid image set
   mnist_image_handle h2 = MNIST_IMAGE_INVALID;
   imagedata = mnist_image_data(h2);
   CU_ASSERT_EQUAL(imagedata, NULL);

}


static void test_mnist_image_label()
{
   int label;

   //test invalid image set
   mnist_image_handle wrong = MNIST_IMAGE_INVALID;
   label = mnist_image_label(wrong);
   CU_ASSERT(label < 0);

   //open t10k
   mnist_dataset_handle z = mnist_open("t10k");
   CU_ASSERT_NOT_EQUAL(z, MNIST_DATASET_INVALID);

   //find begin
   mnist_image_handle h;
   h = mnist_image_begin(z);
   CU_ASSERT_NOT_EQUAL(h, MNIST_IMAGE_INVALID);

   label = mnist_image_label(h);
   CU_ASSERT_EQUAL(label,7);  //the first label in t10k is 7

   mnist_free(z);    
}



static void test_mnist_set_image_label()
{
   int out;
   //test invalid image set
   mnist_image_handle wrong = MNIST_IMAGE_INVALID;
   out = mnist_set_image_label(wrong,1);
   assert(out<0);   

   //open t10k
   mnist_dataset_handle z = mnist_open("t10k");
   CU_ASSERT_NOT_EQUAL(z, MNIST_DATASET_INVALID);

   //find begin
   mnist_image_handle h;
   h = mnist_image_begin(z);
   CU_ASSERT_NOT_EQUAL(h, MNIST_IMAGE_INVALID);

   //test valid set label
   out = mnist_set_image_label(h,1);
   assert(out>=0);   

   mnist_free(z);  
}

static void test_mnist_image_next()
{
   //open t10k
   mnist_dataset_handle z = mnist_open("t10k");
   CU_ASSERT_NOT_EQUAL(z, MNIST_DATASET_INVALID);

   //find begin
   mnist_image_handle h;
   h = mnist_image_begin(z);
   CU_ASSERT_NOT_EQUAL(h, MNIST_IMAGE_INVALID);

   //continue next until the last one
   for(int c=0; c<10000; c++)
   {
      h = mnist_image_next(h);
   }
   CU_ASSERT_EQUAL(mnist_image_next(h), MNIST_IMAGE_INVALID);

   mnist_free(z); 
}

static void test_mnist_image_prev()
{
   //open t10k
   mnist_dataset_handle z = mnist_open("t10k");
   CU_ASSERT_NOT_EQUAL(z, MNIST_DATASET_INVALID);

   //find begin
   mnist_image_handle h;
   h = mnist_image_begin(z);
   CU_ASSERT_NOT_EQUAL(h, MNIST_IMAGE_INVALID);

   CU_ASSERT_EQUAL(mnist_image_prev(h),MNIST_IMAGE_INVALID); //test first

   CU_ASSERT_EQUAL(mnist_image_prev(mnist_image_next(h)),h); //test next then previous

   mnist_free(z);  
}

static void test_mnist_image_add_after()
{
   //open t10k
   mnist_dataset_handle z = mnist_open("t10k");
   CU_ASSERT_NOT_EQUAL(z, MNIST_DATASET_INVALID);

   //find begin
   mnist_image_handle h;
   h = mnist_image_begin(z);
   CU_ASSERT_NOT_EQUAL(h, MNIST_IMAGE_INVALID);

   //find image data
   const unsigned char* image;
   image = mnist_image_data(h);

   //add after the first
   mnist_image_add_after(z,h,image,28,28);

   //add as the first
   mnist_image_handle tmp = MNIST_IMAGE_INVALID;
   CU_ASSERT_EQUAL(mnist_image_add_after(z,tmp,image,28,28),mnist_image_begin(z));

   //invalid 
   CU_ASSERT_EQUAL(mnist_image_add_after(z,h,image,38,38),MNIST_IMAGE_INVALID);

   mnist_free(z);  
}

static void test_mnist_image_remove()
{
   //open t10k
   mnist_dataset_handle z = mnist_open("t10k");
   CU_ASSERT_NOT_EQUAL(z, MNIST_DATASET_INVALID);

   //find begin
   mnist_image_handle h;
   h = mnist_image_begin(z);
   CU_ASSERT_NOT_EQUAL(h, MNIST_IMAGE_INVALID);

   //get the second
   mnist_image_handle tmp;
   tmp = mnist_image_next(h);

   h = mnist_image_remove(z,mnist_image_begin(z)); //remove first image data

   CU_ASSERT_EQUAL(tmp,h); //test next then previous

   mnist_free(z);  
}

static void test_mnist_save()
{
   struct E
   {
      const char * s;
      const char * t;
      bool v;
   };
   struct E tests[] = {
      {"train","train_save", true},   //valid case
      {"t10k","t10k_save", true},    //valid case
   };

   //test two open files
 	for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i ++)
	{
      mnist_dataset_handle x;
      x = mnist_open(tests[i].s); //open
      
      CU_ASSERT_EQUAL(mnist_save(x, tests[i].t), tests[i].v); //test save

      mnist_free(x);
	}
 
   mnist_dataset_handle y = MNIST_DATASET_INVALID;
   CU_ASSERT_EQUAL(mnist_save(y, "incorrect"), false);
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
       (NULL == CU_add_test(pSuite, "mnist_open()", test_mnist_open))
      || (NULL == CU_add_test(pSuite, "mnist_open_sample()", test_mnist_open_sample))
      || (NULL == CU_add_test(pSuite, "mnist_create()", test_mnist_create))
      || (NULL == CU_add_test(pSuite, "mnist_image_count()", test_mnist_image_count))
      || (NULL == CU_add_test(pSuite, "mnist_image_size()", test_mnist_image_size))
      || (NULL == CU_add_test(pSuite, "mnist_image_begin()", test_mnist_image_begin))
      || (NULL == CU_add_test(pSuite, "mnist_image_data()", test_mnist_image_data))
      || (NULL == CU_add_test(pSuite, "mnist_image_label()", test_mnist_image_label))
      || (NULL == CU_add_test(pSuite, "mnist_set_image_label()", test_mnist_set_image_label))
      || (NULL == CU_add_test(pSuite, "mnist_image_next()", test_mnist_image_next))
      || (NULL == CU_add_test(pSuite, "mnist_image_prev()", test_mnist_image_prev))
      || (NULL == CU_add_test(pSuite, "mnist_image_add_after()", test_mnist_image_add_after))
      || (NULL == CU_add_test(pSuite, "mnist_image_remove()", test_mnist_image_remove))
      || (NULL == CU_add_test(pSuite, "mnist_save()", test_mnist_save))
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


