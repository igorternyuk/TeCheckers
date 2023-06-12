#include "alphabeta.hpp"
#include "game.hpp"
#include "utils.hpp"
#include <cmath>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <sstream>

constexpr int INF = 10000000;

AlphaBeta::AlphaBeta(int depth):
    searchDepth_(depth)
{}

Board::Move AlphaBeta::getBestMove(Board board)
{
    Board::Move move;
    auto computer = Game::getInstance()->getAiPlayer();
    std::vector<Board::Move> lolm;
    board.calcLegalMoves(computer, lolm);

    int k = 1;
    auto it = std::remove_if(lolm.begin(), lolm.end(), [&](auto &move){
        for(int i = k; i < lolm.size(); ++i)
        {
            if(isSubset(move, lolm[i]))
            {
                ++k;
                return true;
            }
        }
        ++k;
        return false;
    });

    lolm.erase(it, lolm.end());

    if(lolm.size() == 1)
    {
        std::cout << "Only one move available" << std::endl;
        return lolm.at(0);
    }

    Board::Move bestMove;
    auto human = Game::getInstance()->getHumanPlayer();

    std::cout << "AI starts thinking with depth = " << this->searchDepth_ << std::endl;
    std::cout << "Legal moves count = " << lolm.size() << std::endl;
    auto totalMoveTime = 0u;
    int alpha = -INF;
    int beta = +INF;
    int moveNumber = board.getMoveLog().size() / 2 + 1;
    if(human == Board::Alliance::RED) // CPU is minimizing player
    {
        int minVal = INF;
        for(auto it = lolm.begin(); it != lolm.end(); ++it)
        {
            int number = moveNumber;
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
            board.makeMove(*it); //CPU make the move
            std::string curr_line = board.moveToAlgebraicNotation(*it);
            Evaluation currVal = max(board, calculateQuiescenceDepth(board), alpha, beta, curr_line);
            //beta = std::min(beta, minVal);
            if(currVal.score < minVal)
            {
                bestMove = *it;
                minVal = std::min(currVal.score, minVal);
                if(board.isEndGameScenario())
                    break;
            }

            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            board.undoLastMove();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            totalMoveTime += elapsed;
            std::cout << "Analyzed move " + board.moveToAlgebraicNotation(*it)
                      << " Score: " << currVal.score << " Time taken: "
                      << elapsed << "ms" << std::endl;
            std::cout << "Best line:\n";
            std::vector<std::string> v_best_line = Utils::split(currVal.best_line, " ");

            for(int i = 0; i < v_best_line.size(); ++i)
            {
                if(i % 2 == 0)
                {
                    std::cout << (i == 0 ? "... " : " ");
                }
                else
                {
                    ++number;
                    std::cout << " " << number << ". ";
                }
                std::cout << v_best_line[i];
            }

            std::cout << "\n";
            std::cout << currVal.eval_board << "\n";
        }
    }
    /*else if(human == Board::Alliance::BLUE)
    {
        int currVal = INF;
        for(auto it = lolm.begin(); it != lolm.end(); ++it)
        {
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
            board.makeMove(*it);
            currVal = max(board, this->searchDepth_, alpha, beta);
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

            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            //board.undoLastMove();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            totalMoveTime += elapsed;
            std::cout << "Analyzed move " + board.moveToAlgebraicNotation(*it)
                      << " Score: " << currVal << " Time taken: "
                      << elapsed << "ms" << std::endl;
        }
    }*/

    std::cout << "boardEvaluated_ = " << boardEvaluated_ << std::endl;
    std::cout << "cutoffsProduced_ = " << cutoffsProduced_ << std::endl;
    std::cout << "Best move  = " + board.moveToAlgebraicNotation(bestMove) << std::endl;
    std::cout << "Total move time = " << totalMoveTime << "ms" << std::endl;

    return bestMove;
}

AlphaBeta::Evaluation AlphaBeta::max(Board board, int depth, int alpha, int beta, std::string curr_line)
{
    if(depth == 0 || board.isEndGameScenario())
    {
        ++boardEvaluated_;
        int score = board.score();
        Evaluation eval;
        eval.score = score;
        eval.best_line = curr_line;
        eval.eval_board = board.toString();
        return eval;
    }
    int value = -INF;
    std::vector<Board::Move> lolm; // list of legal moves
    board.calcLegalMoves(Board::Alliance::RED, lolm);
    std::string curr_board = board.toString();
    //std::cout << curr_board << std::endl;
    Evaluation evalBest;
    for(auto it = lolm.begin(); it != lolm.end(); ++it)
    {
        board.makeMove(*it);
        std::stringstream ss;
        ss << curr_line << std::string(" ") << board.moveToAlgebraicNotation(*it);
        std::string line = ss.str();
        Evaluation evaluation = min(board, depth - 1, alpha, beta, line);
        value = std::max(value, evaluation.score);
        if(value == evaluation.score)
        {
            //best_line = evaluation.best_line;
            evalBest = evaluation;
        }

        alpha = std::max(alpha, value);
        if(value >= beta)
        {
            ++cutoffsProduced_;
            break;
        }
        board.undoLastMove();
    }
    return evalBest;
}

AlphaBeta::Evaluation AlphaBeta::min(Board board, int depth, int alpha, int beta, std::string curr_line)
{
    if(depth == 0 || board.isEndGameScenario())
    {
        ++boardEvaluated_;
        int score = board.score();
        Evaluation eval;
        eval.score = score;
        eval.best_line = curr_line;
        eval.eval_board = board.toString();
        return eval;
    }

    int value = INF;
    std::vector<Board::Move> lolm; // list of legal moves
    board.calcLegalMoves(Board::Alliance::BLUE, lolm);
    std::string curr_board = board.toString();
    //std::cout << curr_board << std::endl;
    Evaluation evalBest;
    for(auto it = lolm.begin(); it != lolm.end(); ++it)
    {
        board.makeMove(*it);
        std::stringstream ss;
        ss << curr_line << std::string(" ") << board.moveToAlgebraicNotation(*it);
        std::string line = ss.str();
        Evaluation evaluation = max(board, depth - 1, alpha, beta, line);
        value = std::min(value, evaluation.score);
        if(value == evaluation.score)
        {
            //best_line = evaluation.best_line;
            evalBest = evaluation;
        }
        beta = std::min(beta, value);
        if(value <= alpha)
        {
            ++cutoffsProduced_;
            break;
        }
        board.undoLastMove();
    }
    return evalBest;
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

int AlphaBeta::calculateQuiescenceDepth(const Board &board)
{
    int total = board.GetTotalPieces();
    int depth = this->searchDepth_;
    if(total <= 8)
        depth = std::min(8, depth);
    if(total <= 6)
        depth = std::min(6, depth);
    if(total <= 5)
        depth = std::min(4, depth);
    if(total <= 3)
        depth = std::min(2, depth);
    return depth;
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
