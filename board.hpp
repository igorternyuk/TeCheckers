#pragma once

#include <vector>
#include <map>
#include <string>
#include <cmath>
#include "gamestatus.hpp"

class Board
{
public:
    explicit Board();
    enum
    {
        BOARD_SIZE = 8,
        NUM_CHECKER_ROW_FOR_ONE_SIDE = 3,
        CHECKER_SCORE = 100,
        KING_SCORE = 300
    };

    enum class Alliance
    {
        RED,
        BLUE,
        NONE
    };

    struct Piece
    {
        int x, y;
        Alliance alliance;
        bool isKing;
        int value;
        Piece(int x = -1, int y = -1, Alliance alliance = Alliance::NONE, bool isKing = false):
            x(x), y(y), alliance(alliance), isKing(isKing)
        {
            this->value = isKing ? KING_SCORE : CHECKER_SCORE;
        }

        void crown()
        {
            isKing = true;
            this->value = KING_SCORE;
        }

        bool operator==(const Piece &piece)
        {
            return this->x == piece.x && this->y == piece.y
                    && this->isKing == piece.isKing && this->alliance == piece.alliance;
        }
    };

    struct Tile
    {
        int x, y;
        Piece piece;

        Tile(int x = -1, int y = -1):
            x(x), y(y)
        {}

        bool isEmpty() const
        {
            return piece.alliance == Alliance::NONE;
        }

        bool hasPiece() const
        {
            return !isEmpty();
        }

        bool isLight() const
        {
            return (x + y) % 2 == 0;
        }

        bool isDark() const
        {
            return !isLight();
        }

        void setPiece(Piece piece)
        {
            this->piece = piece;
            this->piece.x = x;
            this->piece.y = y;
        }

        void removePiece()
        {
           this->piece = Piece();
        }

        int distanceTo(const Tile& other)
        {
            return abs(this->x - other.x) + abs(this->y - other.y);
        }

        bool operator==(const Tile &other) {
            return this->x == other.x && this->y == other.y
                    && this->piece == other.piece;
        }

    };

    static Tile NULL_TILE;

    struct Step
    {
        Tile start, end;
        Piece captured;
        bool IsCoronation = false;
        Step(Tile start, Tile end, Piece captured = Piece()):
            start(start), end(end), captured(captured)
        {}

        bool isJump() const
        {
            return captured.x != -1 && captured.y != -1;
        }

        bool operator==(const Step &other) {
            return this->start == other.start && this->end == other.end
                    && this->captured == other.captured;
        }

        bool operator!=(const Step &other) {
            return !(*this == other);
        }

    };

    using Move = std::vector<Step>;

    void Reset();
    bool isValidTile(int x, int y) const;
    bool isTileEmpty(int x, int y) const;
    Tile getTile(int x, int y) const;
    bool setPiece(int x, int y, Alliance alliance, bool isKing = false);
    bool makeMove(Move move);
    void undoLastMove();
    void calcLegalMoves(Alliance alliance, std::vector<Move> &moves) const;
    void calcAllJumps(Piece piece, Move move, std::vector<Move> &legalMoves) const;
    int score() const;
    std::string toString();
    std::string tileToAlgebraicNotation(const Tile& tile);
    std::string moveToAlgebraicNotation(const Move& move);
    const std::vector<Move>& getMoveLog() const;
    GameStatus GetGameStatus() const;
    bool isEndGameScenario() const;
    std::string CalcHash() const;
    int GetTotalPieces() const;

private:
    void clearBoard();
    bool isFriendlyCell(Tile cell, Alliance alliance) const;
    bool checkCrown(const Piece &piece) const;
    void CalcPieceCount(int& count_red_pieces, int& count_red_kings, int& count_blue_pieces, int& count_blue_kings) const;

private:
    Tile _grid[BOARD_SIZE][BOARD_SIZE];
    std::vector<Move> _moveLog;
    int count23 = 0; // 2-3 piece ending limit 5 moves
    int count45 = 0; // 4-5 piece ending limit 30 moves
    int count67 = 0; // 6-7 piece ending limit 60 moves
    int count15 = 0;
    int oldBalance = 0;
    std::string _hash;
    std::map<std::string, int> _mapRep;
};
