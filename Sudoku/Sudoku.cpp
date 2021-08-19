#include "Sudoku.h"
#include <unordered_map>
#include <utility>
#include <vector>
#include <set>

using namespace std;
// Constructor.
Sudoku::Sudoku(int** grid)
        : numVarAssign {0}
        , grid {grid}
        , unassignedSet {}
        ,
{
    // fcTable;

};

// Check if we're done solving.
// No zeros = done
bool Sudoku::doneSolving() 
{
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (this->grid[r][c] == 0) 
            {
                return false;
            }
        }
    }
    return true;
}

void Sudoku::revert(unordered_map<pair<int,int>, vector<int>> table, pair<int,int> pos)
{
    this->fcTable = table;
    updateCell(pos, 0);
}

bool Sudoku::backtrackSearch() 

{
    if (doneSolving()) return true;

    pair<int,int> nextVar = getNextMCVar();


    vector<int> nextVarDomain = getNextValue(nextVar);
    unordered_map< pair<int,int>, vector<int> > backupTable = this->fcTable;
    for (auto possibleAssign : nextVarDomain)
    {
        this->numVarAssign++;

        // Try to place a number (from domain) into the grid position
        if (canPlace(nextVar, possibleAssign)) 
        {
            // If succesful, then place the value
            updateCell(nextVar,possibleAssign);
            updateTable(nextVar,possibleAssign);

            // Revert and restore and try a different assignment
            if (!isLegalTable()) 
            {
                revert(backupTable,nextVar);
                continue;
            }

            // Recurse and try other variables
            if (backtrackSearch()) return true;

            // Failed to find any leads with other variables, revert.
            revert(backupTable,nextVar);
        }

        if (this->numVarAssign > 10000) return false;
    } 

    // failed all values in domain, impossible
    
    this->unassignedSet.emplace(nextVar);
    return false;
}

bool Sudoku::canPlace(pair<int,int> pos, int value)
{
    // Check if legal along row of pos on the grid
    for (int c = 0; c < 9; c++)
    {
        if (this->grid[pos.first][c] == value) return false;
    }

    // Check if legal along col of pos on the grid
    for (int r = 0; r < 9; r++)
    {
        if (this->grid[r][pos.second] == value) return false;
    }

    // Check if legal within the subsection
    int dx = 3*(pos.first/3), dy = 3*(pos.second/3);
    for (int r = dx; r < dx+3; r++) 
    {
        for (int c = dy; c < dy+3; c++) 
        {
            if (this->grid[r][c] == value) return false;
        }
    }

    return true;
}