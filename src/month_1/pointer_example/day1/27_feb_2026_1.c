#include <stdio.h>

// NIN ; Learning how number in pointer and non-pointer works while dynamic programming 


int main(){

    int number = 10 ; 
    int original = number ; 
    int * new = &number ; 
    
    printf("number = %d\n" , number);
    printf("address = %p\n" , &number);

    printf("memories = %p\n" , new) ; 
    printf("value by pointer = %d\n" , *new);

    *new = 100 ; 
    printf("number = %d\n" , number);
    printf("address = %p\n" , &number);

    printf("memories = %p\n" , new) ; 
    printf("value by pointer = %d\n" , *new);

    *new = 1000 ; 
    printf("number = %d\n" , number);
    printf("address = %p\n" , &number);

    printf("memories = %p\n" , new) ; 
    printf("value by pointer = %d\n" , *new);

    printf("Number recently = %d\n" , number );
    number = original ; 
    printf("Number orginal = %d\n" , number );
    return 0 ;
}