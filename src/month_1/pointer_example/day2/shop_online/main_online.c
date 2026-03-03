#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_ORDERS   8
#define MAX_NAME     50
#define MAX_PRODUCT  20
#define MAX_QUANTITY 99
#define MAX_PRICE    1000

struct Order_details {
    char  customer_name[MAX_NAME + 1];
    int   order_id;
    char  product_name[MAX_PRODUCT + 1];
    int   quantity;
    float price;
    int   type;
};

int isValidString(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isalpha(str[i]) && !isdigit(str[i]) && str[i] != ' ') return 0;
    }
    return 1;
}

int validateCustomerName(char *name) {
    if (strlen(name) > MAX_NAME) { printf("Too long! Max %d chars.\n", MAX_NAME); return 0; }
    if (!isValidString(name))    { printf("Invalid characters!\n"); return 0; }
    return 1;
}

int validateProductName(char *name) {
    if (strlen(name) > MAX_PRODUCT) { printf("Too long! Max %d chars.\n", MAX_PRODUCT); return 0; }
    if (!isValidString(name))       { printf("Invalid characters!\n"); return 0; }
    return 1;
}

int validateQuantity(int qty) {
    if (qty < 1 || qty > MAX_QUANTITY) { printf("Quantity must be 1-%d.\n", MAX_QUANTITY); return 0; }
    return 1;
}

int validatePrice(float price) {
    if (price <= 0 || price > MAX_PRICE) { printf("Price must be 0.01-%d.\n", MAX_PRICE); return 0; }
    return 1;
}

int order_input(struct Order_details *order, int type) {
    char buffer[200];
    int status = 0;

    order->customer_name[0] = '\0';
    order->product_name[0]  = '\0';
    order->order_id = 0;
    order->quantity = 0;
    order->price    = 0.0f;
    order->type     = type;

    if (type == 1) {
        printf("Enter order ID       : "); scanf("%d", &order->order_id);
        while (1) { printf("Enter customer name  : "); scanf("%49s", buffer);
            if (validateCustomerName(buffer)) { strcpy(order->customer_name, buffer); break; } }
        while (1) { printf("Enter product name   : "); scanf("%19s", buffer);
            if (validateProductName(buffer))  { strcpy(order->product_name,  buffer); break; } }
        while (1) { printf("Enter quantity       : "); scanf("%d", &order->quantity);
            if (validateQuantity(order->quantity)) break; }
        while (1) { printf("Enter price          : "); scanf("%f", &order->price);
            if (validatePrice(order->price)) break; }
        status = 1;

    } else if (type == 2) {
        while (1) { printf("Enter product name   : "); scanf("%19s", buffer);
            if (validateProductName(buffer)) { strcpy(order->product_name, buffer); break; } }
        printf("Enter order ID       : "); scanf("%d", &order->order_id);
        status = 2;

    } else if (type == 3) {
        while (1) { printf("Enter customer name  : "); scanf("%49s", buffer);
            if (validateCustomerName(buffer)) { strcpy(order->customer_name, buffer); break; } }
        while (1) { printf("Enter product name   : "); scanf("%19s", buffer);
            if (validateProductName(buffer))  { strcpy(order->product_name,  buffer); break; } }
        status = 3;

    } else {
        printf("Invalid type!\n");
    }

    return status;
}

void print_order(struct Order_details *order, int status) {
    if      (status == 1) printf("Type 1 : Full Order\n");
    else if (status == 2) printf("Type 2 : Product + Order ID\n");
    else if (status == 3) printf("Type 3 : Customer + Product\n");
    printf("-------------------------\n");
    if (strlen(order->customer_name) > 0)
         printf("Customer Name : %s\n",  order->customer_name);
    else printf("Customer Name : (not provided)\n");
    if (order->order_id != 0)
         printf("Order ID      : %d\n",  order->order_id);
    else printf("Order ID      : (not provided)\n");
    printf("Product Name  : %s\n",   order->product_name);
    if (order->quantity != 0) printf("Quantity      : %d\n",   order->quantity);
    if (order->price    != 0) printf("Price         : %.2f\n", order->price);
    printf("-------------------------\n");
}

void log_csv(struct Order_details *order, int size) {
    FILE *file = fopen("orders.csv", "w");
    if (file == NULL) { printf("Error opening file!\n"); return; }
    fprintf(file, "Type,Customer Name,Order ID,Product Name,Quantity,Price\n");
    for (int i = 0; i < size; i++) {
        fprintf(file, "%d,%s,%d,%s,%d,%.2f\n",
            order[i].type,
            order[i].customer_name,
            order[i].order_id,
            order[i].product_name,
            order[i].quantity,
            order[i].price);
    }
    fclose(file);
    printf("\nOrders saved to orders.csv\n");
}

int check_csvfile() {
    FILE *file = fopen("orders.csv", "r");
    if (file == NULL) return 0;
    fclose(file);
    return 1;
}

int getOrderType() {
    FILE *file = fopen("orders.csv", "r");
    if (file == NULL) return -1;
    char line[200];
    fgets(line, sizeof(line), file);
    int type;
    fscanf(file, "%d,", &type);
    fclose(file);
    return type;
}

int verifyOrder(int order_id, const char *customer_name, const char *product_name, int type) {
    FILE *file = fopen("orders.csv", "r");
    if (file == NULL) { printf("No orders file found!\n"); return 0; }

    char line[300];
    fgets(line, sizeof(line), file);

    int   t, o_id, qty;
    float price;
    char  c_name[MAX_NAME + 1], p_name[MAX_PRODUCT + 1];

    while (fscanf(file, "%d,%50[^,],%d,%20[^,],%d,%f\n",
                  &t, c_name, &o_id, p_name, &qty, &price) == 6) {
        if (type == 1) {
            if (o_id == order_id && strcmp(c_name, customer_name) == 0)
                { fclose(file); return 1; }
        } else if (type == 2) {
            if (o_id == order_id && strcmp(p_name, product_name) == 0)
                { fclose(file); return 1; }
        } else if (type == 3) {
            if (strcmp(c_name, customer_name) == 0 && strcmp(p_name, product_name) == 0)
                { fclose(file); return 1; }
        }
    }

    fclose(file);
    return 0;
}

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
        while (1) {
            printf("How many orders? (max %d) : ", MAX_ORDERS);
            scanf("%d", &n);
            if (n >= 1 && n <= MAX_ORDERS) break;
            printf("Enter between 1 and %d.\n", MAX_ORDERS);
        }

        struct Order_details orders[n];
        int statuses[n];

        for (int i = 0; i < n; i++) {
            int type;
            printf("\n--- Order %d ---\n", i + 1);
            printf("  1 = Full (ID + Name + Product + Qty + Price)\n");
            printf("  2 = Product + Order ID only\n");
            printf("  3 = Customer Name + Product only\n");
            printf("Choice : ");
            scanf("%d", &type);
            statuses[i] = order_input(&orders[i], type);
        }

        printf("\n===== ALL ORDERS =====\n");
        for (int i = 0; i < n; i++) {
            printf("Order %d\n", i + 1);
            print_order(&orders[i], statuses[i]);
        }

        log_csv(orders, n);

    } else {

        printf("============================\n");
        printf("     VERIFY YOUR ORDER      \n");
        printf("============================\n");

        int savedType = getOrderType();

        int  order_id = 0;
        char customer_name[MAX_NAME + 1]   = "";
        char product_name[MAX_PRODUCT + 1] = "";
        char buffer[200];

        printf("\nSystem detected order type %d.\n", savedType);

        if (savedType == 1) {
            /* type 1 saved: order_id + customer_name -> ask both */
            printf("Enter your Order ID      : "); scanf("%d", &order_id);
            while (1) { printf("Enter your Customer Name : "); scanf("%49s", buffer);
                if (validateCustomerName(buffer)) { strcpy(customer_name, buffer); break; } }

        } else if (savedType == 2) {
            /* type 2 saved: order_id + product_name -> ask both, NO customer name */
            printf("Enter your Order ID      : "); scanf("%d", &order_id);
            while (1) { printf("Enter your Product Name  : "); scanf("%19s", buffer);
                if (validateProductName(buffer)) { strcpy(product_name, buffer); break; } }

        } else if (savedType == 3) {
            /* type 3 saved: customer_name + product_name -> ask both, NO order id */
            while (1) { printf("Enter your Customer Name : "); scanf("%49s", buffer);
                if (validateCustomerName(buffer)) { strcpy(customer_name, buffer); break; } }
            while (1) { printf("Enter your Product Name  : "); scanf("%19s", buffer);
                if (validateProductName(buffer)) { strcpy(product_name, buffer); break; } }

        } else {
            printf("Could not read order type from file.\n");
            return 1;
        }

        if (verifyOrder(order_id, customer_name, product_name, savedType)) {
            printf("\nOrder verified! Welcome back");
            if (strlen(customer_name) > 0) printf(" %s", customer_name);
            printf("!\n");
        } else {
            printf("\nOrder not found! Please check your details.\n");
        }
    }

    return 0;
}