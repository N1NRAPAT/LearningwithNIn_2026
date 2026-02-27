#include <stdio.h>
#include <string.h>

struct books
{
    char title[20]  ;
    int number ; 
    char author[20] ;  
};
// char of non mean only one output coming out 

int main(){

    struct books book ;  

    strcpy(book.title , "Ninninnin");
    // This is the function to use string 
    book.number = 123 ;
    strcpy(book.author , "Ninrapat");


    printf("This is the name = %s\n" , book.title);
    printf("This is the number of pages = %d\n" , book.number);
    printf("This is the author = %s\n" , book.author);
  
    return 0;
}

