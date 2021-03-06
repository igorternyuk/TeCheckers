#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>
#include <map>
#include <string>
#include <cmath>

class Board
{
public:
    explicit Board();
    enum
    {
        BOARD_SIZE = 8,
        NUM_CHECKER_ROW_FOR_ONE_SIDE = 3,
        CHECKER_SCORE = 100,
        KING_SCORE = 200
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

    void setupInitialPosition();
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
    bool isEndGameScenario() const;

private:
    Tile grid_[BOARD_SIZE][BOARD_SIZE];
    const int offsetX_[4] { +1, +1, -1, -1 };
    const int offsetY_[4] { -1, +1, +1, -1 };
    std::vector<Move> moveLog_;
    std::map<int, std::string> algebraicNotaionFileMap;

    void clearBoard();
    bool isFriendlyCell(Tile cell, Alliance alliance) const;
    bool checkCrown(const Piece &piece) const;    
};

#endif // BOARD_HPP
