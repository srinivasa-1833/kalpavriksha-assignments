#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_SIZE 100

int main() {
    char expr[MAX_SIZE];
    int nums[MAX_SIZE], numCount = 0;
    char ops[MAX_SIZE]; 
    int opCount = 0;

    printf("Enter expression: ");
    fgets(expr, MAX_SIZE, stdin);

    for(int i=0; expr[i]; i++){
        if(expr[i]=='\n') expr[i]='\0';
    }

    int i = 0;
    while(expr[i]) {
        if(isspace(expr[i])) { i++; continue; }

        if(isdigit(expr[i])) {
            int val = 0;
            while(isdigit(expr[i])) {
                val = val*10 + (expr[i]-'0');
                i++;
            }
            nums[numCount++] = val;
        }
        else if(expr[i]=='+' || expr[i]=='-' || expr[i]=='*' || expr[i]=='/') {
            ops[opCount++] = expr[i];
            i++;
        }
        else {
            printf("Error: Invalid expression.\n");
            return 0;
        }
    }

    for(i=0; i<opCount; i++) {
        if(ops[i]=='*' || ops[i]=='/') {
            if(ops[i]=='/' && nums[i+1]==0) {
                printf("Error: Division by zero.\n");
                return 0;
            }
            nums[i] = (ops[i]=='*') ? nums[i]*nums[i+1] : nums[i]/nums[i+1];
            for(int j=i+1; j<numCount-1; j++) nums[j] = nums[j+1];
            for(int j=i; j<opCount-1; j++) ops[j] = ops[j+1];
            numCount--; opCount--; i--;
        }
    }

    int result = nums[0];
    for(i=0; i<opCount; i++) {
        if(ops[i]=='+') result += nums[i+1];
        else result -= nums[i+1];
    }

    printf("%d\n", result);
    return 0;
}
