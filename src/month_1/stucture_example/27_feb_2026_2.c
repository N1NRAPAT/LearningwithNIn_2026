#include <stdio.h>

/*
    27 Feb 2026

    Nin ; Using array of structure to store multiple person details and find the tallest person
    and sort them by height and log to csv file

    - Structure 
    - Array 
    - Pointer
    - Log to CSV file
*/

struct Person {
    char name[50];
    int age;
    float height;
};

void inputPerson(struct Person *p , int number){
    printf("Input person (%d) details:\n" , number );
    printf("Enter name   : ");  scanf("%s", p->name); 
    printf("Enter age    : ");  scanf("%d", &p->age);
    printf("Enter height : ");  scanf("%f", &p->height);
}

void printPerson(struct Person *arr , int size){
    printf("\n All Person details \n");
    for(int i = 0 ; i < size ; i++){
        printf("Name   : %s\n", arr[i].name);
        printf("Age    : %d\n", arr[i].age);
        printf("Height : %.1f\n", arr[i].height);
        printf("-------------------------\n");
    }
}

struct Person *getHighest(struct Person *arr , int size){
    struct Person *highest = &arr[0] ; 
    for (int i = 1 ; i < size ; i++){
        if (arr[i].height > highest->height){
            highest = &arr[i] ; 
        }
    }
    return highest ;
}


void logtoCSV(struct Person *arr , int size){
    FILE *file = fopen("csv_file/person_data.csv", "w"); // mode write 
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    fprintf(file, "Name,Age,Height\n"); // CSV header
    for (int i = 0; i < size; i++) {
        fprintf(file, "%s,%d,%.1f\n", arr[i].name, arr[i].age, arr[i].height);
    }

    fclose(file);
    printf("Data logged to person_data.csv successfully.\n");
}


int main(){

    int n ; 
    printf("How many person do you want to input ? : ");
    scanf("%d" , &n) ;

    struct Person person[n];
    for (int i = 0 ; i < n ; i++){
        inputPerson(&person[i] , i+1); // loop input for each person
    }

    printPerson(person , n); // print all person details

    struct Person *tallest = getHighest(person , n); // find tallest person
    printf("\nThe tallest person is : %s with height %.1f\n", tallest->name, tallest->height);

    logtoCSV(person , n); // log data to csv file


    return 0 ;
}