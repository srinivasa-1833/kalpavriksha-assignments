#include <stdio.h>
#include <string.h>

struct Student {
    int roll;
    char name[50];
    int marks[3];
    int total;
    float average;
    char grade;
};

void calculate(struct Student *s) {
    s->total = s->marks[0] + s->marks[1] + s->marks[2];
    s->average = s->total / 3.0;

    if (s->average >= 85)
        s->grade = 'A';
    else if (s->average >= 70)
        s->grade = 'B';
    else if (s->average >= 50)
        s->grade = 'C';
    else if (s->average >= 35)
        s->grade = 'D';
    else
        s->grade = 'F';
}

void performance(char grade) {
    int stars = 0;
    if (grade == 'A') stars = 5;
    else if (grade == 'B') stars = 4;
    else if (grade == 'C') stars = 3;
    else if (grade == 'D') stars = 2;
    else return;
    for (int i = 0; i < stars; i++) {
        printf("*");
    }
}

void printRollNumbers(int n) {
    if (n == 0) return;
    printRollNumbers(n - 1);
    printf("%d ", n);
}

void inputStudents(struct Student s[], int studentCount) {
    for (int i = 0; i < studentCount; i++) {
        scanf("%d %s %d %d %d", &s[i].roll, s[i].name, &s[i].marks[0], &s[i].marks[1], &s[i].marks[2]);

        if (s[i].roll <= 0) {
            printf("Invalid roll number. Skipping student.\n");
            continue;
        }

        int valid = 1;
        for (int j = 0; j < 3; j++) {
            if (s[i].marks[j] < 0 || s[i].marks[j] > 100) {
                valid = 0;
                break;
            }
        }

        if (!valid) {
            printf("Invalid marks for student %s. Skipping entry.\n", s[i].name);
            continue;
        }

        calculate(&s[i]);
    }
}

void displayStudents(struct Student s[], int studentCount) {
    for (int i = 0; i < studentCount; i++) {
        if (s[i].roll <= 0) continue;
        printf("Roll: %d\n", s[i].roll);
        printf("Name: %s\n", s[i].name);
        printf("Total: %d\n", s[i].total);
        printf("Average: %.2f\n", s[i].average);
        printf("Grade: %c\n", s[i].grade);

        if (s[i].average < 35) continue;

        printf("Performance: ");
        performance(s[i].grade);
        printf("\n\n");
    }
}

int main() {
    int studentCount;
    scanf("%d", &studentCount);

    if (studentCount <= 0 || studentCount > 100) {
        printf("Invalid number of students.\n");
        return 0;
    }

    struct Student s[studentCount];
    inputStudents(s, studentCount);
    displayStudents(s, studentCount);

    printf("List of Roll Numbers (via recursion): ");
    printRollNumbers(studentCount);
    printf("\n");

    return 0;
}
