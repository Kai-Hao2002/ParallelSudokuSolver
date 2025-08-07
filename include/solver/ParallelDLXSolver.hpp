//include/solver/ParallelDLXSolver.hpp
// include/solver/ParallelDLXSolver.hpp
#pragma once

#include "SolverBase.hpp"
#include "core/Sudoku.hpp"
#include <mutex>
#include <vector>
#include <deque>       // change to use deque
#include <memory>
#include <string>
#include <taskflow/taskflow.hpp>

class ParallelDLXSolver : public SolverBase {
public:
    explicit ParallelDLXSolver(int numThreads = 2);
    bool solve(Sudoku& sudoku) override;
    std::unique_ptr<SolverBase> clone() const override;

private:
    int numThreads_;
    std::mutex solutionMutex;

    struct ColumnNode;

    struct Node {
        Node* left;
        Node* right;
        Node* up;
        Node* down;
        ColumnNode* column;
        int rowID;

        Node() : left(this), right(this), up(this), down(this), column(nullptr), rowID(-1) {}
    };

    struct ColumnNode : public Node {
        int size;
        std::string name;

        ColumnNode() : size(0), name(""), Node() {
            column = this;
        }

        ColumnNode(const std::string& n) : size(0), name(n) {
            column = this;
        }
    };

    ColumnNode* header;
    int nRows;
    int nCols;

    std::deque<Node> nodes;               // deque<Node>
    std::vector<ColumnNode> columnNodes;

    struct RowInfo {
        int row, col, num;
    };
    std::vector<RowInfo> rowInfos;
    std::vector<RowInfo> solutionRows;

    void buildExactCoverMatrix(const Sudoku& sudoku);
    void cover(ColumnNode* c);
    void uncover(ColumnNode* c);
    bool search(int k, std::vector<Node*>& currentSolution);
    bool parallelSearch(int k);

    void extractSolutionToSudoku(Sudoku& sudoku);

    int sudokuToIndex(int row, int col, int num) const;
    void cloneFrom(const ParallelDLXSolver& other);
    bool searchFromRowID(int rowID, std::vector<Node*>& outSolution);
};
