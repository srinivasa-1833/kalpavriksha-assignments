#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_LENGTH 100

int main() {
    char expr[MAX_LENGTH];
    int numbers[MAX_LENGTH], numCount = 0;
    char operators[MAX_LENGTH]; 
    int opCount = 0;

    printf("Enter expression: ");
    fgets(expr, MAX_LENGTH, stdin);

    for(int i = 0; expr[i]; i++){
        if(expr[i] == '\n') expr[i] = '\0';
    }

    int i = 0;
    while(expr[i]) {
        if(isspace(expr[i])) { 
            i++; 
            continue; 
        }

        if(isdigit(expr[i])) {
            int val = 0;
            while(isdigit(expr[i])) {
                val = val * 10 + (expr[i] - '0');
                i++;
            }
            numbers[numCount++] = val;
        }
        else if(expr[i]=='+' || expr[i]=='-' || expr[i]=='*' || expr[i]=='/') {
            operators[opCount++] = expr[i];
            i++;
        }
        else {
            printf("Error: Invalid expression.\n");
            return 0;
        }
    }

    for(i = 0; i < opCount; i++) {
        if(operators[i] == '*' || operators[i] == '/') {
            if(operators[i] == '/' && numbers[i+1] == 0) {
                printf("Error: Division by zero.\n");
                return 0;
            }
            numbers[i] = (operators[i] == '*') ? numbers[i] * numbers[i+1] : numbers[i] / numbers[i+1];
            for(int j = i+1; j < numCount-1; j++) numbers[j] = numbers[j+1];
            for(int j = i; j < opCount-1; j++) operators[j] = operators[j+1];
            numCount--; 
            opCount--; 
            i--;
        }
    }

    int result = numbers[0];
    for(i = 0; i < opCount; i++) {
        if(operators[i] == '+') result += numbers[i+1];
        else result -= numbers[i+1];
    }

    printf("%d\n", result);
    return 0;
}
