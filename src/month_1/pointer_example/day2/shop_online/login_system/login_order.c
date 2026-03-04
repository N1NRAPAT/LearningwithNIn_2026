#include <stdio.h>
#include <string.h>

/*  Spand time for this mini project : 4 - 11 March 2026 
    Key features:
    1. Guest Login and Registered User Login
    2. Read and Write CSV file
    3. Order selection and validation
    4. Checkout system
    5. Order history and display
    6. Returning user login and order retrieval
*/ 


// struct to store order details
typedef struct {
    int   type;
    char  customer_name[50];
    char  customer_email[50];
    char  customer_password[20];
    int   order_id;
    char  product_name[20];
    int   quantity;
    float price;
} Order_details;


// Selection mode to sort out guest login and registered user login
int selection_mode(){
    int choice;
    printf("============================\n");
    printf("  1. Guest Login\n");
    printf("  2. Registered User\n");
    printf("============================\n");
    printf("Choice : ");
    scanf("%d", &choice);
    return choice;
}

void Read_csv_file(){

}

int main(){

    Order_details customer_name ; 

    int choice = selection_mode();

    if (choice == 1){
        printf("Guest Login\n");
    }
    else {
        printf("Registered User\n");
    }


    return 0;
}