#include <stdio.h>

/* Created by Ninrapat Suttinual 

    This program read a provided 100 number from matrix1.txt file and
    store them in 10x10 matrix. Then the key feature of this program is first to ask user to 
    input the number of rows and columns they want to use from the 10x10 matrix. 
    Then it will ask user to input the starting row and column number for a 2x2 submatrix within
    the selected rows and columns. Finally, it will use the selected 2x2 submatrix to calculate 
    and display the determinant and inverse of that submatrix. The program will continue to run 
    until the user decides to stop it.
  
*/ 

// Define the maximum size of the matrix
#define MAX_SIZE 10

// Define a structure to represent a matrix
struct matrix
{
    float mvalues[MAX_SIZE][MAX_SIZE];
    int nrows;
    int ncols;
};

void read_int_matrix(struct matrix *m)
{
    /*
        This function is basically hardcoded to read a 10x10 matrix from a file named "matrix1.txt".
        The file should contain 100 float values arranged in a 10x10 format.
    */

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

int Valid_Input(char *prompt, int min, int max)
{
    /*
        Defensive programming function to validate user input. 
        It prompts the user with the provided message and checks 
        if the input is an integer within the specified range (min to max). 
        If the input is invalid, it displays an error message and prompts 
        the user again until a valid input is received.
    */

    int value, valid; // Condition to end while loop and store user input

    do
    {
        /*  
        Using do-while loop to ensure the user is prompted at least once and 
        continues until valid input is received
        */

        printf("%s", prompt);
        valid = scanf("%d", &value);
        if (valid != 1)
        {
            printf("Error: please enter a number!\n");
            while (getchar() != '\n');
        }
        else if (value < min || value > max)
        {
            printf("Error: must be between %d and %d!\n", min, max);
        }
    } while (valid != 1 || value < min || value > max);
    return value; // Return the valid input value to where ever the function is called
}

void get_user_input(struct matrix *m)
{
    /*
        This function prompts the user to input the number of rows and columns 
        they want to use from the 10x10 matrix. It uses the Valid_Input function 
        to ensure that the input is valid and within the specified range (1 to MAX_SIZE).
    */
    m->nrows = Valid_Input("Enter the number of rows (1-10): ", 1, MAX_SIZE);
    m->ncols = Valid_Input("Enter the number of columns (1-10): ", 1, MAX_SIZE);

}

float Cal_determinant(struct matrix m, int rows, int cols) 
// pass by value for a struct
{
    // calculate the determinant of a 2x2 matrix starting at (rows, cols)

    float a = m.mvalues[rows][cols]; // (rows, cols) is the top-left 
    float b = m.mvalues[rows][cols + 1]; // (rows, cols + 1) is the top-right 
    float c = m.mvalues[rows + 1][cols]; // (rows + 1, cols) is the bottom-left 
    float d = m.mvalues[rows + 1][cols + 1]; // (rows + 1, cols + 1) is the bottom-right 

    /*  Matrix layout:

        [ a  b ]
        [ c  d ]

        determinant = (a * d) - (b * c)
    */

    return (a * d) - (b * c);
}

float Cal_inverse(struct matrix *m, int rows, int cols)
{
    // calculate the inverse of a 2x2 matrix starting at (rows, cols)

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



int main()
{

    int running = 1; // End loop condition for the main program loop, set to 1 to start the loop

    while (running)
    {

        struct matrix matrixs; // Declare a variable of type struct
        read_int_matrix(&matrixs);
        get_user_input(&matrixs);

        // Print the selected matrix values based on user input
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

        // Prompt user to input the starting row and column for the 2x2 submatrix
        int start_row, start_col;
        start_row = Valid_Input("Enter start row: ", 0, 3);
        start_col = Valid_Input("Enter start col: ", 0, 5);
        // Validate the starting row and column input to ensure they are within the bounds of the selected matrix
        if (start_row < 0 || start_row > 3 || start_col < 0 || start_col > 5)
        {
            printf("Error: Row and column numbers must be between 0 and 5.\n");
        }
        else
        {
            // Calculate and display the determinant of the selected 2x2 submatrix
            float det = Cal_determinant(matrixs, start_row, start_col);
            printf("Determinant of the 2x2 matrix starting at (%d, %d) is: %.2f \n", start_row, start_col, det);
            for (int i = start_row; i < start_row + 2; i++)
            {
                // display the selected 2x2 submatrix values for clarity
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
                    // Calculate and display the inverse of the selected 2x2 submatrix
                    Cal_inverse(&matrixs, start_row, start_col);
                }
        }

        // wait until user is decides to continue or not
        running = Valid_Input("Would you like to continue? (0=no, 1=yes): ", 0, 1);
    }

    return 0;
}