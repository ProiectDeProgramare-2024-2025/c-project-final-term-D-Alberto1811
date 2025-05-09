#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PRODUCTS 100
#define FILE_NAME "inventory.txt"

// ANSI Color Codes
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_YELLOW  "\033[0;33m"
#define COLOR_CYAN    "\033[0;36m"
#define COLOR_RED     "\033[0;31m"
#define COLOR_BLUE    "\033[0;34m"

typedef struct {
    char name[50];
    int quantity;
    float price;
} Product;

Product inventory[MAX_PRODUCTS];
int product_count = 0;

// Function Prototypes
void clear_console();
void interactive_menu(int is_admin);
void display_menu(int is_admin);
void handle_option(int option, int is_admin);
void add_product();
void remove_product();
void adjust_stock();
void change_price();
void list_products();
void buy_product();
void save_inventory();
void load_inventory();
int login();
int product_exists(const char* name);
int read_int(const char* prompt);
float read_float(const char* prompt);
void read_string(const char* prompt, char* buffer, int length);

int main() {
    load_inventory();
    int is_admin = login();
    interactive_menu(is_admin);
    return 0;
}

int login() {
    char username[20], password[20];
    printf("Login\nUsername: ");
    scanf("%s", username);
    printf("Password: ");
    scanf("%s", password);

    if (strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0) {
        printf(COLOR_BLUE "Welcome, Administrator!\n" COLOR_RESET);
        return 1;
    } else {
        printf(COLOR_BLUE "Welcome, Customer!\n" COLOR_RESET);
        return 0;
    }
}

void clear_console() {
    for (int i = 0; i < 100; i++) printf("\n");
}

void interactive_menu(int is_admin) {
    int choice;
    while (1) {
        display_menu(is_admin);
        choice = read_int("Enter your choice: ");
        clear_console();
        handle_option(choice, is_admin);
    }
}

void display_menu(int is_admin) {
    printf("\n=== Store Inventory Menu ===\n");
    if (is_admin) {
        printf("1. Add Product\n");
        printf("2. Remove Product\n");
        printf("3. Adjust Stock\n");
        printf("4. Change Price\n");
        printf("5. List Products\n");
        printf("6. Exit\n");
    } else {
        printf("1. List Products\n");
        printf("2. Buy Product\n");
        printf("3. Exit\n");
    }
}

void handle_option(int option, int is_admin) {
    if (is_admin) {
        switch (option) {
            case 1: add_product(); break;
            case 2: remove_product(); break;
            case 3: adjust_stock(); break;
            case 4: change_price(); break;
            case 5: list_products(); break;
            case 6: save_inventory(); printf("Exiting...\n"); exit(0);
            default: printf(COLOR_RED "Invalid choice!\n" COLOR_RESET);
        }
    } else {
        switch (option) {
            case 1: list_products(); break;
            case 2: buy_product(); break;
            case 3: printf("Exiting...\n"); exit(0);
            default: printf(COLOR_RED "Invalid choice!\n" COLOR_RESET);
        }
    }
}

void add_product() {
    if (product_count >= MAX_PRODUCTS) {
        printf(COLOR_RED "Inventory full! Cannot add more products.\n" COLOR_RESET);
        return;
    }

    char name[50];
    read_string("✅ Enter product name: ", name, sizeof(name));

    if (product_exists(name)) {
        printf(COLOR_RED "Product already exists! Use 'Adjust Stock' or 'Change Price'.\n" COLOR_RESET);
        return;
    }

    int quantity = read_int("✅ Enter quantity (positive number): ");
    if (quantity <= 0) {
        printf(COLOR_RED "Invalid quantity!\n" COLOR_RESET);
        return;
    }

    float price = read_float("✅ Enter price (e.g., 2.50): ");
    if (price <= 0) {
        printf(COLOR_RED "Invalid price!\n" COLOR_RESET);
        return;
    }

    strcpy(inventory[product_count].name, name);
    inventory[product_count].quantity = quantity;
    inventory[product_count].price = price;
    product_count++;

    save_inventory();
    printf(COLOR_BLUE "Product added successfully!\n" COLOR_RESET);
}

void remove_product() {
    char name[50];
    read_string("✅ Enter product name to remove: ", name, sizeof(name));
    for (int i = 0; i < product_count; i++) {
        if (strcmp(inventory[i].name, name) == 0) {
            for (int j = i; j < product_count - 1; j++) {
                inventory[j] = inventory[j + 1];
            }
            product_count--;
            save_inventory();
            printf(COLOR_BLUE "Product removed successfully!\n" COLOR_RESET);
            return;
        }
    }
    printf(COLOR_RED "Product not found!\n" COLOR_RESET);
}

void adjust_stock() {
    char name[50];
    read_string("✅ Enter product name to adjust stock: ", name, sizeof(name));
    for (int i = 0; i < product_count; i++) {
        if (strcmp(inventory[i].name, name) == 0) {
            int quantity = read_int("✅ Enter new quantity (positive number): ");
            inventory[i].quantity = quantity;
            save_inventory();
            printf(COLOR_BLUE "Stock updated successfully!\n" COLOR_RESET);
            return;
        }
    }
    printf(COLOR_RED "Product not found!\n" COLOR_RESET);
}

void change_price() {
    char name[50];
    read_string("✅ Enter product name to change price: ", name, sizeof(name));
    for (int i = 0; i < product_count; i++) {
        if (strcmp(inventory[i].name, name) == 0) {
            float price = read_float("✅ Enter new price: ");
            inventory[i].price = price;
            save_inventory();
            printf(COLOR_BLUE "Price updated successfully!\n" COLOR_RESET);
            return;
        }
    }
    printf(COLOR_RED "Product not found!\n" COLOR_RESET);
}

void list_products() {
    if (product_count == 0) {
        printf(COLOR_RED "No products in inventory.\n" COLOR_RESET);
        return;
    }
    printf("\n=== Inventory List ===\n");
    for (int i = 0; i < product_count; i++) {
        printf("%d. " COLOR_GREEN "%s" COLOR_RESET " - " COLOR_YELLOW "$%.2f" COLOR_RESET " (" COLOR_CYAN "%d pcs" COLOR_RESET ")\n",
               i + 1, inventory[i].name, inventory[i].price, inventory[i].quantity);
    }
}

void buy_product() {
    char name[50];
    read_string("✅ Enter product name to buy: ", name, sizeof(name));
    int quantity = read_int("✅ Enter quantity to buy: ");

    for (int i = 0; i < product_count; i++) {
        if (strcmp(inventory[i].name, name) == 0) {
            if (inventory[i].quantity >= quantity) {
                inventory[i].quantity -= quantity;
                save_inventory();
                printf(COLOR_BLUE "Purchase successful! You bought %d of %s.\n" COLOR_RESET, quantity, name);
                return;
            } else {
                printf(COLOR_RED "Not enough stock!\n" COLOR_RESET);
                return;
            }
        }
    }
    printf(COLOR_RED "Product not found!\n" COLOR_RESET);
}

void save_inventory() {
    FILE *file = fopen(FILE_NAME, "w");
    if (file == NULL) {
        printf(COLOR_RED "Error saving inventory!\n" COLOR_RESET);
        return;
    }
    for (int i = 0; i < product_count; i++) {
        fprintf(file, "%s %d %.2f\n", inventory[i].name, inventory[i].quantity, inventory[i].price);
    }
    fclose(file);
}

void load_inventory() {
    FILE *file = fopen(FILE_NAME, "r");

    

    while (fscanf(file, "%s %d %f", inventory[product_count].name,
                  &inventory[product_count].quantity, &inventory[product_count].price) == 3) {
        product_count++;
    }

    fclose(file);
}

int product_exists(const char* name) {
    for (int i = 0; i < product_count; i++) {
        if (strcmp(inventory[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}

// Input helpers
int read_int(const char* prompt) {
    int value;
    char line[256];
    while (1) {
        printf("%s", prompt);
        fgets(line, sizeof(line), stdin);
        if (sscanf(line, "%d", &value) == 1) return value;
        printf(COLOR_RED "Invalid number. Try again.\n" COLOR_RESET);
    }
}

float read_float(const char* prompt) {
    float value;
    char line[256];
    while (1) {
        printf("%s", prompt);
        fgets(line, sizeof(line), stdin);
        if (sscanf(line, "%f", &value) == 1) return value;
        printf(COLOR_RED "Invalid price. Try again.\n" COLOR_RESET);
    }
}

void read_string(const char* prompt, char* buffer, int length) {
    printf("%s", prompt);
    scanf("%s", buffer);
}
