#include "alphabeta.hpp"
#include "game.hpp"
#include <vector>
#include <cmath>
#include <iostream>

AlphaBeta::AlphaBeta(int depth):
    searchDepth_(depth)
{

}

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
                    board.undoLastMove();
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
                    board.undoLastMove();
                    break;
                }
            }
        }
        board.undoLastMove();
    }
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

int AlphaBeta::calcQuiescenceDepth(Board &board, int depth)
{
    return depth - 1;
}





/*
 *
 * @Override
    public Move execute(final Board board) {
        final long startTime = System.currentTimeMillis();
        final Player currentPlayer = board.getCurrentPlayer();
        Move bestMove = Move.MoveFactory.NULL_MOVE;
        int highestSeenValue = Integer.MIN_VALUE;
        int lowestSeenValue = Integer.MAX_VALUE;
        int currentValue;
        int moveCounter = 0;
        final Collection<Move> legalMoves = MoveSorter.SMART.sort((board.getCurrentPlayer().getLegalMoves()));
        final int numMoves = legalMoves.size();
        System.out.println(board.getCurrentPlayer() + " THINKING with depth = " + this.searchDepth);
        String s = "";
        for (final Move move : legalMoves) {
            final long candidateMoveStartTime = System.nanoTime();
            ++moveCounter;
            final MoveTransition moveTransition = board.getCurrentPlayer().makeMove(move);
            this.quiescenceCount = 0;
            if (moveTransition.getMoveStatus().isDone()) {
                //final long candidateMoveStartTime = System.nanoTime();
                currentValue = currentPlayer.getAlliance().isWhite() ?
                        min(moveTransition.getTransitedBoard(), this.searchDepth - 1, highestSeenValue, lowestSeenValue) :
                        max(moveTransition.getTransitedBoard(), this.searchDepth - 1, highestSeenValue, lowestSeenValue);
                if (currentPlayer.getAlliance().isWhite() && currentValue > highestSeenValue) {
                    highestSeenValue = currentValue;
                    bestMove = move;
                    if (moveTransition.getTransitedBoard().getBlackPlayer().isCheckMate()) {
                        break;
                    }
                } else if (currentPlayer.getAlliance().isBlack() && currentValue < lowestSeenValue) {
                    lowestSeenValue = currentValue;
                    bestMove = move;
                    if (moveTransition.getTransitedBoard().getWhitePlayer().isCheckMate()) {
                        break;
                    }
                }

                final String quiescenceInfo = " " + score(currentPlayer, highestSeenValue, lowestSeenValue) + " q: " + this.quiescenceCount;
                s = "\t" + toString() + "(" + this.searchDepth + "), m: (" + moveCounter + "/" + numMoves + ") " + move + ", best:  " + bestMove

                        + quiescenceInfo + ", t: " + calculateTimeTaken(candidateMoveStartTime, System.nanoTime());
            }
            System.out.println(s);
        }

        System.out.println("Board evaluated = " + this.boardsEvaluated);
        System.out.println("this.cutsOffProduced = " + this.cutsOffProduced);
        System.out.println("Best move = " + bestMove);
        System.out.println("Move time = " + (System.currentTimeMillis() - startTime) / 1000 + " seconds");
        return bestMove;
    }



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
