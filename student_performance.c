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

int main() {
    int n;
    scanf("%d", &n);
    struct Student s[n];

    for (int i = 0; i < n; i++) {
        scanf("%d %s %d %d %d", &s[i].roll, s[i].name, &s[i].marks[0], &s[i].marks[1], &s[i].marks[2]);
        calculate(&s[i]);
    }

    for (int i = 0; i < n; i++) {
        printf("Roll: %d\n", s[i].roll);
        printf("Name: %s\n", s[i].name);
        printf("Total: %d\n", s[i].total);
        printf("Average: %.2f\n", s[i].average);
        printf("Grade: %c\n", s[i].grade);

        if (s[i].average < 35) {
            continue;
        }

        printf("Performance: ");
        performance(s[i].grade);
        printf("\n");
    }

    printf("List of Roll Numbers (via recursion): ");
    printRollNumbers(n);
    printf("\n");
    return 0;
}
