//src/solver/DLXSolver.cpp
#include <climits>
#include "solver/DLXSolver.hpp"
#include <vector>
#include <cstring>
#include <iostream>

DLXSolver::DLXSolver()
    : header(nullptr), nRows(0), nCols(0) {}

int DLXSolver::sudokuToIndex(int row, int col, int num) const {
    // return index 0~728 for 9x9 sudoku
    return (row * 9 + col) * 9 + (num - 1);
}

void DLXSolver::buildExactCoverMatrix(const Sudoku& sudoku) {
    /*
    9x9 sudoku Exact Cover problem:
    Rows = 9*9*9 = 729 (row,col,num)
    Columns = 4*81 = 324 constraints:
      - Cell constraint: each cell must be filled exactly once (81)
      - Row constraint: each number appears once per row (81)
      - Column constraint: each number appears once per column (81)
      - Block constraint: each number appears once per block (81)
    */
    nRows = 9*9*9;
    nCols = 324;

    columnNodes.resize(nCols);
    nodes.clear();

    // Initialize header node
    header = new ColumnNode();
    header->L = header->R = header->U = header->D = header;
    header->size = 0;
    header->name = "header";

    // Setup columns circular linked list
    ColumnNode* prev = header;
    for (int i = 0; i < nCols; ++i) {
        columnNodes[i].size = 0;
        columnNodes[i].name = nullptr;
        columnNodes[i].C = &columnNodes[i];
        columnNodes[i].U = &columnNodes[i];
        columnNodes[i].D = &columnNodes[i];
        columnNodes[i].L = prev;
        columnNodes[i].R = header;
        prev->R = &columnNodes[i];
        header->L = &columnNodes[i];
        prev = &columnNodes[i];
    }

    // Build nodes vector reserve
    nodes.reserve(nRows * 4); // each row has 4 constraints

    // Helper lambdas for constraints
    auto cellConstraint = [](int r, int c) { return r*9 + c; };
    auto rowConstraint = [](int r, int num) { return 81 + r*9 + num - 1; };
    auto colConstraint = [](int c, int num) { return 162 + c*9 + num - 1; };
    auto blockConstraint = [](int r, int c, int num) {
        int block = (r/3)*3 + (c/3);
        return 243 + block*9 + num - 1;
    };

    // Create nodes for each row
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            for (int num = 1; num <= 9; ++num) {
                int rowIndex = sudokuToIndex(r, c, num);

                // Check if cell fixed
                int cellVal = sudoku.getValue(r, c);
                if (cellVal != 0 && cellVal != num) continue;

                // create 4 nodes for this row
                int cols[4] = {
                    cellConstraint(r,c),
                    rowConstraint(r,num),
                    colConstraint(c,num),
                    blockConstraint(r,c,num)
                };

                Node* rowNodes[4];
                for (int i = 0; i < 4; ++i) {
                    nodes.push_back(Node());
                    Node* node = &nodes.back();
                    node->rowID = rowIndex;
                    node->colID = cols[i];
                    node->C = &columnNodes[cols[i]];
                    node->U = columnNodes[cols[i]].U;
                    node->D = &columnNodes[cols[i]];
                    columnNodes[cols[i]].U->D = node;
                    columnNodes[cols[i]].U = node;
                    columnNodes[cols[i]].size++;
                    rowNodes[i] = node;
                }

                // Link rowNodes circularly
                for (int i = 0; i < 4; ++i) {
                    rowNodes[i]->L = rowNodes[(i + 3) % 4];
                    rowNodes[i]->R = rowNodes[(i + 1) % 4];
                }
            }
        }
    }
}

void DLXSolver::cover(ColumnNode* c) {
    c->R->L = c->L;
    c->L->R = c->R;
    for (Node* i = c->D; i != c; i = i->D) {
        for (Node* j = i->R; j != i; j = j->R) {
            j->D->U = j->U;
            j->U->D = j->D;
            static_cast<ColumnNode*>(j->C)->size--;  // 型別轉換修正
        }
    }
}

void DLXSolver::uncover(ColumnNode* c) {
    for (Node* i = c->U; i != c; i = i->U) {
        for (Node* j = i->L; j != i; j = j->L) {
            static_cast<ColumnNode*>(j->C)->size++;  // 型別轉換修正
            j->D->U = j;
            j->U->D = j;
        }
    }
    c->R->L = c;
    c->L->R = c;
}

bool DLXSolver::search(int k) {
    if (header->R == header) return true;

    ColumnNode* c = nullptr;
    int minSize = INT_MAX;

    for (ColumnNode* j = static_cast<ColumnNode*>(header->R); j != header; j = static_cast<ColumnNode*>(j->R)) {
        if (j->size < minSize) {
            minSize = j->size;
            c = j;
        }
    }

    if (c == nullptr || c->size == 0) return false;

    cover(c);

    for (Node* r = c->D; r != c; r = r->D) {
        solution.push_back(r);

        for (Node* j = r->R; j != r; j = j->R) {
            cover(static_cast<ColumnNode*>(j->C));
        }

        if (search(k + 1)) return true;

        for (Node* j = r->L; j != r; j = j->L) {
            uncover(static_cast<ColumnNode*>(j->C));
        }
        solution.pop_back();
    }

    uncover(c);
    return false;
}


bool DLXSolver::solve(Sudoku& sudoku) {
    buildExactCoverMatrix(sudoku);
    solution.clear();

    bool solved = search(0);
    if (!solved) return false;

    // 解析 solution 回填 sudoku
    for (auto node : solution) {
        int rowID = node->rowID;
        int num = rowID % 9 + 1;
        rowID /= 9;
        int col = rowID % 9;
        int row = rowID / 9;

        sudoku.setValue(row, col, num);
    }

    delete header; // free header node
    header = nullptr;

    return true;
}
DLXSolver::DLXSolver(int numThreads) {
    std::cout << "DLXSolver using " << numThreads << " threads .\n";
    // 你可以在這裡儲存 numThreads 或設置執行緒池等等
}