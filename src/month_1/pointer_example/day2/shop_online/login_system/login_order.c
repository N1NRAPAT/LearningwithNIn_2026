#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>

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
    - input_password() : to input password with hidden text using termios
    - save_cart() : to save current cart to cart.csv keyed by customer email
    - load_cart() : to load saved cart from cart.csv on login
    - clear_cart_csv() : to remove cart from cart.csv after checkout
*/

int  selection_mode();
int  Read_csv_file(Product *catalog);
int  Validate_string(const char *str);
void Display_catalog(Product *catalog, int size);
void add_to_cart(Cart *cart, Product *p, Order_details *customer);
void display_cart(Cart *cart);
void checkOut(Cart *cart, Order_details *customer);
void input_password(char *password, int max_len);
void save_cart(Cart *cart, Order_details *customer);
void load_cart(Cart *cart, Order_details *customer);
void clear_cart_csv(Order_details *customer);
void guest_login(Order_details *customer);
int  registered_login(Order_details *customer);
void shopping_menu(Order_details *customer, Product *catalog, int product_count);


/*
    Main Operation 
*/
int main()
{
    Product catalog[MAX_PRODUCTS];
    int product_count = Read_csv_file(catalog);

    if (product_count == 0)
    {
        printf("No products found. Please check orders.csv\n");
        return 1;
    }
    printf("Loaded %d products from catalog.\n", product_count);

    Order_details customer;
    memset(&customer, 0, sizeof(Order_details));

    int choice    = selection_mode();
    int logged_in = 0;

    if (choice == 1)
    {
        guest_login(&customer);
        logged_in = 1;
    }
    else if (choice == 2)
    {
        logged_in = registered_login(&customer);
        if (!logged_in)
        {
            printf("Login failed. Exiting...\n");
            return 1;
        }
    }
    else
    {
        printf("Exiting...\n");
        return 0;
    }

    // shoping menu loop after successful login
    if (logged_in)
    {
        shopping_menu(&customer, catalog, product_count);
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

/*
    input_password -- hides typed characters using termios
    turns off ECHO so password is not shown on screen
    restores terminal settings after input is done
*/
void input_password(char *password, int max_len)
{
    struct termios oldt, newt;

    tcgetattr(STDIN_FILENO, &oldt);          // save current terminal settings
    newt = oldt;
    newt.c_lflag &= ~(ECHO);                 // turn off ECHO flag
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // apply new settings

    int i = 0;
    char c;
    while (i < max_len - 1 && (c = getchar()) != '\n' && c != EOF)
    {
        password[i++] = c;
        printf("*");   // print * for each character typed
        fflush(stdout);
    }
    password[i] = '\0';

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // restore original terminal settings
    printf("\n");
}

/*
    save_cart -- saves current cart items to cart.csv keyed by customer email
    overwrites existing entry for same customer so cart stays up to date
    format : email,product_name,order_id,price
*/
void save_cart(Cart *cart, Order_details *customer)
{
    // read all existing cart entries except current customer
    FILE *old = fopen("cart.csv", "r");
    char lines[500][300];
    int  line_count = 0;

    if (old != NULL)
    {
        char line[300];
        fgets(line, sizeof(line), old);   // skip header
        while (fgets(line, sizeof(line), old) && line_count < 500)
        {
            // keep lines that do NOT belong to this customer
            if (strncmp(line, customer->customer_email,
                        strlen(customer->customer_email)) != 0)
            {
                strcpy(lines[line_count++], line);
            }
        }
        fclose(old);
    }

    // rewrite cart.csv with other customers and updated current customer cart
    FILE *file = fopen("cart.csv", "w");
    if (file == NULL) { printf("Error saving cart!\n"); return; }

    fprintf(file, "Email,Product Name,Order ID,Price\n"); // header

    // write back other customers lines
    for (int i = 0; i < line_count; i++)
        fprintf(file, "%s", lines[i]);

    // write current customer cart items
    for (int i = 0; i < cart->count; i++)
    {
        fprintf(file, "%s,%s,%d,%.2f\n",
            customer->customer_email,
            cart->items[i].product_name,
            cart->items[i].order_id,
            cart->items[i].price);
    }

    fclose(file);
}

/*
    load_cart -- loads saved cart from cart.csv on login
    matches rows by customer email and fills cart struct
*/
void load_cart(Cart *cart, Order_details *customer)
{
    FILE *file = fopen("cart.csv", "r");
    if (file == NULL) return;   // no cart file yet is fine

    char line[300];
    fgets(line, sizeof(line), file);   // skip header

    char email[MAX_EMAIL], p_name[MAX_PRODUCTS_NAME];
    int  order_id;
    float price;

    while (fscanf(file, "%49[^,],%19[^,],%d,%f\n",
                  email, p_name, &order_id, &price) == 4)
    {
        // only load items belonging to this customer
        if (strcmp(email, customer->customer_email) == 0 &&
            cart->count < MAX_CART_ITEMS)
        {
            strcpy(cart->items[cart->count].product_name, p_name);
            cart->items[cart->count].order_id = order_id;
            cart->items[cart->count].price    = price;
            cart->total += price;
            cart->count++;
        }
    }

    fclose(file);

    if (cart->count > 0)
        printf("Restored %d item(s) from your previous cart.\n", cart->count);
}

/*
    clear_cart_csv -- removes customer cart entries from cart.csv after checkout
    rewrites the file keeping all other customers entries intact
*/
void clear_cart_csv(Order_details *customer)
{
    FILE *old = fopen("cart.csv", "r");
    if (old == NULL) return;

    char lines[500][300];
    int  line_count = 0;
    char line[300];
    fgets(line, sizeof(line), old);   // skip header

    while (fgets(line, sizeof(line), old) && line_count < 500)
    {
        // keep lines that do NOT belong to this customer
        if (strncmp(line, customer->customer_email,
                    strlen(customer->customer_email)) != 0)
        {
            strcpy(lines[line_count++], line);
        }
    }
    fclose(old);

    FILE *file = fopen("cart.csv", "w");
    if (file == NULL) return;

    fprintf(file, "Email,Product Name,Order ID,Price\n");
    for (int i = 0; i < line_count; i++)
        fprintf(file, "%s", lines[i]);

    fclose(file);
}

void add_to_cart(Cart *cart, Product *p, Order_details *customer) {
    // count has been added to Cart
    if (cart->count >= MAX_CART_ITEMS){
        printf("Cart is full! Cannot add more items.\n");
        return ;
    }
    cart->items[cart->count] = *p; // item[count] point to p 
    cart->total += p->price;
    cart->count++;
    printf("Added %s to cart. Total: $%.2f\n", p->product_name, cart->total);

    save_cart(cart, customer);   // persist cart immediately after every add
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

    FILE *file = fopen("checkout.csv", "a");// open file in append mode
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

    clear_cart_csv(customer);   // remove cart from cart.csv after checkout

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

/* 
    Guest login -- name only, no account needed
    guest cart is not persisted since there is no email key
*/
void guest_login(Order_details *customer)
{
    customer->type = 1;
    customer->customer_email[0]    = '\0';
    customer->customer_password[0] = '\0';

    printf("\n--- Guest Login ---\n");
    printf("Enter your name : ");
    scanf("%49s", customer->customer_name);
    printf("Welcome %s! (Guest)\n", customer->customer_name);
}

/* 
    Registered login -- option to login or create new account
    reads and writes to users.csv for account storage
    password input is hidden using input_password()
    cart is loaded from cart.csv on successful login
*/
int registered_login(Order_details *customer)
{
    customer->type = 2;

    printf("\n--- Registered User ---\n");
    printf("  1. Login\n");
    printf("  2. Create Account\n");
    printf("Choice : ");
    int choice;
    scanf("%d", &choice);
    getchar();   // flush newline before password input

    if (choice == 2)
    {
        // create new account and save to users.csv
        printf("Enter name     : "); scanf("%49s",  customer->customer_name);
        printf("Enter email    : "); scanf("%49s",  customer->customer_email);
        getchar();   // flush newline
        printf("Enter password : "); input_password(customer->customer_password, MAX_PASSWORD);

        FILE *check = fopen("users.csv", "r");
        if (check == NULL)
        {
            // first time -- write header
            FILE *newfile = fopen("users.csv", "w");
            fprintf(newfile, "Name,Email,Password\n");
            fclose(newfile);
        }
        else
        {
            fclose(check);
        }

        FILE *file = fopen("users.csv", "a");
        if (file == NULL) { printf("Error saving account!\n"); return 0; }
        fprintf(file, "%s,%s,%s\n",
            customer->customer_name,
            customer->customer_email,
            customer->customer_password);
        fclose(file);

        printf("Account created! Welcome %s\n", customer->customer_name);
        return 1;
    }

    // login -- match email + password from users.csv
    printf("Enter email    : "); scanf("%49s", customer->customer_email);
    getchar();   // flush newline before password input
    printf("Enter password : "); input_password(customer->customer_password, MAX_PASSWORD);

    FILE *file = fopen("users.csv", "r");
    if (file == NULL)
    {
        printf("No user database found. Please create an account first.\n");
        return 0;
    }

    char line[200];
    fgets(line, sizeof(line), file);   // skip header

    char name[MAX_NAME], email[MAX_EMAIL], pass[MAX_PASSWORD];
    while (fscanf(file, "%49[^,],%49[^,],%19[^\n]\n", name, email, pass) == 3)
    {
        if (strcmp(email, customer->customer_email) == 0 &&
            strcmp(pass,  customer->customer_password) == 0)
        {
            strcpy(customer->customer_name, name);
            fclose(file);
            printf("Login successful! Welcome back %s\n", name);
            return 1;   // match found
        }
    }

    fclose(file);
    printf("Incorrect email or password!\n");
    return 0;
}

/*
    Shopping menu -- main loop after login
    allows user to browse, add to cart, remove, checkout and logout
    registered users have their cart loaded from cart.csv on entry
*/
void shopping_menu(Order_details *customer, Product *catalog, int product_count)
{
    Cart cart;
    cart.count = 0;
    cart.total = 0.0f;

    // load saved cart for registered users only (guest has no email key)
    if (customer->type == 2)
        load_cart(&cart, customer);

    int choice;

    do {
        printf("\n============================\n");
        printf("  Hello, %s! (%s)\n",
            strlen(customer->customer_name) > 0
                ? customer->customer_name : "Guest",
            customer->type == 1 ? "Guest" : "Registered");
        printf("============================\n");
        printf("  1. Browse Products\n");
        printf("  2. View Cart (%d items)\n", cart.count);
        printf("  3. Checkout\n");
        printf("  4. Logout\n");
        printf("============================\n");
        printf("Choice : ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                // browse catalog and pick item to add to cart
                Display_catalog(catalog, product_count);
                printf("Pick number to add (0 = back) : ");
                int pick;
                scanf("%d", &pick);
                if (pick >= 1 && pick <= product_count) {
                    add_to_cart(&cart, &catalog[pick - 1], customer);
                } else if (pick == 0) {
                    // go back to menu
                } else {
                    printf("Invalid selection!\n");
                }
                break;

            case 2:
                // view cart and optionally remove an item
                display_cart(&cart);
                if (cart.count > 0)
                {
                    printf("Remove item? (enter number / 0 = back) : ");
                    int rem;
                    scanf("%d", &rem);
                    if (rem >= 1 && rem <= cart.count)
                    {
                        printf("Removed %s ($%.2f)\n",
                            cart.items[rem - 1].product_name,
                            cart.items[rem - 1].price);
                        cart.total -= cart.items[rem - 1].price;
                        // shift remaining items left to fill the gap
                        for (int i = rem - 1; i < cart.count - 1; i++) {
                            cart.items[i] = cart.items[i + 1];
                        }
                        cart.count--;
                        save_cart(&cart, customer);   // persist after removal
                    }
                }
                break;

            case 3:
                // confirm and process checkout
                display_cart(&cart);
                if (cart.count > 0)
                {
                    printf("Confirm checkout? (1=Yes / 0=No) : ");
                    int confirm;
                    scanf("%d", &confirm);
                    if (confirm == 1)
                    {
                        checkOut(&cart, customer);
                        cart.count = 0;   // clear cart after checkout
                        cart.total = 0.0f;
                    }
                    else
                    {
                        printf("Checkout cancelled.\n");
                    }
                }
                break;

            case 4:
                printf("\nLogging out... Goodbye %s!\n",
                    strlen(customer->customer_name) > 0
                    ? customer->customer_name : "Guest");
                break;

            default:
                printf("Invalid choice! Enter 1-4.\n");
        }

    } while (choice != 4);
}