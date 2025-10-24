#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SIZE 10

void generate_matrix(int **matrix, int size_of_matrix) {
    for (int i = 0; i < size_of_matrix; i++) {
        for (int j = 0; j < size_of_matrix; j++) {
            *(*(matrix + i) + j) = rand() % 256;
        }
    }
}

void display_matrix(int **matrix, int size_of_matrix) {
    for (int i = 0; i < size_of_matrix; i++) {
        for (int j = 0; j < size_of_matrix; j++) {
            printf("%4d", *(*(matrix + i) + j));
        }
        printf("\n");
    }
}

void rotate_matrix_clockwise(int **matrix, int size_of_matrix) {
    for (int layer = 0; layer < size_of_matrix / 2; layer++) {
        int first = layer;
        int last = size_of_matrix - 1 - layer;
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

void apply_smoothing_filter(int **matrix, int size_of_matrix) {
    for (int i = 0; i < size_of_matrix; i++) {
        for (int j = 0; j < size_of_matrix; j++) {
            int sum = 0, count = 0;
            for (int x = i - 1; x <= i + 1; x++) {
                for (int y = j - 1; y <= j + 1; y++) {
                    if (x >= 0 && x < size_of_matrix && y >= 0 && y < size_of_matrix) {
                        sum += *(*(matrix + x) + y) & 0xFF;
                        count++;
                    }
                }
            }
            int avg = sum / count;
            *(*(matrix + i) + j) = (*(*(matrix + i) + j) & 0xFF) | (avg << 8);
        }
    }

    for (int i = 0; i < size_of_matrix; i++) {
        for (int j = 0; j < size_of_matrix; j++) {
            *(*(matrix + i) + j) = (*(*(matrix + i) + j) >> 8) & 0xFF;
        }
    }
}

void process_matrix(int **matrix, int size_of_matrix) {
    printf("\nOriginal Randomly Generated Matrix:\n");
    generate_matrix(matrix, size_of_matrix);
    display_matrix(matrix, size_of_matrix);

    printf("\nMatrix after 90° Clockwise Rotation:\n");
    rotate_matrix_clockwise(matrix, size_of_matrix);
    display_matrix(matrix, size_of_matrix);

    printf("\nMatrix after Applying 3×3 Smoothing Filter:\n");
    apply_smoothing_filter(matrix, size_of_matrix);
    display_matrix(matrix, size_of_matrix);
} 

void free_matrix(int **matrix, int size_of_matrix) {
    for (int i = 0; i < size_of_matrix; i++) {
        free(*(matrix + i));
        *(matrix + i) = NULL;
    }
    free(matrix);
    matrix = NULL;
}

int main() {
    int size_of_matrix;
    printf("Enter matrix size (2-10): ");
    scanf("%d", &size_of_matrix);

    if (size_of_matrix < 2 || size_of_matrix > MAX_SIZE) {
        printf("Error: Matrix size must be between 2 and 10.\n");
        return 1;
    }

    srand(time(0));

    int **matrix = (int **)malloc(size_of_matrix * sizeof(int *));
    if (matrix == NULL) {
        printf("Memory allocation failed for matrix rows.\n");
        return 1;
    }

    for (int i = 0; i < size_of_matrix; i++) {
        *(matrix + i) = (int *)malloc(size_of_matrix * sizeof(int));
        if (*(matrix + i) == NULL) {
            printf("Memory allocation failed for matrix columns.\n");
            for (int j = 0; j < i; j++) {
                free(*(matrix + j));
                *(matrix + j) = NULL;
            }
            free(matrix);
            matrix = NULL;
            return 1;
        }
    }

    process_matrix(matrix, size_of_matrix);
    free_matrix(matrix, size_of_matrix);

    return 0;
}
