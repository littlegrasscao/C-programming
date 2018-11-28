#include "arb_int.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

// Free arb_int_t
void arb_free (arb_int_t * i)
{
    //free strcut and string
    free((*i)->number);
    free(*i);
}

// Duplicate arb_int_t. The new arb_int_t, when no longer needed,
// needs to be freed using arb_free. The original arb_int_t is unmodified.
// Returns zero on success, non-zero otherwise
int arb_duplicate (arb_int_t * new, const arb_int_t original)
{
    if(original->number==NULL) return 1;  //malloc will fail 

    *new = malloc(sizeof(arb_int));
    //copy original to new
    (*new)->length = original->length;
    (*new)->capacity = original->capacity;

    //malloc i->number and copy integer into it
    (*new)->number=(char*)malloc(sizeof(char)*((*new)->capacity));

    //copy
    strcpy((*new)->number,original->number);
    return 0;
}


// Convert from string representation into arb_int_t.
// Return zero and set *i to the new arb_int_t if successful,
// do not change *i and return non-zero otherwise.
// The following forms should be accepted:
// +2323 -232323 +0 -0 23232
// If any other characters than 0-9,+,- are present you should
// return an error (i.e. nonzero).
int arb_from_string (arb_int_t * i, const char * s)
{
    //error checking        
    if (s==NULL) return 1;
    
    unsigned int ls=strlen(s);	//length of s
    if(ls==0) return 1;        
    //return error if s='+',"01","0002","+00"
    else if((ls==1 && isdigit(s[0])==0) || (ls>1&&s[0]=='0') || (ls>2&&isdigit(s[0])==0&&s[1]=='0')) return 1;      	
    else{
	    //go through string s and determine if s is valid input
	    for(unsigned int x=0;x<ls;x++)
	    {
	        if (x==0 && (s[x]=='+' || s[x]=='-')) continue;  
	        //if input string contain non-digit letter, print error
	        if(isdigit(s[x])==0) return 1;  
	    }    	
    }

    unsigned int j=0,k=0;      //used in for loop
    //check if the first letter is +
    if (s[0]=='+')
    {
        k++;
        ls--;
    }
    //malloc
    *i = malloc(sizeof(arb_int));
    (*i)->length=ls;          //initialize length of i
    (*i)->capacity=ls+1;      //initialize capacity of i
    //malloc space
    (*i)->number=malloc(sizeof(char)*((*i)->capacity));

    //go through string s again and make a copy
    for(;j<ls;j++,k++)
    {
        //copy s to i->number
        (*i)->number[j]=s[k];
    }
    //add 0 in the end
    (*i)->number[ls]='\0';

    return 0;
}

// Convert from long long int to arb_int_t
// Return zero on success, non-zero otherwise.
int arb_from_int (arb_int_t * i, signed long long int source)
{
    int mod;                            //result of source%10
    unsigned int l=0;                              //length of source
    signed long long int n=source;      //make a copy of source
    int sign=1;                         //default positive

    //if source==0, length==1
    if (n==0)
        l=1;

    //get the length of source
    while(n!=0)
    {
        l++;
        n /= 10;
    }

    //initialize sign of i
    if(source<0)
    {
        sign=-1;
        l++;               //need to store '-'
    }

    //malloc space
    *i=malloc(sizeof(arb_int));

    (*i)->length=l;          //initialize length of i
    (*i)->capacity=l+1;      //initialize capacity of i
    //malloc space
    (*i)->number=(char*)malloc(sizeof(char)*((*i)->capacity));
     
    //copy source to i->number
    for(unsigned int x=0;x<l;x++){
        mod = source%10*sign;                //get last digit
        source /= 10;                   //divide by 10
        (*i)->number[l-(x+1)]=(char)(mod+'0');   //we need to add from end to beginning 
    }

    //if negative, add '-' at the beginning
    if(sign==-1)
    {
        (*i)->number[0]='-';
    }

    //add 0 in the end
    (*i)->number[l]='\0';

    return 0;    
}


// Convert arb_int_t to string
// Return zero on success, or non-zero if buf is not big enough.
// Make sure buf is always properly zero-terminated.
// max is the maximum number of bytes (including the terminating
// zero) than can be written to buf.
// Negative numbers should have a leading ¡¯-¡¯; Positive numbers
// *should not* have a +. 0 is considered to be positive.
int arb_to_string (const arb_int_t i, char * buf, size_t max)
{
    //if buf is too small
    if(max < i->capacity) return 1;          
    
    //copy i to buf
    for(unsigned int x=0;x<i->length;x++)
    {
        buf[x]=i->number[x];
    }
    buf[i->length]='\0';
    
    return 0;
}


// Convert arb_int_t to long long int
// Return zero on success, non-zero otherwise.
// (for example if i > LLONG_MAX or < LLONG_MIN)
int arb_to_int (const arb_int_t i, long long int * out)
{
    //declare function will be used
    void arb_free (arb_int_t * i);
    int arb_from_int (arb_int_t * i, signed long long int source);
    int arb_subtract (arb_int_t x, const arb_int_t y);
    int arb_add(arb_int_t x, const arb_int_t y);

    //set sign default as 1
    int sign=1;

    arb_int_t llmax;
    arb_from_int(&llmax, LLONG_MAX);

    if (i->number[0]=='-')
    {
        arb_int_t addone;
        arb_from_int(&addone, 1);
        //return false if LLONG_MAX+1+i<0
        arb_add(llmax,addone); 
        arb_add(llmax,i);
        //free addone
        arb_free(&addone);
        //printf("llim:%s\n",llmin->number);
        if(llmax->number[0]=='-')
        {
            arb_free(&llmax);
            return 1; 
        }
        //set sign
        sign=-1;
    }
    else
    {
        //return false if LLONG_MAX-i<0
        arb_subtract(llmax,i);
        if(llmax->number[0]=='-')
        {
            arb_free(&llmax);
            return 1; 
        } 
    }   
    arb_free(&llmax);

    //set out to 0
    *(out)=0;
    //add i.number to out
    for(unsigned int x=0;x<i->length;x++)
    {
        //if negetive, times -1
        if(x==0 && i->number[x]=='-')
        {
            sign=-1;
            continue;
        }
        //add i.number to out from the end 
        *(out)*=10;
        *(out)+=((int)(i->number[x]-'0'));  
    }
    //sign
    *(out)*=sign;

    return 0;
}


// Assign the numeric value of arb_int_t y to arb_int_t x. x and y have
// to be separate arb_int_t¡¯s construced by one of the functions above.
// Note that
// x and y remain separate (i.e. changing x does not change y).
// This function differs from arb_duplicate in that both x and y
// have to be created already *before* calling this function.
// Returns 0 if success, return non-zero otherwise. In the latter case
// x and y remain unmodified.
int arb_assign (arb_int_t x, const arb_int_t y)
{
    //error if inputs are empty
    if( x->number == NULL || y->number==NULL) return 1; 

    //update length and capacity, realloc char*number
    x->length = y->length;
    x->capacity = x->length+1;
    x->number=realloc(x->number,sizeof(char)*(x->capacity));
    //add y to x
    memcpy(x->number,y->number,x->capacity);
    return 0;
}

//determine sign of arb_int_t
int determinesign(arb_int_t x)
{
    int signx;
    if (x->number[0]=='-')
        signx=-1;
    else
        signx=1;
    return signx;
}

//determine if x is greater than y. Return 1 if true, 0 if equal, -1 is false.
int xgreatery(char* x,char* y)
{
    //comapre legth of x and y first
    if (strlen(x)>strlen(y))
        return 1;
    else if (strlen(x)<strlen(y))
        return -1;
    //compare each element within x and y
    for (int i=0;i<strlen(x);i++)
    {
        if(x[i]>y[i])
            return 1;
        else if (x[i]<y[i])
            return -1;
    }
    //return 0 if equal
    return 0;
}

//add two positive x and y, return x+y
char *TwoPositiveAdd(char* x,char* y)
{
    char *tmp; //temporary store the add result
    unsigned int tmpcap;  //space for tmp
    //decide the longer one between x and y.
    if (strlen(x) < strlen(y))
        tmpcap=strlen(y)+2;
    else
        tmpcap=strlen(x)+2;
    //malloc space for tmp
    tmp=malloc(sizeof(char)*tmpcap);

    int carry=0; //set a carry
    int add;     //set a add
    int i=strlen(x)-1, j=strlen(y)-1, k=tmpcap-2;  //set index i,j,k for x,y and tmp

    //while both x and y have value
    //basic idea is adding each element one by one from back to front. If greater than ten, increment carry. 
    while(i>=0 && j>=0){
        add = (int)(x[i]-'0') + (int)(y[j]-'0') + carry;
        carry=add/10;
        tmp[k]=(add%10)+'0';
        i-=1;j-=1;k-=1;
    }    
    //only x left value
    while (i>=0)
    {
        add = (int)(x[i]-'0') + carry;
        carry=add/10;
        tmp[k]=(add%10)+'0';
        i--;k--;        
    }
    //only y left value
    while (j>=0)
    {
        add = (int)(y[j]-'0') + carry;
        carry=add/10;
        tmp[k]=(add%10)+'0';
        j--;k--;
    }
    //if still have carry, first element should be 1
    if (carry==1)
        tmp[0]='1';
    else
        tmp[0]='0';
    //add an end note
    tmp[tmpcap-1]='\0';

    return tmp;
}

//subtract two positive x and y, return x-y, where x>=y.
char *TwoPositiveSub(char* x,char* y)
{
    char *tmp; //temporary store the add result
    unsigned int tmpcap=strlen(x)+1;  //space for tmp

    //malloc space for tmp
    tmp=malloc(sizeof(char)*tmpcap);

    int carry=0; //set a carry
    int sub;     //set a subtract
    int i=strlen(x)-1, j=strlen(y)-1, k=tmpcap-2;  //set index i,j,k for x,y and tmp

    //while both x and y have value
    //basic idea is adding each element one by one from back to front. If greater than ten, increment carry. 
    while(i>=0 && j>=0){
        if ((int)(x[i]-'0') >= ((int)(y[j]-'0')+carry))
        {
            sub = (int)(x[i]-'0') - ((int)(y[j]-'0')+carry);
            carry=0;
            tmp[k] = sub+'0';
        }
        else
        {
            sub = 10 + (int)(x[i]-'0') - ((int)(y[j]-'0')+carry);
            carry=1;
            tmp[k] = sub+'0';
        }
        i--;j--;k--;
    }    
    //only x left value
    while (i>=0)
    {
        if ((int)(x[i]-'0') >= carry)
        {
            sub = (int)(x[i]-'0') - carry;
            carry=0;
            tmp[k] = sub+'0';
        }
        else
        {
            sub = 10 + (int)(x[i]-'0') - carry;
            carry=1;
            tmp[k] = sub+'0';
        }
        i--;k--;      
    }
    //add an end note
    tmp[tmpcap-1]='\0';

    return tmp; 
}

//copy string tmp to x, sign indicates positive or negative
void copystring(arb_int_t x, char* tmp, int sign)
{
    unsigned int i=0,j=0; //index used in copy. i for x->number and j for tmp.

    x->length=strlen(tmp);
    if(strlen(tmp)>1)
    {
        while (tmp[j]=='0')
        {
            //if tmp='0000', j stops at the last index
            if(j==strlen(tmp)-1)
                break;
            j++;
            x->length--;
        }
    }

    //if negative, needs one more space and put '-' at the beginning
    //if only '0' left, ingore negative sign.
    if (sign==-1 && tmp[j]!='0')
    {
        x->length++;
        x->number[i]='-';
        i++;
    }
    //capacity
    x->capacity=x->length+1;
    //realloc space for number
    x->number=(char*)realloc(x->number,sizeof(char)*x->capacity);

    //copy tmp to x->number
    for(;j<strlen(tmp);i++,j++)
    {
        x->number[i]=tmp[j];
    }
    //add end
    x->number[x->length]='\0';
}

// Add one arb_int_t to another one, modifying one of them.
// (x += y)
// Return zero on success, non-zero otherwise
int arb_add (arb_int_t x, const arb_int_t y){
    //error if inputs are empty
    if( x == NULL || y==NULL) return 1; 
    if( x->number == NULL || y->number==NULL) return 1; 

    /*All the functions i would use
    int determinesign(arb_int_t x);
    int xgreatery(char* x,char* y);
    char *TwoPositiveAdd(char* x,char* y);
    char *TwoPositiveSub(char* x,char* y);
    void copystring(arb_int_t x, char* tmp, int sign);
    */

    //determine sign of x and sign of y
    int signx,signy;
    signx=determinesign(x);
    signy=determinesign(y);  

    char *tmp; //temporary store the add result

    //two positive
    if (signx==1 && signy==1)
    {
        tmp=TwoPositiveAdd(x->number,y->number);
        copystring(x,tmp,1);
    }
        
    //two negatives
    else if (signx==-1 && signy==-1)
    {
        tmp=TwoPositiveAdd(x->number+1,y->number+1);
        copystring(x,tmp,-1);
    }   

    //x negative and y positive
    else if (signx==-1 && signy==1)
    {
        if (xgreatery(x->number+1,y->number)==1)        //abs(x)>abs(y)
        {
            tmp=TwoPositiveSub(x->number+1,y->number);  //tmp=negative
            copystring(x,tmp,-1);
        }
        else if(xgreatery(x->number+1,y->number)==-1)   //abs(x)<abs(y)
        {
            tmp=TwoPositiveSub(y->number,x->number+1);  //tmp=positive
            copystring(x,tmp,1);
        }
        else
        {   
            tmp=(char*)malloc(sizeof(char)*2);
            tmp[0]='0';    //if abs(x)==abs(y), tmp=0
            tmp[1]='\0';
            copystring(x,tmp,1);
        }
            

    }
    //x positive and y negative
    else if (signx==1 && signy==-1)
    {
        if (xgreatery(x->number,y->number+1)==1)    //abs(x)>abs(y)
        {
            tmp=TwoPositiveSub(x->number,y->number+1);  //tmp=positive
            copystring(x,tmp,1);
        }
        else if(xgreatery(x->number,y->number+1)==-1) //abs(x)<abs(y)
        {
            tmp=TwoPositiveSub(y->number+1,x->number); //tmp=negative
            copystring(x,tmp,-1);
        }
        else
        {
            tmp=(char*)malloc(sizeof(char)*2);
            tmp[0]='0';    //if abs(x)==abs(y), tmp=0
            tmp[1]='\0';
            copystring(x,tmp,1);
        }
    }
    //free tmp
    free(tmp);

    return 0;
}


// Subtract an arb_int_t from another.
// (x -= y);
// Return zero on success, non-zero otherwise
int arb_subtract (arb_int_t x, const arb_int_t y)
{
    //error if inputs are empty
    if( x == NULL || y==NULL) return 1; 
    if( x->number == NULL || y->number==NULL) return 1; 

    /*All the functions i would use
    int determinesign(arb_int_t x);
    int xgreatery(char* x,char* y);
    char *TwoPositiveAdd(char* x,char* y);
    char *TwoPositiveSub(char* x,char* y);
    void copystring(arb_int_t x, char* tmp, int sign);
    */

    //determine sign of x and sign of y
    int signx,signy;
    signx=determinesign(x);
    signy=determinesign(y);  

    char *tmp; //temporary store the add result

    //two positive
    if (signx==1 && signy==1)
    {
        if (xgreatery(x->number,y->number)==1)        //x>y
        {
            tmp=TwoPositiveSub(x->number,y->number);  //tmp=positive
            copystring(x,tmp,1);
        }
        else if(xgreatery(x->number,y->number)==-1)  //x<y
        {
            tmp=TwoPositiveSub(y->number,x->number); //tmp=negative
            copystring(x,tmp,-1);
        }
        else
        {
            //if x==y, tmp=0
            tmp=(char*)malloc(sizeof(char)*2);
            tmp[0]='0';    
            tmp[1]='\0';
            copystring(x,tmp,1);
        }        
    }
        
    //two negatives
    else if (signx==-1 && signy==-1)
    {
        if (xgreatery(x->number+1,y->number+1)==1)        //x>y
        {
            tmp=TwoPositiveSub(x->number+1,y->number+1);  //tmp=positive
            copystring(x,tmp,-1);
        }
        else if(xgreatery(x->number+1,y->number+1)==-1)  //x<y
        {
            tmp=TwoPositiveSub(y->number+1,x->number+1); //tmp=negative
            copystring(x,tmp,1);
        }
        else
        {
            //if x==y, tmp=0
            tmp=(char*)malloc(sizeof(char)*2);
            tmp[0]='0';    
            tmp[1]='\0';
            copystring(x,tmp,1);
        } 
    }   

    //x negative and y positive
    else if (signx==-1 && signy==1)
    {
        tmp=TwoPositiveAdd(x->number+1,y->number);
        copystring(x,tmp,-1);        
    }
    //x positive and y negative
    else if (signx==1 && signy==-1)
    {
        tmp=TwoPositiveAdd(x->number,y->number+1);
        copystring(x,tmp,1);
    }
    //free tmp
    free(tmp);

    return 0;    
}

//multiply string x and string y, return a char pointer
char *TwoPositiveMul(char* x,char* y)
{
    int lx=strlen(x);  //length of x 
    int ly=strlen(y);  //length of y
    //create an sum of x*y
    char *sum;
    int lsum = lx+ly; //length of sum
    sum=malloc(sizeof(char)*(lsum+1));

    //initialize '0' to all space of sum 
    sum[lsum]='\0';
    for(int i=0;i<lsum;i++)
        sum[i]='0';
    
    //calculate each digit from back to front
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

    //printf("sum:%s\n",sum);
    //find first not '0' index
    int start=0;
    for(;start<lsum;)
    {
        if (sum[start]!='0')
            break;
        start++;
    }
    //printf("start:%d\n",start);
    //create a out
    char* out;
    out=malloc(sizeof(char)*(lsum-start+1));
    int iout=0; //index of out
    //copy sum to out
    for(;start<lsum;start++)
    {   
        out[iout]=sum[start];
        iout++;
    }
    out[iout]='\0';
    //printf("out:%s\n",out);
    //free sum
    free(sum);

    return out; 
}

// Multiply x *= y
// Return zero on success, non-zero otherwise
int arb_multiply (arb_int_t x, const arb_int_t y)
{
    //error if inputs are empty
    if( x == NULL || y==NULL) return 1; 
    if( x->number == NULL || y->number==NULL) return 1; 

    //if x or y == '0', x->number = '0' 
    if ((x->number[0]=='0' && x->length==1) || (y->number[0]=='0' && y->length==1))
    {
        x->length=1;
        x->capacity=2;
        x->number=realloc(x->number,sizeof(char)*x->capacity);
        x->number[0]='0';
        x->number[1]='\0';
    }
    else
    {
        /*All the functions i would use
        int determinesign(arb_int_t x);
        char *TwoPositiveMul(char* x,char* y);
        void copystring(arb_int_t x, char* tmp, int sign);
        */

        //determine sign of x and sign of y
        int signx,signy;
        signx=determinesign(x);
        signy=determinesign(y);

        //tmporary store number
        char * tmp;

        //For every combination of x and y, get the result of abs(x)*abs(y) and copy to arb_int_t x with the sign.
        //x positive, y positive
        if (signx==1 && signy==1)
        {
            tmp = TwoPositiveMul(x->number,y->number);
            copystring(x,tmp,1);
        }
        //x negative, y positive
        else if (signx==-1 && signy==1)
        {
            tmp = TwoPositiveMul(x->number+1,y->number);
            copystring(x,tmp,-1);
        }
        //x positive, y negative
        else if (signx==1 && signy==-1)
        {
            tmp = TwoPositiveMul(x->number,y->number+1);
            copystring(x,tmp,-1);
        }
        else        //signx==-1 && signy==-1
        {
            tmp = TwoPositiveMul(x->number+1,y->number+1);
            copystring(x,tmp,1);
        }
        //free tmp
        free(tmp);
    }

    return 0;
}


// Return -1 if x<y, 0 if x==y and 1 if x>y.
int arb_compare (const arb_int_t x, const arb_int_t y)
{
    //declare fucntion
    int determinesign(arb_int_t x);
    int xgreatery(char* x,char* y);

    //return 1 or -1 for sign
    int signx = determinesign(x);
    int signy = determinesign(y);

    //comapre sign first
    if(signx==-1 && signy==1)
        return -1;
    else if(signx==1 && signy==-1)
        return 1;
    //if both sign are positive, call function to compare
    else if(signx==1 && signy==1)
        return xgreatery(x->number,y->number);
    //if both sign are negative, reverse the output of the function
    else
    {
        int greater = xgreatery(x->number+1,y->number+1);
        if (greater == 1)
            return -1;
        else if (greater == -1)
            return 1;
        else
            return 0;
    }

}



// Returns the minimum number of characters required to represent
// the number when representing it as a string.
// This includes space for the ¡®-¡¯ sign *but only if the number
// is strictly negative (i.e. <0)*.
// Example: for the arb_int 0, the function returns 1.
// for: -23, the function returns 3.
int arb_digit_count (const arb_int_t x)
{
    return x->length;
}
    
