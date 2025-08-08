// src/solver/ParallelDLXSolver.cpp
#include "solver/ParallelDLXSolver.hpp"
#include <iostream>
#include <atomic>
#include <array>

ParallelDLXSolver::ParallelDLXSolver(int numThreads)
    : numThreads_(numThreads > 0 ? numThreads : std::thread::hardware_concurrency()) {
    // Debug prints out the number of execution threads
    static bool printed = false;
    if (!printed) {
        std::cout << "ParallelDLXSolver using " << numThreads << " threads .\n";
        printed = true;
    }

}

std::unique_ptr<SolverBase> ParallelDLXSolver::clone() const {
    return std::make_unique<ParallelDLXSolver>(numThreads_);
}

bool ParallelDLXSolver::solve(Sudoku& sudoku) {
    buildExactCoverMatrix(sudoku);
    //std::cout << "[Debug] Matrix built. Starting DLX search..." << std::endl;

    bool success = parallelSearch(0);

    if (success) {
        //std::cout << "[Debug] Solution found." << std::endl;
        extractSolutionToSudoku(sudoku);
    } else {
        //std::cout << "[Debug] No solution found." << std::endl;
    }
    return success;
}



void ParallelDLXSolver::buildExactCoverMatrix(const Sudoku& sudoku) {
    N = sudoku.getSize();            // 9, 16, 25 ...
    boxSize = static_cast<int>(std::sqrt(N));
    int N2 = N * N;
    nCols = 4 * N2;

    header = new ColumnNode("header");

    columnNodes.resize(nCols);
    for (int i = 0; i < nCols; ++i) {
        columnNodes[i] = ColumnNode("C" + std::to_string(i));
        columnNodes[i].up = columnNodes[i].down = &columnNodes[i];
        columnNodes[i].column = &columnNodes[i];
    }

    header->right = &columnNodes[0];
    header->left = &columnNodes[nCols - 1];
    columnNodes[0].left = header;
    columnNodes[nCols - 1].right = header;

    for (int i = 1; i < nCols; ++i) {
        columnNodes[i].left = &columnNodes[i - 1];
        columnNodes[i - 1].right = &columnNodes[i];
    }

    auto addRow = [&](int row, int col, int num) {
        int base = (row * N + col) * N + num;
        std::array<int, 4> cols = {
            row * N + num,                                 // Row constraint
            N2 + col * N + num,                            // Column constraint
            2 * N2 + (row / boxSize * boxSize + col / boxSize) * N + num, // Block constraint
            3 * N2 + row * N + col                         // Cell constraint
        };

        std::vector<Node*> rowNodes(4);
        for (int i = 0; i < 4; ++i) {
            int cIdx = cols[i];
            ColumnNode* colNode = &columnNodes[cIdx];

            nodes.emplace_back();  // Using deque will not cause the indicator to become invalid
            Node* node = &nodes.back();
            rowNodes[i] = node;

            node->column = colNode;
            node->rowID = base;

            // Vertical concatenation
            node->down = colNode;
            node->up = colNode->up;
            colNode->up->down = node;
            colNode->up = node;

            colNode->size++;
        }

        // horizontal concatenation
        for (int i = 0; i < 4; ++i) {
            rowNodes[i]->left = rowNodes[(i + 3) % 4];
            rowNodes[i]->right = rowNodes[(i + 1) % 4];
        }

        rowInfos.push_back({row, col, num});
    };

    for (int row = 0; row < N; ++row) {
        for (int col = 0; col < N; ++col) {
            int val = sudoku.getValue(row, col);
            if (val != 0) {
                addRow(row, col, val - 1);
            } else {
                for (int num = 0; num < N; ++num) {
                    addRow(row, col, num);
                }
            }
        }
    }
    //std::cout << "[Debug] Total ColumnNodes: " << columnNodes.size() << "\n";
    //std::cout << "[Debug] Total Data Nodes: " << nodes.size() << "\n";
    //std::cout << "[Debug] Total Matrix Rows (rowInfos): " << rowInfos.size() << "\n";
}



void ParallelDLXSolver::cover(ColumnNode* c) {
    //std::cout << "[Debug] Cover column: " << c->name << std::endl;
    c->right->left = c->left;
    c->left->right = c->right;
    for (Node* i = c->down; i != c; i = i->down) {
        for (Node* j = i->right; j != i; j = j->right) {
            j->down->up = j->up;
            j->up->down = j->down;
            j->column->size--;
        }
    }
}

void ParallelDLXSolver::uncover(ColumnNode* c) {
    for (Node* i = c->up; i != c; i = i->up) {
        for (Node* j = i->left; j != i; j = j->left) {
            j->column->size++;
            j->down->up = j;
            j->up->down = j;
        }
    }
    c->right->left = c;
    c->left->right = c;
}

bool ParallelDLXSolver::search(int k, std::vector<Node*>& localSolution) {
    //std::cout << "[Debug] Entering search with depth = " << k << "\n";

    if (header->right == header) return true;

    ColumnNode* c = nullptr;
    int minSize = INT32_MAX;

    // Choose column with fewest 1s (heuristic)
    for (ColumnNode* j = static_cast<ColumnNode*>(header->right); j != header; j = static_cast<ColumnNode*>(j->right)) {
        if (j->size < minSize) {
            minSize = j->size;
            c = j;
        }
    }

    if (!c) return false;

    cover(c);

    for (Node* r = c->down; r != c; r = r->down) {
        localSolution.push_back(r);

        // Cover all columns in row r, and record them for uncovering later
        std::vector<ColumnNode*> coveredCols;
        for (Node* j = r->right; j != r; j = j->right) {
            cover(j->column);
            coveredCols.push_back(j->column);
        }

        if (search(k + 1, localSolution)) {
            uncover(c);
            return true;
        }

        // Uncover in reverse order
        for (auto it = coveredCols.rbegin(); it != coveredCols.rend(); ++it) {
            uncover(*it);
        }

        localSolution.pop_back();
    }

    uncover(c);
    return false;
}



void ParallelDLXSolver::extractSolutionToSudoku(Sudoku& sudoku) {
    //std::cout << "[Debug] Extracting solution. Size: " << solutionRows.size() << std::endl;
    for (const auto& info : solutionRows) {
        sudoku.setValue(info.row, info.col, info.num + 1);
    }
}


int ParallelDLXSolver::sudokuToIndex(int row, int col, int num) const {
    return (row * N + col) * N + num;
}

bool ParallelDLXSolver::parallelSearch(int k) {
    //std::cout << "[Debug] Entering parallelSearch(k=" << k << ")" << std::endl;

    // whether it has answer
    std::atomic<bool> found{false};
    tf::Executor executor(numThreads_);
    tf::Taskflow taskflow;

    // Check if all columns are covered, indicating that the solution has been found
    if (header->right == header) {
        //std::cout << "[Debug] All columns covered. Solution found!" << std::endl;
        return true;
    }

    // Select columns with at least 1 node
    ColumnNode* col = nullptr;
    int minSize = INT32_MAX;
    for (ColumnNode* c = static_cast<ColumnNode*>(header->right); c != header; c = static_cast<ColumnNode*>(c->right)) {
        //std::cout << "[Debug] Checking column " << c->name << " with size " << c->size << std::endl;
        if (c->size < minSize) {
            minSize = c->size;
            col = c;
        }
    }

    if (col == nullptr) {
        //std::cout << "[Debug] No column selected. Exiting." << std::endl;
        return false;
    }
    //std::cout << "[Debug] Selected column with minSize=" << minSize << " (" << col->name << ")" << std::endl;

    if (col->down == col) {
        //std::cout << "[Debug] No rows under selected column. Dead end." << std::endl;
        return false;
    }
    
    // First collect all row nodes of the column
    std::vector<Node*> candidateRows;
    int countRows = 0;
    for (Node* row = col->down; row != col; row = row->down) {
        //std::cout << "[Debug] Found row node at ptr " << row << ", rowID = " << row->rowID << std::endl;
        //std::cout << "[Debug] Found row node with rowID = " << row->rowID << std::endl;
        candidateRows.push_back(row);
        countRows++;
    }
    //std::cout << "[Debug] Total rows under selected column: " << countRows << std::endl;

    // cover then column
    cover(col);
    //std::cout << "[Debug] Column " << col->name << " covered." << std::endl;

    // Creating parallel tasks
    int task_id = 0;
    for (Node* row : candidateRows) {
        int rowID = row->rowID;
        //std::cout << "[Debug] Creating Task " << task_id++ << " for rowID: " << rowID << std::endl;
        taskflow.emplace([this, rowID, &found]() {
            if (found.load()) return;

            ParallelDLXSolver localSolver;
            localSolver.cloneFrom(*this);

            std::vector<Node*> localSolution;
            if (localSolver.searchFromRowID(rowID, localSolution)) {
                found.store(true);
                std::lock_guard<std::mutex> lock(solutionMutex);
                solutionRows.clear();
                for (Node* n : localSolution) {
                    int idx = n->rowID;
                    int row = idx / (N * N);
                    int col = (idx % (N * N)) / N;
                    int num = idx % N;
                    solutionRows.push_back({row, col, num});
                }
            }
        });
    }

    //std::cout << "[Debug] All tasks added. Running executor..." << std::endl;

    executor.run(taskflow).wait();

    uncover(col);
    //std::cout << "[Debug] Column " << col->name << " uncovered." << std::endl;

    //std::cout << "[Debug] Exiting parallelSearch(k=" << k << "), found: " << found.load() << std::endl;
    return found.load();
}

void ParallelDLXSolver::cloneFrom(const ParallelDLXSolver& other) {
    N = other.N;                 
    boxSize = other.boxSize;     
    nCols = other.nCols;
    nRows = other.nRows;
    rowInfos = other.rowInfos;

    columnNodes.clear();
    nodes.clear();
    columnNodes.resize(nCols);

    // Initialize ColumnNodes. The name can be omitted if not necessary.
    for (int i = 0; i < nCols; ++i) {
        columnNodes[i] = ColumnNode(); 
        columnNodes[i].up = columnNodes[i].down = &columnNodes[i];
        columnNodes[i].column = &columnNodes[i];
    }

    // Connect columnNodes into a horizontal ring chain
    header = new ColumnNode();
    header->left = &columnNodes[nCols - 1];
    header->right = &columnNodes[0];
    columnNodes[0].left = header;
    columnNodes[nCols - 1].right = header;
    for (int i = 1; i < nCols; ++i) {
        columnNodes[i].left = &columnNodes[i - 1];
        columnNodes[i - 1].right = &columnNodes[i];
    }

    // Add Row node
    for (const auto& info : rowInfos) {
        const int base = sudokuToIndex(info.row, info.col, info.num);
        const int N2 = N * N;
        const std::array<int, 4> cols = {
            info.row * N + info.num,                                    // row constraint
            N2 + info.col * N + info.num,                               // col constraint
            2 * N2 + (info.row / boxSize * boxSize + info.col / boxSize) * N + info.num, // box constraint
            3 * N2 + info.row * N + info.col                            // cell constraint
        };

        std::array<Node*, 4> rowNodes;
        for (int i = 0; i < 4; ++i) {
            ColumnNode* colNode = &columnNodes[cols[i]];
            nodes.emplace_back();
            Node* node = &nodes.back();
            rowNodes[i] = node;

            node->column = colNode;
            node->rowID = base;

           // Vertical concatenation
            node->down = colNode;
            node->up = colNode->up;
            colNode->up->down = node;
            colNode->up = node;

            colNode->size++;
        }

        // Horizontally concatenate four nodes
        for (int i = 0; i < 4; ++i) {
            rowNodes[i]->left = rowNodes[(i + 3) % 4];
            rowNodes[i]->right = rowNodes[(i + 1) % 4];
        }
    }
}

bool ParallelDLXSolver::searchFromRowID(int rowID, std::vector<Node*>& outSolution) {
    // Find the Node* corresponding to row
    Node* targetRow = nullptr;
    for (Node& node : nodes) {
        if (node.rowID == rowID) {
            targetRow = &node;
            break;
        }
    }

    if (!targetRow) return false;

    outSolution.push_back(targetRow);

    // First cover the column of the row
    ColumnNode* mainCol = targetRow->column;
    cover(mainCol);

    // Cover all related columns of this column
    std::vector<ColumnNode*> covered;
    for (Node* j = targetRow->right; j != targetRow; j = j->right) {
        cover(j->column);
        covered.push_back(j->column);
    }

    // Enter the next level recursively
    if (search(1, outSolution)) {
        return true;
    }

    // If unsuccessful, return status
    for (auto it = covered.rbegin(); it != covered.rend(); ++it) {
        uncover(*it);
    }
    uncover(mainCol);
    outSolution.pop_back();
    return false;
}
