#pragma once

#include "board.hpp"
#include <vector>

class AlphaBeta
{
public:
    explicit AlphaBeta(int depth = 8);
    Board::Move getBestMove(Board board);
private:
    struct Evaluation
    {
        int score;
        std::string best_line;
        std::string eval_board;
    };
    int searchDepth_;
    int boardEvaluated_;
    int cutoffsProduced_;
    Evaluation max(Board board, int depth, int alpha, int beta, std::string curr_line);
    Evaluation min(Board board, int depth, int alpha, int beta, std::string curr_line);
    bool isSubset(std::vector<Board::Step> &first, std::vector<Board::Step> &second);
    int calculateQuiescenceDepth(const Board& board);
};
