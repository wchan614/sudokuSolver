#include "Sudoku.h"
#include <unordered_map>
#include <utility>
#include <vector>
#include <set>
#include <cassert>
#include <limits>
#include <algorithm>
#include <iterator>


using namespace std;
// Constructor.
Sudoku::Sudoku(int** grid)
        : numVarAssign {0}
        , grid {grid}
        , unassignedSet {}
        , fcTable {}
{
    constructTable();
};

// Check if we're done solving.
void Sudoku::constructTable()
{
    set<pair<int,int>>::iterator it = this->unassignedSet.begin();
    vector<int> domain = {1,2,3,4,5,6,7,8,9};
    // Iterate till the end of set
    while (it != this->unassignedSet.end())
    {
        pair<int,int> pos = *it;
        int dx = pos.first, dy = pos.second;
        for (int r = 0; r < 9 ; r++) 
        {
            auto search = domain.find(this->grid[pos.first][r]);
            if (search != domain.end()) {
                domain.erase(remove(domain.begin(), domain.end(), r), domain.end());
            }
        }

        for (int c = 0; c < 9 ; c++) 
        {
            auto search = domain.find(this->grid[c][pos.second]);
            if (search != domain.end()) {
                domain.erase(remove(domain.begin(), domain.end(), r), domain.end());
            }
        }

        for (int r = 3 * (dx / 3); r < 3 + 3 * (dx / 3) ; r++) 
        {
            for (int c = 3 * (dy / 3); 3 + 3 * (dy / 3); c++) 
            {
                auto search = domain.find(this->grid[r][c]);
                if (search != domain.end()) {
                    domain.erase(remove(domain.begin(), domain.end(), r), domain.end());
                }
            }
        }
        this->fcTable.at(pos) = domain;
    }
}

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

void Sudoku::updateTable(pair<int,int> pos, int value)
{
    pair<int,int> otherPos;
    for(int r = 0; r < 9; r++) 
    {
        otherPos = pair{r, pos.second};
        auto search = this->fcTable.find(otherPos);
        if (r != pos.first && search != this->fcTable.end()) 
        {
            auto &table = this->fcTable.at(otherPos); 
            table.erase(remove(table.begin(), table.end(), value), table.end());
        }
    }
    for(int c = 0; c < 9; c++)
    {
        otherPos = pair{pos.first,c};
        auto search = this->fcTable.find(otherPos);
        if (c != pos.second && search != this->fcTable.end())
        {
            auto &table = this->facTable.at(otherPos);
            table.erase(remove(table.begin(), table.end(), value), table.end());
        }
    }

    int dx = pos.first, dy = pos.second;
    for (int i = 3*(dx/3); i < 3+3*(dx/3); i++)
    {
        for (int j = 3*(dy/3); 3+3*(dy/3); j++)
        {
            pair<int,int> sectionPos = pair{i,j};
            auto search = this->fcTable.find(sectionPos);
            if (pos != sectionPos && search != this->fcTable.end())
            {
                auto &table = this->facTable.at(sectionPos);
                table.erase(remove(table.begin(), table.end(), value), table.end());
            }
        }
    } 
}

void Sudoku::tryUpdateTable(pair<int,int> pos, int value)
{
    int count  = 0;
    for(int r = 0; r < 9; r++) 
    {
        otherPos = pair{r, pos.second};
        auto search = this->fcTable.find(otherPos);
        if (r != pos.first && search != this->fcTable.end()) 
        {
           count++;
        }
    }
    for(int c = 0; c < 9; c++)
    {
        otherPos = pair{pos.first,c};
        auto search = this->fcTable.find(otherPos);
        if (c != pos.second && search != this->fcTable.end())
        {
            count++;
        }
    }

    int dx = pos.first, dy = pos.second;
    for (int i = 3*(dx/3); i < 3+3*(dx/3); i++)
    {
        for (int j = 3*(dy/3); 3+3*(dy/3); j++)
        {
            pair<int,int> sectionPos = pair{i,j};
            auto search = this->fcTable.find(sectionPos);
            if (pos != sectionPos && search != this->fcTable.end())
            {
                count++;
            }
        }
    } 
    return count;
}

bool Sudoku::isLegalTable()
{
    for(std::map<Key,Val>::iterator iter = this->fcTable.begin(); iter != this->fcTable.end(); ++iter)
    {
        if (iter->first == NULL || iter->second == NULL) {return false;}
    }

    return true;
}

