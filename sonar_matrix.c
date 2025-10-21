#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SIZE 10

void generate_matrix(int **matrix, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            *(*(matrix + i) + j) = rand() % 256;
}

void display_matrix(int **matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            printf("%4d", *(*(matrix + i) + j));
        printf("\n");
    }
}

void rotate_matrix_clockwise(int **matrix, int n) {
    for (int layer = 0; layer < n / 2; layer++) {
        int first = layer;
        int last = n - 1 - layer;
        for (int i = first; i < last; i++) {
            int offset = i - first;
            int *top = *(matrix + first) + i;
            int *left = *(matrix + (last - offset)) + first;
            int *bottom = *(matrix + last) + (last - offset);
            int *right = *(matrix + i) + last;

            int temp = *top;
            *top = *left;
            *left = *bottom;
            *bottom = *right;
            *right = temp;
        }
    }
}

void apply_smoothing_filter(int **matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int sum = 0, count = 0;
            for (int x = i - 1; x <= i + 1; x++) {
                for (int y = j - 1; y <= j + 1; y++) {
                    if (x >= 0 && x < n && y >= 0 && y < n) {
                        sum += *(*(matrix + x) + y);
                        count++;
                    }
                }
            }
            *(*(matrix + i) + j) = sum / count;
        }
    }
}

int main() {
    int n;
    printf("Enter matrix size (2-10): ");
    scanf("%d", &n);

    if (n < 2 || n > 10) {
        printf("Error: Matrix size must be between 2 and 10.\n");
        return 1;
    }

    int **matrix = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
        *(matrix + i) = (int *)malloc(n * sizeof(int));

    srand(time(0));

    printf("\nOriginal Randomly Generated Matrix:\n");
    generate_matrix(matrix, n);
    display_matrix(matrix, n);

    printf("\nMatrix after 90° Clockwise Rotation:\n");
    rotate_matrix_clockwise(matrix, n);
    display_matrix(matrix, n);

    printf("\nMatrix after Applying 3×3 Smoothing Filter:\n");
    apply_smoothing_filter(matrix, n);
    display_matrix(matrix, n);

    for (int i = 0; i < n; i++)
        free(*(matrix + i));
    free(matrix);

    return 0;
}
