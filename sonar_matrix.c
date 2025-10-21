#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    int n;
    printf("Enter matrix size (2-10): ");
    scanf("%d", &n);

    int **matrix = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
        *(matrix + i) = (int *)malloc(n * sizeof(int));

    srand(time(NULL));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            *(*(matrix + i) + j) = rand() % 256;

    printf("\nOriginal Randomly Generated Matrix:\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            printf("%3d ", *(*(matrix + i) + j));
        printf("\n");
    }

    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++) {
            int temp = *(*(matrix + i) + j);
            *(*(matrix + i) + j) = *(*(matrix + j) + i);
            *(*(matrix + j) + i) = temp;
        }

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n / 2; j++) {
            int temp = *(*(matrix + i) + j);
            *(*(matrix + i) + j) = *(*(matrix + i) + (n - 1 - j));
            *(*(matrix + i) + (n - 1 - j)) = temp;
        }

    printf("\nMatrix after 90° Clockwise Rotation:\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            printf("%3d ", *(*(matrix + i) + j));
        printf("\n");
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int sum = 0, count = 0;
            for (int x = -1; x <= 1; x++) {
                for (int y = -1; y <= 1; y++) {
                    int ni = i + x, nj = j + y;
                    if (ni >= 0 && nj >= 0 && ni < n && nj < n) {
                        sum += *(*(matrix + ni) + nj);
                        count++;
                    }
                }
            }
            *(*(matrix + i) + j) = sum / count;
        }
    }

    printf("\nMatrix after Applying 3×3 Smoothing Filter:\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            printf("%3d ", *(*(matrix + i) + j));
        printf("\n");
    }

    for (int i = 0; i < n; i++)
        free(*(matrix + i));
    free(matrix);

    return 0;
}
