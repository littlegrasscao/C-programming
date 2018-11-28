// NOTE the header protection here! (protect against double inclusion!)
#pragma once
#include <stddef.h>

struct arb_int{
    char *number;               //int pointers used to store integer
    unsigned int length;        //length of integers
    unsigned int capacity;      //capacity of memory space of pointers
};

typedef struct arb_int arb_int;

typedef struct arb_int * arb_int_t;

// Free arb_int_t
void arb_free (arb_int_t * i);

// Duplicate arb_int_t. The new arb_int_t, when no longer needed,
// needs to be freed using arb_free. The original arb_int_t is unmodified.
// Returns zero on success, non-zero otherwise
int arb_duplicate (arb_int_t * new, const arb_int_t original);


// Convert from string representation into arb_int_t.
// Return zero and set *i to the new arb_int_t if successful,
// do not change *i and return non-zero otherwise.
// The following forms should be accepted:
// +2323 -232323 +0 -0 23232
// If any other characters than 0-9,+,- are present you should
// return an error (i.e. nonzero).
int arb_from_string (arb_int_t * i, const char * s);


// Convert from long long int to arb_int_t
// Return zero on success, non-zero otherwise.
int arb_from_int (arb_int_t * i, signed long long int source);


// Convert arb_int_t to string
// Return zero on success, or non-zero if buf is not big enough.
// Make sure buf is always properly zero-terminated.
// max is the maximum number of bytes (including the terminating
// zero) than can be written to buf.
// Negative numbers should have a leading ¡¯-¡¯; Positive numbers
// *should not* have a +. 0 is considered to be positive.
int arb_to_string (const arb_int_t i, char * buf, size_t max);


// Convert arb_int_t to long long int
// Return zero on success, non-zero otherwise.
// (for example if i > LLONG_MAX or < LLONG_MIN)
int arb_to_int (const arb_int_t i, long long int * out);


// Assign the numeric value of arb_int_t y to arb_int_t x. x and y have
// to be separate arb_int_t¡¯s construced by one of the functions above.
// Note that
// x and y remain separate (i.e. changing x does not change y).
// This function differs from arb_duplicate in that both x and y
// have to be created already *before* calling this function.
// Returns 0 if success, return non-zero otherwise. In the latter case
// x and y remain unmodified.
int arb_assign (arb_int_t x, const arb_int_t y);

// Add one arb_int_t to another one, modifying one of them.
// (x += y)
// Return zero on success, non-zero otherwise
int arb_add (arb_int_t x, const arb_int_t y);
// Subtract an arb_int_t from another.
// (x -= y);
// Return zero on success, non-zero otherwise
int arb_subtract (arb_int_t x, const arb_int_t y);
// Multiply
// x *= y
// Return zero on success, non-zero otherwise
int arb_multiply (arb_int_t x, const arb_int_t y);


// Divide (for extra credit)
// x /= y
// Note: this is integer division, so the result gets truncated to an
// integer. For example, 192/100 => 1
// Return 0 on success, non-zero otherwise
//int arb_divide (arb_int_t x, const arb_int_t y);


// Return -1 if x<y, 0 if x==y and 1 if x>y.
int arb_compare (const arb_int_t x, const arb_int_t y);

// Returns the minimum number of characters required to represent
// the number when representing it as a string.
// This includes space for the ¡®-¡¯ sign *but only if the number
// is strictly negative (i.e. <0)*.
// Example: for the arb_int 0, the function returns 1.
// for: -23, the function returns 3.
int arb_digit_count (const arb_int_t x);

