#include <iostream>
#include <fstream>
#include <string>


using namespace std;


// Create a 2D array intrepretation of the text and pass it to Sudoku Class.
int main()
{
    ifstream sudokuReadFile("data.sd");

    if (sudokuReadFile.good()) 
    {
        int intNum = 0;

        int grid[9][9];
        int x = 0, y = 0;
        while (sudokuReadFile >> intNum)
        {
            grid[x][y] = intNum;
            (y == 8) ? y = 0 , x++ : (y++); 
        }
        Sudoku sudoku = new Sudoku(grid);
        sudoku.solve();
    }
    else 
    {
        return 0xdeadbeef;
    }
}