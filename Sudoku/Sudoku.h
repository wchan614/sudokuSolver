#ifndef SUDOKU_H
#define SUDOKU_H

#include <vector>
#include <unordered_map>
#include <utility>
#include <set>


class Sudoku
{
public:
    int numVarAssign;
    int ** grid; // [9][9]
    std::set<std::pair<int,int>>  unassignedSet;
    std::unordered_map<std::pair<int,int>, std::vector<int>> fcTable;
 
    Sudoku(int** grid);
    void solve();
    void display();
private:
    // need set variable
    // need to do unassigned, unassignedSet, unassignedMap, fcTable

    void constructTable();
    bool isLegalTable();
    int tryUpdateTable(std::pair<int,int> pos, int value);
    void updateTable(std::pair<int,int> pos, int value);

    pair<int,int> getNextMCVar();
    vector<int> getNextValue(std::pair<int,int> pos);

    bool canPlace(std::pair<int,int> pos, int value);
    void updateCell(std::pair<int,int> pos, int value);
    bool doneSolving();

    bool backtrackSearch();
    void revert(std::unordered_map<std::pair<int,int>, std::vector<int>> table, pair<int,int> pos);

};

#endif