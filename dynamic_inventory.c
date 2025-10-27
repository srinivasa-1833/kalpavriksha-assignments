#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LENGTH 50

typedef struct {
    int id;
    char name[NAME_LENGTH];
    float price;
    int quantity;
} Product;

void inputInitialProducts(Product *products, int count) {
    for (int i = 0; i < count; i++) {
        printf("Enter details for product %d:\n", i + 1);
        printf("Product ID: ");
        scanf("%d", &products[i].id);
        printf("Product Name: ");
        scanf(" %[^\n]", products[i].name);
        printf("Product Price: ");
        scanf("%f", &products[i].price);
        printf("Product Quantity: ");
        scanf("%d", &products[i].quantity);
    }
}

void addProduct(Product **products, int *count) {
    *products = realloc(*products, (*count + 1) * sizeof(Product));
    if (*products == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    Product *newProduct = &(*products)[*count];
    printf("Enter Product ID: ");
    scanf("%d", &newProduct->id);
    printf("Enter Product Name: ");
    scanf(" %[^\n]", newProduct->name);
    printf("Enter Product Price: ");
    scanf("%f", &newProduct->price);
    printf("Enter Product Quantity: ");
    scanf("%d", &newProduct->quantity);

    (*count)++;
    printf("Product added successfully!\n");
}

void viewProducts(Product *products, int count) {
    if (count == 0) {
        printf("No products available.\n");
        return;
    }

    printf("========= PRODUCT LIST =========\n");
    for (int i = 0; i < count; i++) {
        printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
               products[i].id, products[i].name, products[i].price, products[i].quantity);
    }
}

void updateQuantity(Product *products, int count) {
    int id, newQty, found = 0;
    printf("Enter Product ID to update quantity: ");
    scanf("%d", &id);

    for (int i = 0; i < count; i++) {
        if (products[i].id == id) {
            printf("Enter new Quantity: ");
            scanf("%d", &newQty);
            products[i].quantity = newQty;
            printf("Quantity updated successfully!\n");
            found = 1;
            break;
        }
    }

    if (!found)
        printf("Product not found.\n");
}

// Search product by ID
void searchByID(Product *products, int count) {
    int id, found = 0;
    printf("Enter Product ID to search: ");
    scanf("%d", &id);

    for (int i = 0; i < count; i++) {
        if (products[i].id == id) {
            printf("Product Found: Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   products[i].id, products[i].name, products[i].price, products[i].quantity);
            found = 1;
            break;
        }
    }

    if (!found)
        printf("Product not found.\n");
}

void searchByName(Product *products, int count) {
    char keyword[NAME_LENGTH];
    int found = 0;
    printf("Enter name to search (partial allowed): ");
    scanf(" %[^\n]", keyword);

    printf("Products Found:\n");
    for (int i = 0; i < count; i++) {
        if (strstr(products[i].name, keyword)) {
            printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   products[i].id, products[i].name, products[i].price, products[i].quantity);
            found = 1;
        }
    }

    if (!found)
        printf("No products match the search.\n");
}

void searchByPriceRange(Product *products, int count) {
    float minPrice, maxPrice;
    int found = 0;
    printf("Enter minimum price: ");
    scanf("%f", &minPrice);
    printf("Enter maximum price: ");
    scanf("%f", &maxPrice);

    printf("Products in price range:\n");
    for (int i = 0; i < count; i++) {
        if (products[i].price >= minPrice && products[i].price <= maxPrice) {
            printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   products[i].id, products[i].name, products[i].price, products[i].quantity);
            found = 1;
        }
    }

    if (!found)
        printf("No products found in the given price range.\n");
}

void deleteProduct(Product **products, int *count) {
    int id, found = 0, index = -1;
    printf("Enter Product ID to delete: ");
    scanf("%d", &id);

    for (int i = 0; i < *count; i++) {
        if ((*products)[i].id == id) {
            index = i;
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Product not found.\n");
        return;
    }

    for (int i = index; i < *count - 1; i++)
        (*products)[i] = (*products)[i + 1];

    *count -= 1;
    *products = realloc(*products, (*count) * sizeof(Product));

    if (*products == NULL && *count > 0) {
        printf("Memory reallocation failed after deletion.\n");
        exit(1);
    }

    printf("Product deleted successfully!\n");
}

void displayMenu() {
    printf("\n========= INVENTORY MENU =========\n");
    printf("1. Add New Product\n");
    printf("2. View All Products\n");
    printf("3. Update Quantity\n");
    printf("4. Search Product by ID\n");
    printf("5. Search Product by Name\n");
    printf("6. Search Product by Price Range\n");
    printf("7. Delete Product\n");
    printf("8. Exit\n");
    printf("Enter your choice: ");
}

int main() {
    int count;
    printf("Enter initial number of products: ");
    scanf("%d", &count);

    if (count < 1 || count > 100) {
        printf("Invalid number of products.\n");
        return 1;
    }

    Product *products = calloc(count, sizeof(Product));
    if (products == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    inputInitialProducts(products, count);

    int choice;
    do {
        displayMenu();
        scanf("%d", &choice);

        switch (choice) {
            case 1: addProduct(&products, &count); break;
            case 2: viewProducts(products, count); break;
            case 3: updateQuantity(products, count); break;
            case 4: searchByID(products, count); break;
            case 5: searchByName(products, count); break;
            case 6: searchByPriceRange(products, count); break;
            case 7: deleteProduct(&products, &count); break;
            case 8:
                free(products);
                products = NULL;
                printf("Memory released successfully. Exiting program...\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 8);

    return 0;
}
