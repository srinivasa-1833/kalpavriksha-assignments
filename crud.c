#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char name[50];
    int age;
} User;

void createUser() {
    User u;
    FILE *f = fopen("users.txt", "a");
    if (!f) { printf("Cannot open file.\n"); return; }

    printf("Enter ID: "); scanf("%d", &u.id);
    printf("Enter Name: "); scanf(" %[^\n]", u.name);
    printf("Enter Age: "); scanf("%d", &u.age);

    fprintf(f, "%d|%s|%d\n", u.id, u.name, u.age);
    fclose(f);
    printf("User added successfully.\n");
}

void readUsers() {
    User u;
    FILE *f = fopen("users.txt", "r");
    if (!f) { printf("No users found.\n"); return; }

    printf("\nID\tName\tAge\n");
    while(fscanf(f, "%d|%49[^|]|%d\n", &u.id, u.name, &u.age) != EOF) {
        printf("%d\t%s\t%d\n", u.id, u.name, u.age);
    }
    fclose(f);
}

void updateUser() {
    int id, found=0;
    User u;
    FILE *f = fopen("users.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!f || !temp) { printf("Cannot open file.\n"); return; }

    printf("Enter ID to update: "); scanf("%d", &id);

    while(fscanf(f, "%d|%49[^|]|%d\n", &u.id, u.name, &u.age) != EOF) {
        if(u.id == id) {
            found = 1;
            printf("Enter new Name: "); scanf(" %[^\n]", u.name);
            printf("Enter new Age: "); scanf("%d", &u.age);
        }
        fprintf(temp, "%d|%s|%d\n", u.id, u.name, u.age);
    }

    fclose(f); fclose(temp);
    remove("users.txt");
    rename("temp.txt", "users.txt");

    if(found) printf("User updated successfully.\n");
    else printf("User ID not found.\n");
}

void deleteUser() {
    int id, found=0;
    User u;
    FILE *f = fopen("users.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!f || !temp) { printf("Cannot open file.\n"); return; }

    printf("Enter ID to delete: "); scanf("%d", &id);

    while(fscanf(f, "%d|%49[^|]|%d\n", &u.id, u.name, &u.age) != EOF) {
        if(u.id == id) {
            found = 1;
            continue;
        }
        fprintf(temp, "%d|%s|%d\n", u.id, u.name, u.age);
    }

    fclose(f); fclose(temp);
    remove("users.txt");
    rename("temp.txt", "users.txt");

    if(found) printf("User deleted successfully.\n");
    else printf("User ID not found.\n");
}

int main() {
    int choice;
    while(1) {
        printf("\n1.Create 2.Read 3.Update 4.Delete 5.Exit\nEnter choice: ");
        scanf("%d", &choice);
        switch(choice) {
            case 1: createUser(); break;
            case 2: readUsers(); break;
            case 3: updateUser(); break;
            case 4: deleteUser(); break;
            case 5: exit(0);
            default: printf("Invalid choice.\n");
        }
    }
    return 0;
}
