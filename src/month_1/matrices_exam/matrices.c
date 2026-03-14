#include <stdio.h>

// Ninrapat Suttinual - 13 March 2026

// Left -> 1. Inverse of a 2x2 matrix:
//      -> 2. Display the invserse of the 2x2 matrix in a formatted manner.

#define MAX_SIZE 10

struct matrix
{
    float mvalues[MAX_SIZE][MAX_SIZE];
    int nrows;
    int ncols;
};

void read_int_matrix(struct matrix *m)
{
    FILE *file = fopen("matrix1.txt", "r");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return;
    }
    printf("Reading integer matrix from file:\n");

    for (int i = 0; i < MAX_SIZE; i++)
    {
        for (int j = 0; j < MAX_SIZE; j++)
        {
            fscanf(file, "%f", &m->mvalues[i][j]);
        }
    }
    fclose(file);
}

void get_user_input(struct matrix *m)
{

    int valid;
    do
    {
        printf("Enter the number of rows (1-%d): ", MAX_SIZE);
        valid = scanf("%d", &m->nrows);

        if (valid != 1)
        {
            printf("Error: please enter a number!\n");
            while (getchar() != '\n')
                ;
            // flush 'a' out of buffer
        }
        else if (m->nrows < 1 || m->nrows > 10)
        {
            printf("Error: must be between 1 and 10!\n");
        }

    } while (valid != 1 || m->nrows < 1 || m->nrows > MAX_SIZE);

    do
    {
        printf("Enter the number of columns (1-%d): ", MAX_SIZE);
        valid = scanf("%d", &m->ncols);

        if (valid != 1)
        {
            printf("Error: please enter a number!\n");
            while (getchar() != '\n')
                ;
        }
        else if (m->ncols < 1 || m->ncols > 10)
        {
            printf("Error: must be between 1 and 10!\n");
        }

    } while (valid != 1 || m->ncols < 1 || m->ncols > MAX_SIZE);
}

float Cal_determinant(struct matrix m, int rows, int cols)
{

    float a = m.mvalues[rows][cols];
    float b = m.mvalues[rows][cols + 1];
    float c = m.mvalues[rows + 1][cols];
    float d = m.mvalues[rows + 1][cols + 1];

    /*  Matrix layout:

        [ a  b ]
        [ c  d ]

        determinant = (a * d) - (b * c)
    */

    return (a * d) - (b * c);
}

float Cal_inverse(struct matrix *m, int rows, int cols)
{
    float det = Cal_determinant(*m, rows, cols);

    float a = m->mvalues[rows][cols];
    float b = m->mvalues[rows][cols + 1];
    float c = m->mvalues[rows + 1][cols];
    float d = m->mvalues[rows + 1][cols + 1];

    // Inverse of a 2x2 matrix:
    // [ a  b ]^-1 = (1/det) * [ d  -b ]
    // [ c  d ]                [ -c  a ]

    float inv_a = d / det;
    float inv_b = -b / det;
    float inv_c = -c / det;
    float inv_d = a / det;

    printf("Inverse of the 2x2 matrix starting at (%d, %d):\n", rows, cols);
    printf("\n");
    printf("| %7.2f %7.2f |\n", inv_a, inv_b);
    printf("| %7.2f %7.2f |\n", inv_c, inv_d);

    return det;
}

int Valid_Input(char *prompt, int min, int max)
{
    int value, valid;
    do
    {
        printf("%s", prompt);
        valid = scanf("%d", &value);
        if (valid != 1)
        {
            printf("Error: please enter a number!\n");
            while (getchar() != '\n')
                ;
        }
        else if (value < min || value > max)
        {
            printf("Error: must be between %d and %d!\n", min, max);
        }
    } while (valid != 1 || value < min || value > max);
    return value;
}

int main()
{

    int running = 1;
    int valid_exit;

    while (running)
    {

        struct matrix matrixs;
        read_int_matrix(&matrixs);
        get_user_input(&matrixs);

        printf("Matrix values:\n");
        for (int i = 0; i < matrixs.nrows; i++)
        {
            printf("| ");
            for (int j = 0; j < matrixs.ncols; j++)
            {
                printf("%7.2f ", matrixs.mvalues[i][j]);
            }
            printf("|\n");
        }

        printf("\n");
        printf("Finding the determinant of the matrix now \n");

        int start_row, start_col;

        start_row = Valid_Input("Enter start row: ", 0, matrixs.nrows - 2);
        start_col = Valid_Input("Enter start col: ", 0, matrixs.ncols - 2);

        if (start_row < 0 || start_row > 3 || start_col < 0 || start_col > 5)
        {
            printf("Error: Row and column numbers must be between 0 and 5.\n");
        }
        else
        {
            float det = Cal_determinant(matrixs, start_row, start_col);
            printf("Determinant of the 2x2 matrix starting at (%d, %d) is: \n %.2f\n", start_row, start_col, det);
            for (int i = start_row; i < start_row + 2; i++)
            {
                printf("| ");
                for (int j = start_col; j < start_col + 2; j++)
                {
                    printf("%7.2f ", matrixs.mvalues[i][j]);
                }
                printf("|\n");
            }
            printf("\n");
            printf("Finding the inverse of the 2x2 matrix now \n");
                if (det == 0)
                {
                    printf("The matrix is singular and does not have an inverse.\n");
                }
                else
                {
                    Cal_inverse(&matrixs, start_row, start_col);
                }
        }

        // wait until user is decides to continue or not
        running = Valid_Input("Would you like to continue? (0=no, 1=yes): ", 0, 1);
    }

    return 0;
}