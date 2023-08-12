#include <stdio.h>
#include <sys/time.h>

#define MATRIX_SIZE 6
#define MAX_SCALER 16777215

float m_well[MATRIX_SIZE][MATRIX_SIZE] = 
{
    {5, 2, 1, 1, 2, 1},
    {5, 1, 7, 1, 1, 3},
    {7, 1, 7, 9, 1, 2},
    {1, 1, 5, 3, 3, 5},
    {1, 2, 5, 2, 2, 1},
    {1, 9, 0, 5, 1, 7}
};

float m_well_result[MATRIX_SIZE][MATRIX_SIZE] = 
{
    {1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1},
};

float scale_factor_calculation(float matrix[MATRIX_SIZE][MATRIX_SIZE]){
    float scale_factor = 0;
    float max = 0;

    //Loop through matrix to find the maximum element
    for(int i = 0; i < MATRIX_SIZE; i++){
        for(int j = 0; j < MATRIX_SIZE; j++){
            if (matrix[i][j] > max) max = matrix[i][j];
        }
    }
    if(max > 0) scale_factor = MAX_SCALER/max;

    return scale_factor;
}

float scale_matrix(float inverted_matrix[MATRIX_SIZE][MATRIX_SIZE], int scale_factor){
    //Scale each element in the matrix
    for(int i = 0; i < MATRIX_SIZE; i++){
        for(int j = 0; j < MATRIX_SIZE; j++){
            inverted_matrix[i][j]*=scale_factor;
        }
    }
}

float approximate_norm(float matrix[MATRIX_SIZE][MATRIX_SIZE]){
    //Goal: estimate the condition number without calculating the norm
    //Approximate the matrix norm by find the maximum absolute row sum

    float norm = 0;
    for(int i = 0; i < MATRIX_SIZE; i++){
        float row_sum = 0;
        for(int j = 0; j < MATRIX_SIZE; j++){
            if(matrix[i][j] < 0){
                //Multiply index by -1 if negative
                row_sum+=(matrix[i][j]*(-1));
            }else{
                row_sum+=matrix[i][j];
            }
        }
        if(row_sum > norm) norm = row_sum;
    }
    return norm;
}

void swapRows(float matrix[MATRIX_SIZE][MATRIX_SIZE], int row1, int row2) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        float temp = matrix[row1][i];
        matrix[row1][i] = matrix[row2][i];
        matrix[row2][i] = temp;
    }
}

void scaleRow(float matrix[MATRIX_SIZE][MATRIX_SIZE], int row, float scalar) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        matrix[row][i] *= scalar;
    }
}

void addRows(float matrix[MATRIX_SIZE][MATRIX_SIZE], int srcRow, int destRow, float scalar) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        matrix[destRow][i] += scalar * matrix[srcRow][i];
    }
}

void invertMatrix(float matrix[MATRIX_SIZE][MATRIX_SIZE], float inverse[MATRIX_SIZE][MATRIX_SIZE]) {

    for (int i = 0; i < MATRIX_SIZE; i++) {
        // If the pivot element is zero, swap rows
        float pivot = matrix[i][i];
        if (pivot == 0) {
            int j;
            for (j = i + 1; j < MATRIX_SIZE; j++) {
                if (matrix[j][i] != 0) {
                    swapRows(matrix, i, j);
                    swapRows(inverse, i, j);
                    break;
                }
            }
            if (j == MATRIX_SIZE) {
                printf("Matrix is not invertible\n");
                return;
            }
        }

        // Scale the pivot row
        scaleRow(matrix, i, 1.0 / pivot);
        scaleRow(inverse, i, 1.0 / pivot);

        // Eliminate other rows
        for (int j = 0; j < MATRIX_SIZE; j++) {
            if (j != i) {
                float factor = matrix[j][i];
                addRows(matrix, i, j, -factor);
                addRows(inverse, i, j, -factor);
            }
        }
    }
}


float compute_condition_num(float matrix[MATRIX_SIZE][MATRIX_SIZE], float inverse[MATRIX_SIZE][MATRIX_SIZE]){
    //Goal: estimate the condition number without calculating the norm
    //Approximate the matrix norm by find the maximum absolute row sum
    float norm = approximate_norm(matrix);
    //printf("Norm is:%f\n", norm);

    invertMatrix(matrix, inverse);


    float inverse_norm = approximate_norm(inverse);
    //printf("Inverse norm is:%f\n", inverse_norm);

    float condition_num = norm*inverse_norm;
    
    return condition_num;
}

void printMatrix(float matrix[MATRIX_SIZE][MATRIX_SIZE]) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            printf("%.8f\t", matrix[i][j]);
        }
        printf("\n");
    }
}

int start_process(){
    printf("Original matrx: \n");
    printMatrix(m_well);
    printf("\n");
    float scale_factor = scale_factor_calculation(m_well);
    float condition_number = compute_condition_num(m_well, m_well_result);
    scale_matrix(m_well, scale_factor);
    printf("Inverted matrx: \n");
    printMatrix(m_well_result);
    printf("\n");
    
    return 0;
}
//Function to get the time it takes for the program to run
void start_process_time(){
    double runtime = 0;

    for(int i = 0; i < 100; i++){

        struct timeval start, end;
        double elapsed_time;

        gettimeofday(&start, NULL);
        float scale_factor = scale_factor_calculation(m_well);
        float condition_number = compute_condition_num(m_well, m_well_result);
        scale_matrix(m_well, scale_factor);

        gettimeofday(&end, NULL);
        elapsed_time = (end.tv_sec - start.tv_sec) * 1000000.0 + (end.tv_usec - start.tv_usec);

        runtime+=elapsed_time;

    }

    printf("Run time avg: %.2f\n", runtime/100);
}

int main() {

    start_process();

    start_process_time();

    return 0;
}