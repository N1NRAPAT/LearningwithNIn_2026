#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*  
    duration time for this mini project : 4 - 11 March 2026 ; intermediate level
    Key features:
    1. Guest Login and Registered User Login
    2. Read and Write CSV file
    3. Order selection and validation
    4. Checkout system
    5. Order history and display
    6. Returning user login and order retrieval
*/

#define MAX_PRODUCTS 100
#define MAX_CART_ITEMS 50
#define MAX_NAME 50
#define MAX_EMAIL 50
#define MAX_PASSWORD 20
#define MAX_PRODUCTS_NAME 20

/*
    Define struct for order details, product, and cart
     - Order_details : to store customer name, email, password, order id,
       product name, quantity, price and type of user
     - Product : to store product name, price and order id
     - Cart : to store selected products, count of items and total price
*/
typedef struct
{
    int   type;
    char  customer_name[MAX_NAME];
    char  customer_email[MAX_EMAIL];
    char  customer_password[MAX_PASSWORD];
    int   order_id;
    char  product_name[MAX_PRODUCTS_NAME];
    int   quantity;
    float price;
} Order_details;

typedef struct
{
    char  product_name[MAX_PRODUCTS_NAME];
    float price;
    int   order_id;
} Product;

typedef struct
{
    Product items[MAX_CART_ITEMS];
    int     count;
    float   total;
} Cart;

/*  
    list of function prototypes
    - slection_mode() : to select guest login or registered user login
    - read_csv_file() : to read product catalog from csv file
    - validate_string() : to validate input string for customer name and product name
    - display_catalog() : to display product catalog to user
    - add_to_cart() : to add selected product to cart
    - display_cart() : to display current items in cart
    - checkOut() : to process checkout and save order details to csv file
*/

int selection_mode();
int read_csv_file(Product *catalog);
int validate_string(const char *str);
void display_catalog(Product *catalog, int size);
void add_to_cart(Cart *cart, Product *p);
void display_cart(Cart *cart);
void checkOut(Cart *cart , Order_details *customer);


/*
    Main Operation 
*/
int main()
{

    Order_details customer_name;

    int choice = selection_mode();

    if (choice == 1)
    {
        printf("Guest Login\n");
    }
    else if (choice == 2)
    {
        printf("Registered User\n");
    }
    else
    {
        printf("Exiting...\n");
        return 0;
    }

    return 0;
}

/*
    Selection mode to sort out guest login and registered user login
*/ 
int selection_mode()
{
    int choice;
    printf("\n============================\n");
    printf("   WELCOME TO OUR SHOP!     \n");
    printf("============================\n");
    printf("  1. Guest Login\n");
    printf("  2. Registered User\n");
    printf("  0. Exit\n");
    printf("============================\n");
    printf("Choice : ");
    scanf("%d", &choice);
    return choice;
}

/*
    Read CSV file and populate the product catalog
*/

int Read_csv_file(Product *catalog)
{
    FILE *file = fopen("orders.csv", "r");
    if (file == NULL)
    {
        printf("No orders file found!\n");
        return 0 ;
    }

    char line[300];
    fgets(line, sizeof(line), file); // skip header line 
    int count = 0; 
    int type , order_id , qty;
    float price;
    char c_name[MAX_NAME] ,email[MAX_EMAIL], pass[MAX_PASSWORD], p_name[MAX_PRODUCTS_NAME];

    while (fscanf(file, "%d,%49[^,],%49[^,],%19[^,],%d,%19[^,],%d,%f\n",
                  &type, c_name, email, pass,
                  &order_id, p_name, &qty, &price) == 8 && count < MAX_PRODUCTS) {

            char *trimmed_name = p_name;
            while (*trimmed_name == ' ') {
                trimmed_name++;
            }
           
            strcpy(catalog[count].product_name, trimmed_name); // string copy trimmed_name replace p_name 
            catalog[count].price = price ; // assign price to catalog
            catalog[count].order_id = order_id; // assign order_id to catalog
            count++;
            }

    // Add code to read from the CSV file
    fclose(file);
    return count;
}

/*
    Validate string 
*/ 
int Validate_string(const char *str){
    for(int i = 0 ; str[i] != '\0'; i++){
        if(!isalpha(str[i]) && !isdigit(str[i]) && str[i] != ' ')
            return 0;
            // if char != A-Z and char != digit and char != space return 0
    }
    return 1;
}

/*
    Display product catalog to user
*/

void Display_catalog(Product *catalog, int size){
    printf("\n============================\n");
    printf("       PRODUCT CATALOG      \n");
    printf("============================\n");
    printf("%-4s %-15s %-10s %s\n", "No.", "Product", "Price", "Order ID");
    printf("--------------------------------------------\n");
    for (int i = 0; i < size; i++) {
        printf("%-4d %-15s $%-9.2f %d\n",
            i + 1,
            catalog[i].product_name,
            catalog[i].price,
            catalog[i].order_id);
    }
    printf("--------------------------------------------\n");
}

void add_to_cart(Cart *cart, Product *p) {
    // count has been added to Cart
    if (cart->count >= MAX_CART_ITEMS){
        printf("Cart is full! Cannot add more items.\n");
        return ;
    }
    cart->items[cart->count] = *p; // item[count] point to p 
    cart->total += p->price;
    cart->count++;
    printf("Added %s to cart. Total: $%.2f\n", p->product_name, p->price);
}

/*
    Display current items in cart by iterating through cart items and 
    print product name and price, also display total price at the end
*/
void display_cart(Cart *cart){
    if(cart->count == 0){
        printf("Your cart is empty.\n");
        return ;
    }
    printf("\n============================\n");
    printf("          YOUR CART         \n");
    printf("============================\n");
    for(int i = 0 ; i < cart->count ; i++){
        printf("%d. %-15s $%.2f\n",
            i + 1,
            cart->items[i].product_name,
            cart->items[i].price);
    }
    printf("----------------------------\n");
    printf("   TOTAL : $%.2f\n", cart->total);
    printf("============================\n");
}

void checkOut(Cart *cart , Order_details *customer){
    if (cart->count == 0) {
        printf("Nothing to checkout!\n");
        return;
    }

    FILE *file = fopen("orders.csv", "a");// open file in append mode
    if (file == NULL) {
        printf("Error saving checkout!\n");
        return;
    }
    for (int i = 0; i <cart->count; i++) {
        fprintf(file, "%s,%s,%s,%d,%s,%.2f\n",
            customer->customer_name,
            customer->customer_email,
            customer->type == 1 ? "Guest" : "Registered",
            cart->items[i].order_id,
            cart->items[i].product_name,
            cart->items[i].price);
            // append customer detail to csv file
    }
    fclose(file);

    printf("\n============================\n");
    printf("     CHECKOUT COMPLETE!     \n");
    printf("============================\n");
    printf("Customer : %s\n", strlen(customer->customer_name) > 0
                               ? customer->customer_name : "Guest");
    printf("Items    : %d\n", cart->count);
    printf("Total    : $%.2f\n", cart->total);
    printf("Saved to : checkout.csv\n");
    printf("============================\n");

}

