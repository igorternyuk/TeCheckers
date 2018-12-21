#include "alphabeta.hpp"
#include "game.hpp"
#include <vector>
#include <cmath>
#include <iostream>

AlphaBeta::AlphaBeta(int depth):
    searchDepth_(depth)
{}

Board::Move AlphaBeta::getBestMove(Board board)
{
    Board::Move move;
    int alpha = -1000000;
    int beta = +1000000;
    auto computer = Game::getInstance()->getAiPlayer();
    std::vector<Board::Move> lolm;
    board.calcLegalMoves(computer, lolm);
    int currVal = 0;
    Board::Move bestMove;
    auto human = Game::getInstance()->getHumanPlayer();

    for(auto it = lolm.begin(); it != lolm.end(); ++it)
    {
        board.makeMove(*it);
        if(human == Board::Alliance::RED)
        {
            currVal = min(board, this->searchDepth_, alpha, beta);
            if(currVal > alpha)
            {
                alpha = currVal;
                bestMove = *it;
                if(board.isEndGameScenario())
                {
                    //board.undoLastMove();
                    break;
                }
            }
        }
        else if(human == Board::Alliance::BLUE)
        {
            currVal = max(board, this->searchDepth_, alpha, beta);
            if(currVal < beta)
            {
                beta = currVal;
                bestMove = *it;
                if(board.isEndGameScenario())
                {
                    //board.undoLastMove();
                    break;
                }
            }
        }
        //board.undoLastMove();
    }
    std::cout << "boardEvaluated_ = " << boardEvaluated_ << std::endl;
    std::cout << "cutoffsProduced_ = " << cutoffsProduced_ << std::endl;
    return bestMove;
}

int AlphaBeta::max(Board board, int depth, int alpha, int beta)
{
    if(depth == 0 || board.isEndGameScenario())
    {
        ++boardEvaluated_;
        return board.score();
    }
    std::vector<Board::Move> lolm; // list of legal moves
    board.calcLegalMoves(Board::Alliance::RED, lolm);
    for(auto it = lolm.begin(); it != lolm.end(); ++it)
    {
        board.makeMove(*it);
        alpha = std::max(alpha, min(board, calcQuiescenceDepth(board, depth), alpha, beta));
        if(alpha > beta)
        {
            ++cutoffsProduced_;
            break;
        }
        //board.undoLastMove();
    }
    return alpha;
}

int AlphaBeta::min(Board board, int depth, int alpha, int beta)
{
    if(depth == 0 || board.isEndGameScenario())
    {
        ++boardEvaluated_;
        return board.score();
    }
    std::vector<Board::Move> lolm; // list of legal moves
    board.calcLegalMoves(Board::Alliance::BLUE, lolm);
    for(auto it = lolm.begin(); it != lolm.end(); ++it)
    {
        board.makeMove(*it);
        beta = std::min(beta, max(board, calcQuiescenceDepth(board, depth), alpha, beta));
        if(beta < alpha)
        {
            ++cutoffsProduced_;
            break;
        }
        //board.undoLastMove();
    }
    return beta;
}

int AlphaBeta::calcQuiescenceDepth(Board board, int depth)
{
    return depth - 1;
}

/*
     private int calculateQuiescenceDepth(final MoveTransition moveTransition, int depth) {
        if (depth == 1 && this.quiescenceCount < MAX_QUIESCENCE) {
            int activityMeasure = 0;
            if (moveTransition.getTransitedBoard().getCurrentPlayer().isUnderCheck()) {
                activityMeasure += 2;
            }
            final Collection<Move> history = BoardUtils.getMoveHistory(moveTransition.getTransitedBoard(), 4);

            for (final Move move : history) {
                if (move.isCapturingMove()) {
                    ++activityMeasure;
                }
            }

            if (activityMeasure > 3) {
                ++this.quiescenceCount;
                return 2;
            }
        }
        return depth - 1;
    }
*/
