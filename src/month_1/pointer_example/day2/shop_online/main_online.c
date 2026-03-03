#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>   // for isalpha(), isspace()

/*  New projects : Dificulty level : Meduim 
    2 Mar 2026 : 1. Order detail and login system 
                 2. Basic GUI 
            Consider part 3 and 4 as challenge goal but not mandatory for this project : Hard 
                 3. api to launch in browser
                 4. Raspberry Pi to host server and connect to database 
*/

#define MAX_ORDERS    8     // max 8 orders
#define MAX_NAME      50    // max 50 chars for customer name
#define MAX_PRODUCT   20    // max 20 chars for product name
#define MAX_QUANTITY  99    // max quantity
#define MAX_PRICE     1000  // max price

struct Order_details {
    char  customer_name[MAX_NAME + 1];
    int   order_id;
    char  product_name[MAX_PRODUCT + 1];
    int   quantity;
    float price;
};

// ─────────────────────────────────────────
// VALIDATION FUNCTIONS
// ─────────────────────────────────────────

// Check string has no special characters — letters, numbers, spaces only
int isValidString(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isalpha(str[i]) && !isdigit(str[i]) && str[i] != ' ') {
            return 0;   // invalid character found ❌
        }
    }
    return 1;   // all good ✅
}

// Validate customer name: max 50 chars, no special chars
int validateCustomerName(char *name) {
    if (strlen(name) > MAX_NAME) {
        printf("❌ Customer name too long! Max %d characters.\n", MAX_NAME);
        return 0;
    }
    if (!isValidString(name)) {
        printf("❌ Customer name has invalid characters! Letters and numbers only.\n");
        return 0;
    }
    return 1;
}

// Validate product name: max 20 chars, no special chars
int validateProductName(char *name) {
    if (strlen(name) > MAX_PRODUCT) {
        printf("❌ Product name too long! Max %d characters.\n", MAX_PRODUCT);
        return 0;
    }
    if (!isValidString(name)) {
        printf("❌ Product name has invalid characters! Letters and numbers only.\n");
        return 0;
    }
    return 1;
}

// Validate quantity: 1 to 99
int validateQuantity(int qty) {
    if (qty < 1 || qty > MAX_QUANTITY) {
        printf("❌ Quantity must be between 1 and %d.\n", MAX_QUANTITY);
        return 0;
    }
    return 1;
}

// Validate price: 0.01 to 1000
int validatePrice(float price) {
    if (price <= 0 || price > MAX_PRICE) {
        printf("❌ Price must be between 0.01 and %.2f.\n", (float)MAX_PRICE);
        return 0;
    }
    return 1;
}

// ─────────────────────────────────────────
// INPUT with validation (retry until valid)
// ─────────────────────────────────────────
int order_input(struct Order_details *order, int type) {

    int status = 0;
    char buffer[200];

    if (type == 1) {

        // Order ID
        printf("Enter order ID       : ");
        scanf("%d", &order->order_id);

        // Customer name — retry loop
        while (1) {
            printf("Enter customer name  : ");
            scanf("%49s", buffer);
            if (validateCustomerName(buffer)) {
                strcpy(order->customer_name, buffer);
                break;
            }
        }

        // Product name — retry loop
        while (1) {
            printf("Enter product name   : ");
            scanf("%19s", buffer);
            if (validateProductName(buffer)) {
                strcpy(order->product_name, buffer);
                break;
            }
        }

        // Quantity — retry loop
        while (1) {
            printf("Enter quantity       : ");
            scanf("%d", &order->quantity);
            if (validateQuantity(order->quantity)) break;
        }

        // Price — retry loop
        while (1) {
            printf("Enter price          : ");
            scanf("%f", &order->price);
            if (validatePrice(order->price)) break;
        }

        status = 1;

    } else if (type == 2) {

        // Product name — retry loop
        while (1) {
            printf("Enter product name   : ");
            scanf("%19s", buffer);
            if (validateProductName(buffer)) {
                strcpy(order->product_name, buffer);
                break;
            }
        }

        printf("Enter order ID       : ");
        scanf("%d", &order->order_id);
        order->customer_name[0] = '\0';
        order->quantity = 0;
        order->price    = 0.0f;
        status = 2;

    } else if (type == 3) {

        // Customer name — retry loop
        while (1) {
            printf("Enter customer name  : ");
            scanf("%49s", buffer);
            if (validateCustomerName(buffer)) {
                strcpy(order->customer_name, buffer);
                break;
            }
        }

        // Product name — retry loop
        while (1) {
            printf("Enter product name   : ");
            scanf("%19s", buffer);
            if (validateProductName(buffer)) {
                strcpy(order->product_name, buffer);
                break;
            }
        }

        order->order_id = 0;
        order->quantity = 0;
        order->price    = 0.0f;
        status = 3;

    } else {
        printf("❌ Invalid type! Please enter 1, 2, or 3.\n");
    }

    return status;
}

// ─────────────────────────────────────────
// PRINT order
// ─────────────────────────────────────────
void print_order(struct Order_details *order, int status) {
    if      (status == 1) printf("Type 1 : Full Order\n");
    else if (status == 2) printf("Type 2 : Product + Order ID\n");
    else if (status == 3) printf("Type 3 : Customer + Product\n");

    printf("-------------------------\n");
    printf("Customer Name : %s\n",   order->customer_name);
    printf("Order ID      : %d\n",   order->order_id);
    printf("Product Name  : %s\n",   order->product_name);
    printf("Quantity      : %d\n",   order->quantity);
    printf("Price         : %.2f\n", order->price);
    printf("-------------------------\n");
}

// ─────────────────────────────────────────
// SAVE to CSV
// ─────────────────────────────────────────
void log_csv(struct Order_details *order, int size) {
    FILE *file = fopen("orders.csv", "w");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }
    fprintf(file, "Customer Name,Order ID,Product Name,Quantity,Price\n");
    for (int i = 0; i < size; i++) {
        fprintf(file, "%s,%d,%s,%d,%.2f\n",
            order[i].customer_name,
            order[i].order_id,
            order[i].product_name,
            order[i].quantity,
            order[i].price);
    }
    fclose(file);
    printf("Orders saved to orders.csv ✅\n");
}

// ─────────────────────────────────────────
// CHECK csv exists
// ─────────────────────────────────────────
int check_csvfile() {
    FILE *file = fopen("orders.csv", "r");
    if (file == NULL) return 0;
    fclose(file);
    return 1;
}

// ─────────────────────────────────────────
// VERIFY order
// ─────────────────────────────────────────
int verifyOrder(int order_id, const char *customer_name) {
    FILE *file = fopen("orders.csv", "r");
    if (file == NULL) {
        printf("No orders file found!\n");
        return 0;
    }

    char line[200];
    fgets(line, sizeof(line), file);   // skip header

    char c_name[50], p_name[50];
    int  o_id, qty;
    float price;

    while (fscanf(file, "%49[^,],%d,%49[^,],%d,%f\n",
                  c_name, &o_id, p_name, &qty, &price) == 5) {
        if (o_id == order_id && strcmp(c_name, customer_name) == 0) {
            fclose(file);
            return 1;   // match ✅
        }
    }

    fclose(file);
    return 0;   // no match ❌
}

// ─────────────────────────────────────────
// MAIN
// ─────────────────────────────────────────
int main() {

    if (check_csvfile() == 0) {

        printf("============================\n");
        printf("Welcome to our online shop!\n");
        printf("============================\n");
        fflush(stdout);

        sleep(1);
        printf("============================\n");
        printf("      FIRST TIME SETUP      \n");
        printf("============================\n");
        fflush(stdout);

        int n;

        // Validate order count: max 8
        while (1) {
            printf("How many orders? (max %d) : ", MAX_ORDERS);
            scanf("%d", &n);
            if (n >= 1 && n <= MAX_ORDERS) break;
            printf("❌ Please enter between 1 and %d orders.\n", MAX_ORDERS);
        }

        struct Order_details orders[n];
        int statuses[n];

        for (int i = 0; i < n; i++) {
            int type;
            printf("\nOrder %d — Enter type (1=Full / 2=Product+ID / 3=Customer+Product) : ", i + 1);
            scanf("%d", &type);
            statuses[i] = order_input(&orders[i], type);
        }

        printf("\n===== ALL ORDERS =====\n");
        for (int i = 0; i < n; i++) {
            print_order(&orders[i], statuses[i]);
        }

        log_csv(orders, n);

    } else {

        printf("============================\n");
        printf("     VERIFY YOUR ORDER      \n");
        printf("============================\n");

        int order_id;
        char customer_name[MAX_NAME + 1];
        char buffer[200];

        printf("Enter your Order ID      : ");
        scanf("%d", &order_id);

        // Validate customer name on verify too
        while (1) {
            printf("Enter your Customer Name : ");
            scanf("%49s", buffer);
            if (validateCustomerName(buffer)) {
                strcpy(customer_name, buffer);
                break;
            }
        }

        if (verifyOrder(order_id, customer_name)) {
            printf("\n✅ Order verified! Welcome back %s\n", customer_name);
        } else {
            printf("\n❌ Order not found! Please check your details.\n");
        }
    }

    return 0;
}