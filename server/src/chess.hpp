#ifndef CHESS_HPP
#define CHESS_HPP

#include <map>
#include <set>
#include <vector>
#include <boost/optional.hpp>
#include <boost/variant.hpp>

enum Piece
{
    KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN
};

enum Color
{
    WHITE, BLACK
};

enum CastleDir
{
    KINGSIDE, QUEENSIDE
};

struct ColoredPiece;
struct Square;
struct SimpleMove;
struct Castle;
struct Promotion;
struct Move;
struct MoveResult;
struct Movement;
class Board;

struct ColoredPiece
{
    Color color;
    Piece piece;
};

struct Square
{
    int row, col;
};

bool operator <(Square, Square);
bool operator ==(Square, Square);

struct SimpleMove
{
    Square from, to;
};

struct Castle
{
    Square king;
    CastleDir dir;
};

struct Promotion
{
    Square from, to;
    Color color;
};

struct Move
{
    boost::variant<SimpleMove, Castle, Promotion> movement;
    boost::optional<Square> hit;
};

struct MoveResult
{
    Move move;
    bool gave_check;
    bool opponent_cannot_move;
};

class Board
{
public:
    bool has_moved(Square) const;
    boost::optional<ColoredPiece> piece_at(Square) const;
    Square king_pos(Color) const;

    bool any_piece(Color, std::function<bool(Square, Piece)>) const;

    void move(Square from, Square to);
    void put(ColoredPiece, Square s);
    void remove(Square from);
private:
    std::map<Square, Piece> white_pieces, black_pieces;
    std::set<Square> initial_squares;
};

Board initial_position();

boost::optional<MoveResult> try_move(Board&, Color as, Square from, Square to);

void apply(Board&, Move);
boost::optional<Move> move(const Board&, Color as, Square from, Square to);

bool in_check(const Board&, Color);
bool can_move(const Board&, Color);
std::vector<Square> possible_moves(ColoredPiece, Square);
boost::optional<Move> move_maybe_to_check(const Board&, Color as, Square from,
                                          Square to);

#endif
