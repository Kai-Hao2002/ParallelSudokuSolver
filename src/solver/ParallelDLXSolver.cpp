// src/solver/ParallelDLXSolver.cpp
#include "solver/ParallelDLXSolver.hpp"
#include <iostream>
#include <atomic>
#include <array>

ParallelDLXSolver::ParallelDLXSolver(int numThreads)
    : numThreads_(numThreads), header(nullptr), nRows(0), nCols(0) {}

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
    constexpr int N = 9;
    constexpr int N2 = N * N;
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
        int base = (row * 9 + col) * 9 + num;
        std::array<int, 4> cols = {
            row * 9 + num,                                 // Row constraint
            81 + col * 9 + num,                            // Column constraint
            162 + (row / 3 * 3 + col / 3) * 9 + num,      // Block constraint
            243 + row * 9 + col                            // Cell constraint
        };

        std::vector<Node*> rowNodes(4);
        for (int i = 0; i < 4; ++i) {
            int cIdx = cols[i];
            ColumnNode* colNode = &columnNodes[cIdx];

            nodes.emplace_back();  // 使用 deque 不會導致指標失效
            Node* node = &nodes.back();
            rowNodes[i] = node;

            node->column = colNode;
            node->rowID = base;

            // 垂直串接
            node->down = colNode;
            node->up = colNode->up;
            colNode->up->down = node;
            colNode->up = node;

            colNode->size++;
        }

        // 水平串接
        for (int i = 0; i < 4; ++i) {
            rowNodes[i]->left = rowNodes[(i + 3) % 4];
            rowNodes[i]->right = rowNodes[(i + 1) % 4];
        }

        rowInfos.push_back({row, col, num});
    };

    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            int val = sudoku.getValue(row, col);
            if (val != 0) {
                addRow(row, col, val - 1);
            } else {
                for (int num = 0; num < 9; ++num) {
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
    return (row * 9 + col) * 9 + num;
}
bool ParallelDLXSolver::parallelSearch(int k) {
    //std::cout << "[Debug] Entering parallelSearch(k=" << k << ")" << std::endl;

    // 解是否找到
    std::atomic<bool> found{false};
    tf::Executor executor(numThreads_);
    tf::Taskflow taskflow;

    // ✅ 檢查是否所有列都被 cover，代表已找到解
    if (header->right == header) {
        //std::cout << "[Debug] All columns covered. Solution found!" << std::endl;
        return true;
    }

    // 選擇最少 1 節點的列
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
    
    // 先蒐集該列的所有 row 節點
    std::vector<Node*> candidateRows;
    int countRows = 0;
    for (Node* row = col->down; row != col; row = row->down) {
        //std::cout << "[Debug] Found row node at ptr " << row << ", rowID = " << row->rowID << std::endl;
        //std::cout << "[Debug] Found row node with rowID = " << row->rowID << std::endl;
        candidateRows.push_back(row);
        countRows++;
    }
    //std::cout << "[Debug] Total rows under selected column: " << countRows << std::endl;

    // 再 cover 該 column
    cover(col);
    //std::cout << "[Debug] Column " << col->name << " covered." << std::endl;

    // 建立平行任務
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
                    int row = idx / 81;
                    int col = (idx % 81) / 9;
                    int num = idx % 9;
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
    nCols = other.nCols;
    nRows = other.nRows;
    rowInfos = other.rowInfos;

    columnNodes.clear();
    nodes.clear();

    columnNodes.resize(nCols);
    for (int i = 0; i < nCols; ++i) {
        columnNodes[i] = ColumnNode("C" + std::to_string(i));
        columnNodes[i].up = columnNodes[i].down = &columnNodes[i];
        columnNodes[i].column = &columnNodes[i];
    }

    header = new ColumnNode("header");
    header->right = &columnNodes[0];
    header->left = &columnNodes[nCols - 1];
    columnNodes[0].left = header;
    columnNodes[nCols - 1].right = header;
    for (int i = 1; i < nCols; ++i) {
        columnNodes[i].left = &columnNodes[i - 1];
        columnNodes[i - 1].right = &columnNodes[i];
    }

    auto addRow = [&](const RowInfo& info) {
        int base = sudokuToIndex(info.row, info.col, info.num);
        std::array<int, 4> cols = {
            info.row * 9 + info.num,
            81 + info.col * 9 + info.num,
            162 + (info.row / 3 * 3 + info.col / 3) * 9 + info.num,
            243 + info.row * 9 + info.col
        };

        std::vector<Node*> rowNodes(4);
        for (int i = 0; i < 4; ++i) {
            int cIdx = cols[i];
            ColumnNode* colNode = &columnNodes[cIdx];

            nodes.emplace_back();
            Node* node = &nodes.back();
            rowNodes[i] = node;

            node->column = colNode;
            node->rowID = base;

            // 垂直串接
            node->down = colNode;
            node->up = colNode->up;
            colNode->up->down = node;
            colNode->up = node;

            colNode->size++;
        }

        for (int i = 0; i < 4; ++i) {
            rowNodes[i]->left = rowNodes[(i + 3) % 4];
            rowNodes[i]->right = rowNodes[(i + 1) % 4];
        }
    };

    for (const auto& info : rowInfos) {
        addRow(info);
    }
}
bool ParallelDLXSolver::searchFromRowID(int rowID, std::vector<Node*>& outSolution) {
    // 找到對應 row 的 Node*
    Node* targetRow = nullptr;
    for (Node& node : nodes) {
        if (node.rowID == rowID) {
            targetRow = &node;
            break;
        }
    }

    if (!targetRow) return false;

    outSolution.push_back(targetRow);

    // ✅ 先 cover 該 row 的 column
    ColumnNode* mainCol = targetRow->column;
    cover(mainCol);

    // ✅ cover 該列所有相關 column
    std::vector<ColumnNode*> covered;
    for (Node* j = targetRow->right; j != targetRow; j = j->right) {
        cover(j->column);
        covered.push_back(j->column);
    }

    // 進入下一層遞迴
    if (search(1, outSolution)) {
        return true;
    }

    // 若沒成功，回復狀態
    for (auto it = covered.rbegin(); it != covered.rend(); ++it) {
        uncover(*it);
    }
    uncover(mainCol);
    outSolution.pop_back();
    return false;
}
