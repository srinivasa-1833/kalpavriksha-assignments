#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id, age;
    char name[50];
} User;

void addUser() {
    User u;
    FILE *f = fopen("users.txt", "a");
    if (!f) return;

    printf("Enter ID: "); scanf("%d", &u.id);
    printf("Enter Name: "); scanf(" %[^\n]", u.name);
    printf("Enter Age: "); scanf("%d", &u.age);

    fprintf(f, "%d|%s|%d\n", u.id, u.name, u.age);
    fclose(f);
    printf("User added successfully.\n");
}

void displayUsers() {
    User u;
    FILE *f = fopen("users.txt", "r");
    if (!f) { printf("No users found.\n"); return; }

    printf("\nID\tName\tAge\n");
    while(fscanf(f, "%d|%49[^|]|%d\n", &u.id, u.name, &u.age) != EOF)
        printf("%d\t%s\t%d\n", u.id, u.name, u.age);

    fclose(f);
}

int processUserById(int id, int operation) {
    User u;
    int found = 0;
    FILE *f = fopen("users.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!f || !temp) return 0;

    while(fscanf(f, "%d|%49[^|]|%d\n", &u.id, u.name, &u.age) != EOF) {
        if(u.id == id) {
            found = 1;
            if(operation == 1) {
                printf("Enter new Name: "); scanf(" %[^\n]", u.name);
                printf("Enter new Age: "); scanf("%d", &u.age);
            }
            if(operation == 2) continue;
        }
        fprintf(temp, "%d|%s|%d\n", u.id, u.name, u.age);
    }

    fclose(f); fclose(temp);
    remove("users.txt");
    rename("temp.txt", "users.txt");

    return found;
}

void modifyUserById() {
    int id;
    printf("Enter ID to update: "); scanf("%d", &id);
    if(processUserById(id, 1)) printf("User updated successfully.\n");
    else printf("User ID not found.\n");
}

void deleteUserById() {
    int id;
    printf("Enter ID to delete: "); scanf("%d", &id);
    if(processUserById(id, 2)) printf("User deleted successfully.\n");
    else printf("User ID not found.\n");
}

int main() {
    int choice;
    while(1) {
        printf("\n1.Add 2.Display 3.Update 4.Delete 5.Exit\nEnter choice: ");
        scanf("%d", &choice);
        switch(choice) {
            case 1: addUser(); break;
            case 2: displayUsers(); break;
            case 3: modifyUserById(); break;
            case 4: deleteUserById(); break;
            case 5: exit(0);
            default: printf("Invalid choice.\n");
        }
    }
    return 0;
}
