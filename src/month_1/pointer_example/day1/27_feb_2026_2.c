#include <stdio.h>

// Nin ; Using array of 10 number and doubles them 

void doubleUp(int * arr , int size){
    for (int i = 0; i < size; i++) {
        arr[i] = arr[i] * 2;   // modifies original array
    }
}

int main (){
    int number[10] = {1 , 3 , 5 , 7 ,9 , 11 , 13, 15 , 17 ,19} ; 
    int size = 10 ; 

    printf("Before ... \n") ; 
    for (int i = 0 ; i < size ; i++){
        printf("Number(%d) = %d " , i+1 , number[i]);
        if (i+1 == 5 ){
            printf("\n");
        }
    }

    doubleUp(number , size);
    printf("\nAfter ... \n") ; 
    for (int i = 0 ; i < size ; i++){
        printf("Number(%d) = %d " , i+1 , number[i]);
        if (i+1 == 5 ){
            printf("\n");
        }
    }

    int *ptr = number ; 
    printf("\nUsing pointer arithmetic:\n");
    for (int i = 0; i < size; i++) {
        printf("ptr+(%d) = %d ", i, *(ptr + i));
        if (i+1 == 5 ){
            printf("\n");
        }  
    }



    return 0 ; 
}