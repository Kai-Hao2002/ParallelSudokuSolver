//include/slover/DLXSolver.hpp
#pragma once

#include "SolverBase.hpp"
#include "core/Sudoku.hpp"

class DLXSolver : public SolverBase {
public:
    DLXSolver();
    bool solve(Sudoku& sudoku) override;
    explicit DLXSolver(int numThreads);
    std::unique_ptr<SolverBase> clone() const override {
        return std::make_unique<DLXSolver>(*this);
    }    

private:
    struct Node {
        Node* L; Node* R; Node* U; Node* D;
        Node* C; // column header
        int rowID;
        int colID;
        int nodeCount;
    };

    struct ColumnNode : Node {
        int size; // number of nodes in column
        const char* name;
    };

    ColumnNode* header;
    int nRows;
    int nCols;
    std::vector<Node> nodes;
    std::vector<ColumnNode> columnNodes;

    std::vector<Node*> solution;

    void buildExactCoverMatrix(const Sudoku& sudoku);
    void cover(ColumnNode* c);
    void uncover(ColumnNode* c);
    bool search(int k);
    int sudokuToIndex(int row, int col, int num) const;
};
