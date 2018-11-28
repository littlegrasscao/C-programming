My arb_int truct contains following contents: 
''''
struct arb_int{
    char *number;               //char pointer used to store integer number
    unsigned int length;        //length of integers
    unsigned int capacity;      //capacity of memory space of pointers
};
typedef struct arb_int arb_int;
typedef struct arb_int * arb_int_t;
'''
By doing two typedefs, I make arb_int as a struct and arb_int_t as a struct pointer.
So when I pass arb_int_t x into the fucntion, I am passing a pointer to a struct.
When I pass arb_int_t *new into the fucntion, I am passing a pointer to a pointer to a struct.
Since we want to do pass-by-reference and some functions are passed by arb_int_t x, make arb_int_t as a pointer is necessary.

//These five functions are used in arb_add and arb_subtract to help calculate
    int determinesign(arb_int_t x);     //determine sign of arb_int x
    int xgreatery(char* x,char* y);     //determine if x is greater than y. Return 1 if true, 0 if equal, -1 is false.
    char *TwoPositiveAdd(char* x,char* y);    //add two positive char number x and char number y, return char number x+y
    char *TwoPositiveSub(char* x,char* y);    //subtract two positive x and y, return x-y, where x>=y.
    void copystring(arb_int_t x, char* tmp, int sign);  //copy temporary result from TwoPositiveAdd or TwoPositiveSub into arb_int_t x. Sign matters.

//arb_add
x is the number of arb_int_t x, y is the number of arb_int_t y. 
If both x and y are positive or negative, add equals to sign*(abs(x)+abs(y)). Make x and y become two positive integers, call tmp=TwoPositiveAdd(x,y). call copystring(x,tmp,sign).
If x and y have different signs, make x and y become two positive integers, call xgreatery(x,y) to see which one is greater. Then call tmp=TwoPositiveSub(x,y) or tmp=TwoPositiveSub(y,x), where the first parameter is the greater one. 
When x=-y or -x=y, simply make tmp=0;
**use logical if else to control sign when copy tmp into x through function copystring().


//arb_subtract
x is the number of arb_int_t x, y is the number of arb_int_t y. 
If x and y have the same signs, make x and y become two positive integers, call xgreatery(x,y) to see which one is greater. Then call tmp=TwoPositiveSub(x,y) or tmp=TwoPositiveSub(y,x), where the first parameter is the greater one. 
When x=y, simply make tmp=0;
**use logical if else to control sign when copy tmp into x through function copystring().
If x negative and y positive, subtarct is equal to -(x+y). Simply call tmp=TwoPositiveAdd(x->number+1,y->number) and copystring(x,tmp,-1).
If x positive and y negative, subtarct is equal to (x+y). Simply call tmp=TwoPositiveAdd(x->number,y->number+1) and copystring(x,tmp,1).


//arb_multiply
Basic idea is to achieve x*y by adding x to x in y times.
char *TwoPositiveMul(char* x,char* y);  //times two positive char number x and char number y, return char number x*y
The following function is the key idea to calculate x*y. After the function, sum is already calculated, just find the first non-zero element and return from it to the end.
"""
//use two for loops to time every digit in y to x. Add tmp%10 to sum[i+j+1] and carry=tmp/10 in each multiply.
    for (int i=lx-1; 0<=i; --i) {
        int carry = 0;
        for (int j=ly-1; 0<=j; --j) {
            int tmp = (sum[i+j+1]-'0') + (x[i]-'0') * (y[j]-'0') + carry;
            sum[i+j+1] = tmp%10 + '0';
            carry = tmp/10;
        }
        sum[i] += carry;
    }
"""
x is the number of arb_int_t x, y is the number of arb_int_t y. 
make x and y become two positive integers, apply tmp=TwoPositiveMul(x,y).use logical if else to control sign when copy tmp into x through copystring(x,tmp,sign).


//arb_compare
use determinesign and xgreatery.
compare sign first. if sign is the same, use xgreatery to compare x and y.

//arb_digit_count
'-' is already stored in the char*number in the struct. So I only need to return arb_int_t x->length.


//arb_from_string
In this way, when I do from_string or from_int, I need to malloc both the *i and (*i)->number.
First do error checking, return 1 if any unaccepted input.
Find length and copy s to (*i)->number one by one. Make sure to include '-' if negative.

//arb_from_int
The input shold always be a long long int, otherwise the system itself will report an error.
get the length of source and its sign. add sign to (*i)->number if '-'.
mod source by 10, add the result from end to beginning to number. Divide source by 10 after each add.

//arb_to_string
error checking.
copy straght forward.

//arb_to_int
error checking using arb_subtract and arb_add. It is faster and cleaner than comapre every digit.
add number to out from beginning to end one by one. out*=10 after each add.

//arb_free
In arb_free, I need to free both *i and (*i)->number.

//arb_duplicate
malloc *new and (*new)->number.
copy length, capacity from origiinal to new.
use a strcpy to copy the content (*new)->number pointers to.

//arb_assign
realloc x->number and copy y->number to x->number.

