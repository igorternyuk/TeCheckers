#ifndef ALPHABETA_HPP
#define ALPHABETA_HPP

#include "board.hpp"
#include <vector>

class AlphaBeta
{
public:
    explicit AlphaBeta(int depth = 13);
    Board::Move getBestMove(Board board);
private:
    int searchDepth_;
    int boardEvaluated_;
    int cutoffsProduced_;
    int max(Board board, int depth, int alpha, int beta);
    int min(Board board, int depth, int alpha, int beta);
    bool isSubset(std::vector<Board::Step> &first, std::vector<Board::Step> &second);
};

#endif // ALPHABETA_HPP
