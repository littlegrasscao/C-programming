#include "arb_int.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc==1)
    {
        //test from_string
        arb_int_t a;
        arb_int_t b;
        arb_from_string(&a, "+12345123451234512345999");
        arb_from_string(&b, "-2245");

        //test from_int
        signed long long int x=-922203685417;
        arb_int_t c;
        arb_from_int(&c, x);
        signed long long int y=5417;
        arb_int_t d;
        arb_from_int(&d, y);

        //test duplicate
        arb_int_t e;
        arb_duplicate (&e, a);
        //test assign
        arb_assign (d,b);

        //test to_string
        char output1[10];
        arb_to_string(b, output1, sizeof(output1));
        char output2[100];
        arb_to_string(b, output2, sizeof(output2));

        //test to_int
        long long int num=0;
        long long int *out=&num;
        arb_to_int (b, out);
        arb_to_int (a, out);

        //e+=b
        arb_add(e,b);

        //b-=c
        arb_subtract(b,c);
        
        //a*=d
        arb_multiply(a,d);
        
        //compare a and b
        arb_compare(a,b);
        
        //return digits of a
        arb_digit_count(a);

        //free
        arb_free (&a);
        arb_free (&b);
	arb_free (&c);
        arb_free (&d);
        arb_free (&e);
        return EXIT_SUCCESS;
    }
    else if(argc!=4)
    {
        fprintf(stderr, "Error: Must enter zero or three arguments!\n");
        return 0;        
    }
    else{
        //initialize
        arb_int_t a;
        arb_int_t b;
        //check if first input number is valid or not
        int x = arb_from_string(&a, argv[1]);
        if(x!=0)
        {
            fprintf(stderr, "Error: %s is not a valid integer!\n",argv[1]);
            return 0;
        }
        //check if second input number is valid or not
        int y = arb_from_string(&b, argv[3]); 
        if(y!=0)
        {
            fprintf(stderr, "Error: %s is not a valid integer!\n",argv[3]);
            arb_free(&a);
            return 0;
        }
        //check if operation is valid
        if (strcmp(argv[2],"+")!=0 && strcmp(argv[2],"-")!=0 && strcmp(argv[2],"*")!=0)
        {
            fprintf(stderr, "Error: %s is not a operation in +/-/*!\n",argv[2]);
            arb_free(&a);
            arb_free(&b);
            return 0;            
        }

        //printf("a:%s\n",a->number);
        //printf("b:%s\n",b->number);

        //if a+b
        if (strcmp(argv[2],"+")==0)
        {
            arb_add(a,b);
            printf("%s\n",a->number);
        }
        //if a-b
        if (strcmp(argv[2],"-")==0)
        {
            arb_subtract(a,b);
            printf("%s\n",a->number);
        }
        //a*b
        if (strcmp(argv[2],"*")==0)
        {
            arb_multiply(a,b);
            printf("%s\n",a->number);
        }

        //free 
        arb_free (&a);
        arb_free (&b);   

        return 0;     
    }
}

