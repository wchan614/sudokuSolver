#include "Sudoku.h"
#include <unordered_map>
#include <utility>
#include <vector>
#include <set>
#include <cassert>
#include <limits>
#include <algorithm>


using namespace std;
// Constructor.
Sudoku::Sudoku(int** grid)
        : numVarAssign {0}
        , grid {grid}
        , unassignedSet {}
        , fcTable {}
{};

// Check if we're done solving.
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
    for (auto &possibleAssign : nextVarDomain)
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

vector<int> Sudoku::getNextValue(pair<int,int> pos) 
{
    vector<int> domainForPos = this->fcTable.at(pos);
    vector<int> scores {};

    for (auto & val : domainForPos)
    {
        int score = tryUpdateTable(pos,val);
        scores.push_back(score);
    }

    vector<int> orderedDomain {};
    while (!scores.empty())
    {
        int where = min_element(scores.begin(), scores.end()) - scores.begin();
        orderedDomain.push_back(domainForPos.at(where));

        scores.erase(scores.begin()+where);
        domainForPos.erase(domainForPos.begin()+where);        
    }

    return domainForPos;
}

void Sudoku::updateCell(pair<int,int> pos, int value)
{
    this->grid[pos.first][pos.second] = value;
}

pair<int,int> Sudoku::getNextMCVar() // return (x,y) position
{
    // Applying most constraint variable
    vector<pair<int,int>> constraintCanidates {};
    int lowestScore = INT_MAX;
    for (const auto pos : this->unassignedSet) 
    {
        if ((this->fcTable.at(pos)).size() < lowestScore)
        {
            constraintCanidates.clear();
            constraintCanidates.push_back(pos);
            lowestScore = (this->fcTable.at(pos)).size();
        }
        else if ((this->fcTable.at(pos)).size() == lowestScore)
        {
            constraintCanidates.push_back(pos);
        }

        if (constraintCanidates.size() == 1)
        {
            this->unassignedSet.erase(constraintCanidates.at(0));
            return constraintCanidates.at(0);
        }
    }

    // Applying most constraining variable
    vector<pair<int,int>> constrainingCanidates {};
    int highestConstraint = 0;
    for (auto pos : constrainingCanidates) 
    {
        // Count 0's or constraints
        // -3 because we are overcounting the pos 3 times : row col section
        int constraintCount = -3;

        // count row
        for (int c = 0; c < 9; c++)
        {
           this->grid[pos.first][c] == 0 ? constraintCount++ : NULL ;
        }

        // count column
        for (int r = 0; r < 9; r++)
        {
           this->grid[r][pos.second] == 0 ? constraintCount++ : NULL ;
        }

        // count section
        int dx = 3*(pos.first/3), dy = 3*(pos.second/3);
        for (int r = dx; r < dx+3; r++) 
        {
            for (int c = dy; c < dy+3; c++) 
            {
                this->grid[r][c] == 0 ? constraintCount++ : NULL ;
            }
        }

        if (constraintCount > highestConstraint) 
        {
            constrainingCanidates.clear();
            constrainingCanidates.push_back(pos);
            highestConstraint = constraintCount;
        }
        else if (constraintCount == highestConstraint) 
        {
            constrainingCanidates.push_back(pos);
        }

        
        pair<int,int> pick = constrainingCanidates.at(17%constrainingCanidates.size());
        this->unassignedSet.erase(pick);
        return pick;
    }
}