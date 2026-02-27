#include <stdio.h>
#include <string.h>

/*
    Learn C every day project by NIN ; Aim to improve coding skill throghout AI generation world !

    This is program to simulate a circumstance of book store service. 
    Key feature is search each of elements in the library to serve the level 
    of anticipation as user would love to see. 

    - pointer
    - array 
    - structure 
    - string 
    
*/

struct books {
    char title[50];
    char author[50];
    int pages;
};


void setbook(struct books *b, const char *title, int pages, const char *author) {
    strcpy(b->title, title);
    strcpy(b->author, author);
    b->pages = pages;
}

void printbook(struct books *b) {
    printf("Title : %s\n", b->title);
    printf("Pages : %d\n", b->pages);
    printf("Author: %s\n", b->author);
    printf("--------------------------\n");
}

void printAll(struct books *library, int size) {
    for (int i = 0; i < size; i++) {
        printf("Book %d\n", i + 1);
        printbook(&library[i]);
    }
}

struct books* findBook(struct books *library, int size, const char *title) {
    for (int i = 0; i < size; i++) {
        if (strcmp(library[i].title, title) == 0) {
            return &library[i];
        }
    }
    return NULL;
}

int main() {

    struct books library[3];

    setbook(&library[0], "C Programming", 300, "Dennis Ritchie");
    setbook(&library[1], "Clean Code",    431, "Robert Martin");
    setbook(&library[2], "The Pragmatic", 352, "Andrew Hunt");

    printf("===== ALL BOOKS =====\n");
    printAll(library, 3);

    printf("===== SEARCH =====\n");
    struct books *found = findBook(library, 3, "dang Code");
    if (found != NULL) {
        printf("Found it!\n");
        printbook(found);
    } else {
        printf("Book not found.\n");
    }

    return 0;
}

