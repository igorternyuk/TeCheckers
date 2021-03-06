#include "alphabeta.hpp"
#include "game.hpp"

#include <cmath>
#include <algorithm>
#include <chrono>
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

    auto it = std::remove_if(lolm.begin(), lolm.end(), [&](auto &move){
        for(auto &m: lolm)
        {
            if(isSubset(move, m))
            {
                return true;
            }
        }
        return false;
    });

    lolm.erase(it, lolm.end());

    if(lolm.size() == 1)
    {
        std::cout << "Only one move available" << std::endl;
        return lolm.at(0);
    }

    int currVal = 0;
    Board::Move bestMove;
    auto human = Game::getInstance()->getHumanPlayer();

    std::cout << "AI starts thinking eith depth = " << this->searchDepth_ << std::endl;
    std::cout << "Legal moves count = " << lolm.size() << std::endl;
    auto totalMoveTime = 0u;
    for(auto it = lolm.begin(); it != lolm.end(); ++it)
    {
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
        board.makeMove(*it);
        int currVal = 0;
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
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        //board.undoLastMove();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        totalMoveTime += elapsed;
        std::cout << "Analyzed move " + board.moveToAlgebraicNotation(*it)
                  << " Score: " << currVal << " Time taken: "
                  << elapsed << "ms" << std::endl;
    }
    std::cout << "boardEvaluated_ = " << boardEvaluated_ << std::endl;
    std::cout << "cutoffsProduced_ = " << cutoffsProduced_ << std::endl;
    std::cout << "Total move time = " << totalMoveTime << "ms" << std::endl;
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

bool AlphaBeta::isSubset(std::vector<Board::Step> &first, std::vector<Board::Step> &second)
{
    if(first.size() < second.size())
    {
        const size_t min = first.size();
        for(size_t i = 0; i < min; ++i)
        {
            if(first[i] != second[i])
            {
                return false;
            }
        }
        return true;
    }
    return false;
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
