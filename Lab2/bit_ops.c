#include <stdio.h>

/* Return the nth bit of x.
   Assume 0 <= n <= 31 */
unsigned
get_bit (unsigned x, unsigned n)
{
    /* YOUR CODE HERE
       Returning -1 is a placeholder (it makes
       no sense, because get_bit only returns 
       0 or 1) */
    return (x >> n) & (unsigned)1;
    // return -1;
}

/* Set the nth bit of the value of x to v.
   Assume 0 <= n <= 31, and v is 0 or 1 */
void
set_bit (unsigned *x, unsigned n, unsigned v)
{
    /* YOUR CODE HERE */
    /* I. The basic clue:
     * 1. Get an operand which has 0 at n-th bit and 1 at other bits
     * 11..111..11 XOR 00..010..00 = 11..101..11
     * 2. Set the n-th bit of x to 0
     * 11..101..11 AND xx..xxx..xx = xx..x0x..xx
     * 3. Set the n-th bit of x to v(actually, we now only consider how to set 0 to v)
     * xx..x0x..xx XOR 00..0v0..00 = xx..xvx..xx
     */
    (*x) = ((*x) & (((unsigned)-1) ^ ((unsigned)1 << n))) ^ (v << n);
    /* II. The basic clue:
     * 1. If v = 1, then -v = 111...111
     * 2. 111...111 XOR xx..xxx..xx = xx..x~xx..xx
     * 3. Reserve the n-th bit of ~x: xx..x~xx..xx AND 00..010..00 = 00..0~x0..00
     * 4. 00..0(~x)0..00 XOR xx..xxx..xx = xx..x1x..xx
     * 5. If v = 0, then -v = 000...000
     * 6. 000...000 XOR xxx...xxx = xxx...xxx
     * 7. Reserve the n-th bit of x: xx..xxx..xx AND 00..010..00 = 00..0x0..00
     * 8. 00..0x0..00 XOR xx..xxx..xx = xx..x0x..xx
     */    
    (*x) ^= ((unsigned)-v ^ *x) & ((unsigned)1 << n);
}

/* Flip the nth bit of the value of x.
   Assume 0 <= n <= 31 */
void
flip_bit (unsigned *x, unsigned n)
{
    /* YOUR CODE HERE */
    (*x) = (*x) ^ ((unsigned)1 << n);
}


/*
 * YOU CAN IGNORE THE REST OF THIS FILE
 */

void test_get_bit(unsigned x,
        unsigned n,
        unsigned expected) {
    unsigned a = get_bit(x, n);
    if(a!=expected) {
        printf("get_bit(0x%08x,%u): 0x%08x, expected 0x%08x\n",x,n,a,expected);
    } else {
        printf("get_bit(0x%08x,%u): 0x%08x, correct\n",x,n,a);
    }
}
void test_set_bit(unsigned x,
        unsigned n,
        unsigned v,
        unsigned expected) {
    unsigned o = x;
    set_bit(&x, n, v);
    if(x!=expected) {
        printf("set_bit(0x%08x,%u,%u): 0x%08x, expected 0x%08x\n",o,n,v,x,expected);
    } else {
        printf("set_bit(0x%08x,%u,%u): 0x%08x, correct\n",o,n,v,x);
    }
}
void test_flip_bit(unsigned x,
        unsigned n,
        unsigned expected) {
    unsigned o = x;
    flip_bit(&x, n);
    if(x!=expected) {
        printf("flip_bit(0x%08x,%u): 0x%08x, expected 0x%08x\n",o,n,x,expected);
    } else {
        printf("flip_bit(0x%08x,%u): 0x%08x, correct\n",o,n,x);
    }
}
int main()
{
    printf("\nTesting get_bit()\n\n");
    test_get_bit(0b1001110,0,0);
    test_get_bit(0b1001110,1,1);
    test_get_bit(0b1001110,5,0);
    test_get_bit(0b11011,3,1);
    test_get_bit(0b11011,2,0);
    test_get_bit(0b11011,9,0);
    printf("\nTesting set_bit()\n\n");
    test_set_bit(0b1001110,2,0,0b1001010);
    test_set_bit(0b1101101,0,0,0b1101100);
    test_set_bit(0b1001110,2,1,0b1001110);
    test_set_bit(0b1101101,0,1,0b1101101);
    test_set_bit(0b1001110,9,0,0b1001110);
    test_set_bit(0b1101101,4,0,0b1101101);
    test_set_bit(0b1001110,9,1,0b1001001110);
    test_set_bit(0b1101101,7,1,0b11101101);
    printf("\nTesting flip_bit()\n\n");
    test_flip_bit(0b1001110,0,0b1001111);
    test_flip_bit(0b1001110,1,0b1001100);
    test_flip_bit(0b1001110,2,0b1001010);
    test_flip_bit(0b1001110,5,0b1101110);
    test_flip_bit(0b1001110,9,0b1001001110);
    printf("\n");
    return 0;
}