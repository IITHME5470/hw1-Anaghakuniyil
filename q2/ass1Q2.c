#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Function 
double *matrix_vector_multiply(double **matrix, double *vector, int n);
int is_eigenvector(double **matrix, double *vector, int n, double *eigenvalue);
void append_eigenvalue_to_file(const char *filename, double eigenvalue);

int main() {
    // Step 1: Read the matrix sizes `n` from "input.in"
    FILE *input_file = fopen("input.in", "r");
    if (!input_file) {
        perror("Error in opening input.in");
        return EXIT_FAILURE;
    }
    int n;
   
    fscanf(input_file, "%d", &n);
    fclose(input_file);
    printf("n = %d\n",n);

    // Step 2: Read the matrix from "mat_fnnnnnn.in" file
    char matrix_filename[20];
    snprintf(matrix_filename, sizeof(matrix_filename), "mat_%06d.in", n);

    FILE *matrix_file = fopen(matrix_filename, "r");
    if (!matrix_file) {
        perror("Error in opening matrix file");
        return EXIT_FAILURE;
    }

    double **matrix = malloc(n * sizeof(double *));
    for (int i = 0; i < n; i++) {
        matrix[i] = malloc(n * sizeof(double));
        for (int j = 0; j < n; j++) {
            fscanf(matrix_file, "%lf", &matrix[i][j]);
        }
    }
    fclose(matrix_file);

    // Step 3: Check each vector file from inputs
    for (int vecnum = 1; ; vecnum++) {
        // Generate vector filename in here
        char vector_filename[30];
        snprintf(vector_filename, sizeof(vector_filename), "vec_%06d_%06d.in", n, vecnum);

        // Open the vector file in here
        FILE *vector_file = fopen(vector_filename, "r");
        if (!vector_file) {
            // Break the loop file does not exist (assume no more vectors)
            break;
        }

        // Read the vector file from the input
        double *vector = malloc(n * sizeof(double));
        for (int i = 0; i < n; i++) {
            fscanf(vector_file, "%lf", &vector[i]);
        }
        fclose(vector_file);

        // Check if the input is an eigenvector
        double eigenvalue;
        if (is_eigenvector(matrix, vector, n, &eigenvalue)) {
            printf("%s : Yes : %.15e\n", vector_filename, eigenvalue);
            append_eigenvalue_to_file(vector_filename, eigenvalue);
        } else {
            printf("%s :Is not an eigenvector\n", vector_filename);
        }

        free(vector);
    }

    // Free allocated memory for the matrix from disk
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);

    return EXIT_SUCCESS;
}

// Function used for multiplying  a matrix with a vector
double *matrix_vector_multiply(double **matrix, double *vector, int n) {
    double *result = malloc(n * sizeof(double));
    for (int i = 0; i < n; i++) {
        result[i] = 0.0;
        for (int j = 0; j < n; j++) {
            result[i] += matrix[i][j] * vector[j];
        }
    }
    return result;
}

// Function to check if the vector is an eigenvector and calculate its eigenvalue
int is_eigenvector(double **matrix, double *vector, int n, double *eigenvalue) {
    double *result = matrix_vector_multiply(matrix, vector, n);
    int valid_eigenvalue = 0;  // To track if a valid eigenvalue is found

    for (int i = 0; i < n; i++) {
        if (fabs(vector[i]) > 1e-6) {  // Check if the vector element is not zero here
            double current_eigenvalue = result[i] / vector[i];
            if (!valid_eigenvalue) {
                *eigenvalue = current_eigenvalue;  // Set eigenvalue for the first non-zero element in here
                valid_eigenvalue = 1;
            } else if (fabs(current_eigenvalue - *eigenvalue) > 1e-6) {
                free(result);
                return 0;  // Shows eigenvalue mismatched
            }
        } else if (fabs(result[i]) > 1e-6) {  
            // If vector(i) is zero but result(i) is non-zero, it's not an eigenvector
            free(result);
            return 0;
        }
    }

    free(result);
    return valid_eigenvalue;  // Return true if a true eigenvalue was found
}

// Function to append the eigenvalue to the vector files
void append_eigenvalue_to_file(const char *filename, double eigenvalue) {
    FILE *file = fopen(filename, "a");
    if (!file) {
        perror("Error in  opening vector file for appending");
        return;
    }
    fprintf(file, " %.15e\n", eigenvalue);
    fclose(file);
}
