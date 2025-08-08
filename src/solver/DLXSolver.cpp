//src/solver/DLXSolver.cpp
#include <climits>
#include "solver/DLXSolver.hpp"
#include <vector>
#include <cstring>
#include <iostream>
#include <cmath>

DLXSolver::DLXSolver()
    : header(nullptr), nRows(0), nCols(0), size(9), boxSize(3) {}

int DLXSolver::sudokuToIndex(int row, int col, int num) const {
    // num starts from 1, so num-1
    return (row * size + col) * size + (num - 1);
}

void DLXSolver::buildExactCoverMatrix(const Sudoku& sudoku) {
    sudoku_ = sudoku;
    size = sudoku.getSize();
    boxSize = static_cast<int>(std::sqrt(size));

    nRows = size * size * size;    // size^3
    nCols = 4 * size * size;       // 4 * size^2

    columnNodes.clear();
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

    nodes.reserve(nRows * 4);

    // Helper lambdas for constraints
    auto cellConstraint = [this](int r, int c) {
        return r * size + c;
    };
    auto rowConstraint = [this](int r, int num) {
        return size * size + r * size + num - 1;
    };
    auto colConstraint = [this](int c, int num) {
        return 2 * size * size + c * size + num - 1;
    };
    auto blockConstraint = [this](int r, int c, int num) {
        int block = (r / boxSize) * boxSize + (c / boxSize);
        return 3 * size * size + block * size + num - 1;
    };

    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            int cellVal = sudoku.getValue(r, c);
            for (int num = 1; num <= size; ++num) {
                int rowIndex = sudokuToIndex(r, c, num);
                if (cellVal != 0 && cellVal != num) continue;

                int cols[4] = {
                    cellConstraint(r, c),
                    rowConstraint(r, num),
                    colConstraint(c, num),
                    blockConstraint(r, c, num)
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

                // link rowNodes circularly
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
            static_cast<ColumnNode*>(j->C)->size--;  // Type conversion correction
        }
    }
}

void DLXSolver::uncover(ColumnNode* c) {
    for (Node* i = c->U; i != c; i = i->U) {
        for (Node* j = i->L; j != i; j = j->L) {
            static_cast<ColumnNode*>(j->C)->size++;  // Type conversion correction
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

    // parse solution backfill sudoku
    for (auto node : solution) {
        int rowID = node->rowID;

        int num = rowID % size + 1;
        rowID /= size;
        int col = rowID % size;
        int row = rowID / size;

        sudoku.setValue(row, col, num);
    }

    delete header;
    header = nullptr;

    return true;
}
DLXSolver::DLXSolver(int numThreads)
    : DLXSolver() // Call the default constructor to initialize
{
    std::cout << "DLXSolver using " << numThreads << " threads.\n";
}