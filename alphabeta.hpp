#ifndef ALPHABETA_HPP
#define ALPHABETA_HPP

#include "board.hpp"

class AlphaBeta
{
public:
    explicit AlphaBeta(int depth = 6);
    Board::Move getBestMove(Board board);
private:
    int searchDepth_;
    int boardEvaluated_;
    int cutoffsProduced_;
    int max(Board board, int depth, int alpha, int beta);
    int min(Board board, int depth, int alpha, int beta);
    int calcQuiescenceDepth(Board &board, int depth);
};

#endif // ALPHABETA_HPP
