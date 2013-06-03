#include "chess.hpp"

#include <utility>

struct ApplyMoveVisitor : public boost::static_visitor<>
{
    Board* board;

    ApplyMoveVisitor(Board& b) : board(&b) {}

    void operator()(SimpleMove& m) const
    {
        board->move(m.from, m.to);
    }

    void operator()(Castle& m) const
    {
        switch (m.dir) {
        case KINGSIDE:
            board->move(m.king, {m.king.row, 6});
            board->move({m.king.row, 7}, {m.king.row, 5});
            break;
        case QUEENSIDE:
            board->move(m.king, {m.king.row, 2});
            board->move({m.king.row, 0}, {m.king.row, 3});
            break;
        }
    }

    void operator()(Promotion& m) const
    {
        board->remove(m.from);
        board->put({m.color, QUEEN}, m.to);
    }
};

bool operator <(Square s1, Square s2)
{
    return s1.row == s2.row ? s1.col < s2.col : s1.row < s2.row;
}

bool operator ==(Square s1, Square s2)
{
    return s1.row == s2.row && s1.col == s2.col;
}

bool Board::has_moved(Square s) const
{
    return initial_squares.find(s) == initial_squares.end();
}

boost::optional<ColoredPiece> Board::piece_at(Square square) const
{
    auto it = white_pieces.find(square);
    if (it != white_pieces.end()){
        return ColoredPiece{WHITE, it->second};
    }
    it = black_pieces.find(square);
    if (it != black_pieces.end()) {
        return ColoredPiece{BLACK, it->second};
    }
    return boost::none_t();
}

Square Board::king_pos(Color c) const
{
    auto& player_pieces = c == WHITE ? white_pieces : black_pieces;
    return std::find_if(
            player_pieces.begin(),
            player_pieces.end(),
            [](std::pair<Square, Piece> p) {return p.second == KING;})
        ->first;
}

bool Board::any_piece(Color c, std::function<bool(Square, Piece)> f) const
{
    auto& pcs = c == WHITE ? white_pieces : black_pieces;
    return std::any_of(
            pcs.begin(),
            pcs.end(),
            [f](std::pair<Square, Piece> p) {return f(p.first, p.second);});
}

void Board::move(Square from, Square to)
{
    ColoredPiece cp = *piece_at(from);
    switch (cp.color) {
    case WHITE:
        white_pieces.erase(from);
        white_pieces[to] = cp.piece;
        break;
    case BLACK:
        black_pieces.erase(from);
        black_pieces[to] = cp.piece;
        break;
    }
    initial_squares.erase(from);
}

void Board::put(ColoredPiece cp, Square s)
{
    switch (cp.color) {
    case WHITE:
        white_pieces[s] = cp.piece;
        break;
    case BLACK:
        black_pieces[s] = cp.piece;
        break;
    }
    initial_squares.insert(s);
}

void Board::remove(Square from)
{
    ColoredPiece cp = *piece_at(from);
    switch (cp.color) {
    case WHITE:
        white_pieces.erase(from);
        break;
    case BLACK:
        black_pieces.erase(from);
        break;
    }
    initial_squares.erase(from);
}

Board initial_position()
{
    static const Piece first_row[8] =
        {ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK};

    Board b;
    for (int i = 0; i < 8; ++i) {
        b.put({WHITE, first_row[i]}, {7, i});
        b.put({BLACK, first_row[i]}, {0, i});
        b.put({WHITE, PAWN}, {6, i});
        b.put({BLACK, PAWN}, {1, i});
    }
    return b;
}

boost::optional<MoveResult> try_move(Board& b, Color as, Square from, Square to)
{
    auto maybe_move = move(b, as, from, to);
    if (!maybe_move) {
        return boost::none_t();
    }

    apply(b, *maybe_move);

    Color next_player = as == WHITE ? BLACK : WHITE;
    MoveResult mr;
    mr.move = *maybe_move;
    mr.gave_check = in_check(b, next_player);
    mr.opponent_cannot_move = !can_move(b, next_player);
    return mr;
}

void apply(Board& board, Move move)
{
    if (move.hit) {
        board.remove(*move.hit);
    }
    boost::apply_visitor(ApplyMoveVisitor(board), move.movement);
}

boost::optional<Move> move(const Board& b, Color as, Square from, Square to)
{
    auto maybe_move = move_maybe_to_check(b, as, from, to);
    if (!maybe_move) {
        return maybe_move;
    }

    Move move = *maybe_move;
    Board next_board = b;
    apply(next_board, move);
    if (in_check(next_board, as)) {
        return boost::none_t();
    }

    return maybe_move;
}

bool in_check(const Board& b, Color c)
{
    Square player_king = b.king_pos(c);
    Color opp = c == WHITE ? BLACK : WHITE;
    return b.any_piece(
            opp,
            [&](Square s, Piece p) {
                return move_maybe_to_check(b, opp, s, player_king);
            });
}

bool can_move(const Board& b, Color c)
{
    return b.any_piece(
            c,
            [&](Square s, Piece p) {
                auto moves = possible_moves({c, p}, s);
                for (Square to : moves) {
                    auto m = move(b, c, s, to);
                    if (m) {
                        return true;
                    }
                }
                return false;
            });
}

std::vector<Square> possible_moves(ColoredPiece cp, Square pos)
{
    std::vector<Square> diffs;
    switch (cp.piece) {
    case PAWN:
        if (cp.color == WHITE) {
            diffs = {{-1, 0}, {-2, 0}, {-1, -1}, {-1, 1}};
        } else {
            diffs = {{1, 0}, {2, 0}, {1, -1}, {1, 1}};
        }
        break;
    case ROOK:
        for (int i = -7; i <= 7; ++i) {
            diffs.push_back({i, 0});
            diffs.push_back({0, i});
        }
        break;
    case KNIGHT:
        diffs = {{2, 1}, {-2, 1}, {2, -1}, {-2, -1},
                 {1, 2}, {-1, 2}, {1, -2}, {-1, -2}};
        break;
    case BISHOP:
        for (int i = -7; i <= 7; ++i) {
            diffs.push_back({i, i});
            diffs.push_back({i, -i});
        }
        break;
    case QUEEN:
        for (int i = -7; i <= 7; ++i) {
            diffs.push_back({i, 0});
            diffs.push_back({0, i});
            diffs.push_back({i, i});
            diffs.push_back({i, -i});
        }
        break;
    case KING:
        diffs = {{-1, -1}, {-1, 0}, {-1, 1},
                 {0, -1}, {0, 1},
                 {1, -1}, {1, 0}, {1, 1},
                 {0, -2}, {0, 2}};
        break;
    }
    for (Square& s : diffs) {
        s.row += pos.row;
        s.col += pos.col;
    }
    diffs.erase(
        std::remove_if(
            diffs.begin(),
            diffs.end(),
            [](Square s) {return s.row < 0 || s.col < 0 ||
                                 s.row >= 8 || s.col >= 8;}),
        diffs.end());
    return diffs;
}

// warning: long ugly monolithic function :(
boost::optional<Move> move_maybe_to_check(const Board& b, Color as, Square from,
                                          Square to)
{
    auto maybe_piece = b.piece_at(from);
    if (from == to || !maybe_piece || maybe_piece->color != as) {
        return boost::none_t();
    }

    Piece piece = maybe_piece->piece;
    Square diff = {to.row - from.row, to.col - from.col};
    boost::optional<Move> result;
    bool l;
    switch (piece) {
    case PAWN:
        if (as == WHITE) {
            boost::optional<Move> res;
            if (diff == Square{-1, 0} && !b.piece_at(to)) {
                res = Move{SimpleMove{from, to}, boost::none_t()};
            } else if (diff == Square{-2, 0} && !b.piece_at(to) &&
                       !b.piece_at(Square{from.row - 1, from.col}) &&
                       !b.has_moved(from)) {
                res = Move{SimpleMove{from, to}, boost::none_t()};
            } else if ((diff == Square{-1, -1} || diff == Square{-1, 1}) &&
                       b.piece_at(to) && b.piece_at(to)->color != as) {
                res = Move{SimpleMove{from, to}, to};
            }
            if (res && to.row == 0) {
                res->movement = Promotion{from, to, WHITE};
            }
            return res;
        } else {
            boost::optional<Move> res;
            if (diff == Square{1, 0} && !b.piece_at(to)) {
                res = Move{SimpleMove{from, to}, boost::none_t()};
            } else if (diff == Square{2, 0} && !b.piece_at(to) &&
                       !b.piece_at(Square{from.row + 1, from.col}) &&
                       !b.has_moved(from)) {
                res = Move{SimpleMove{from, to}, boost::none_t()};
            } else if ((diff == Square{1, -1} || diff == Square{1, 1}) &&
                       b.piece_at(to) && b.piece_at(to)->color != as) {
                res = Move{SimpleMove{from, to}, to};
            }
            if (res && to.row == 7) {
                res->movement = Promotion{from, to, BLACK};
            }
            return res;
        }
        break;
    case ROOK:
        if (diff.row == 0 || diff.col == 0) {
            l = false;
            if (diff.row == 0) {
                for (int i = std::min(from.col, to.col) + 1;
                     i <= std::max(from.col, to.col) - 1 && !l;
                     ++i) {
                    l = b.piece_at({from.row, i});
                }
            } else {
                for (int i = std::min(from.row, to.row) + 1;
                     i <= std::max(from.row, to.row) - 1 && !l;
                     ++i) {
                    l = b.piece_at({i, from.col});
                }
            }
            if (!l) {
                if (!b.piece_at(to)) {
                    return Move{SimpleMove{from, to}, boost::none_t()};
                } else if (b.piece_at(to)->color != as) {
                    return Move{SimpleMove{from, to}, to};
                }
            }
        }
        break;
    case KNIGHT:
        if (diff == Square{1, 2} || diff == Square{2, 1} ||
            diff == Square{-1, 2} || diff == Square{2, -1} ||
            diff == Square{1, -2} || diff == Square{-2, 1} ||
            diff == Square{-1, -2} || diff == Square{-2, -1}) {
            if (!b.piece_at(to)) {
                return Move{SimpleMove{from, to}, boost::none_t()};
            } else if (b.piece_at(to)->color != as) {
                return Move{SimpleMove{from, to}, to};
            }
        }
        break;
    case BISHOP:
        if (diff.row == diff.col || diff.row == -diff.col) {
            l = false;
            if (diff.row == diff.col) {
                for (int d = std::min(diff.row, 0) + 1;
                     d <= std::max(0, diff.row) - 1 && !l;
                     ++d) {
                    l = b.piece_at({from.row + d, from.col + d});
                }
            } else {
                for (int d = std::min(diff.row, 0) + 1;
                     d <= std::max(0, diff.row) - 1 && !l;
                     ++d) {
                    l = b.piece_at({from.row + d, from.col - d});
                }
            }
            if (!l) {
                if (!b.piece_at(to)) {
                    return Move{SimpleMove{from, to}, boost::none_t()};
                } else if (b.piece_at(to)->color != as) {
                    return Move{SimpleMove{from, to}, to};
                }
            }
        }
        break;
    case QUEEN: // TODO factor out
        if (diff.row == 0 || diff.col == 0) {
            l = false;
            if (diff.row == 0) {
                for (int i = std::min(from.col, to.col) + 1;
                     i <= std::max(from.col, to.col) - 1 && !l;
                     ++i) {
                    l = b.piece_at({from.row, i});
                }
            } else {
                for (int i = std::min(from.row, to.row) + 1;
                     i <= std::max(from.row, to.row) - 1 && !l;
                     ++i) {
                    l = b.piece_at({i, from.col});
                }
            }
            if (!l) {
                if (!b.piece_at(to)) {
                    return Move{SimpleMove{from, to}, boost::none_t()};
                } else if (b.piece_at(to)->color != as) {
                    return Move{SimpleMove{from, to}, to};
                }
            }
        } else if (diff.row == diff.col || diff.row == -diff.col) {
            l = false;
            if (diff.row == diff.col) {
                for (int d = std::min(diff.row, 0) + 1;
                     d <= std::max(0, diff.row) - 1 && !l;
                     ++d) {
                    l = b.piece_at({from.row + d, from.col + d});
                }
            } else {
                for (int d = std::min(diff.row, 0) + 1;
                     d <= std::max(0, diff.row) - 1 && !l;
                     ++d) {
                    l = b.piece_at({from.row + d, from.col - d});
                }
            }
            if (!l) {
                if (!b.piece_at(to)) {
                    return Move{SimpleMove{from, to}, boost::none_t()};
                } else if (b.piece_at(to)->color != as) {
                    return Move{SimpleMove{from, to}, to};
                }
            }
        }
        break;
    case KING:
        if (diff == Square{-1, -1} || diff == Square{-1, 0} ||
            diff == Square{-1, 1} || diff == Square{0, -1} ||
            diff == Square{0, 1} || diff == Square{1, -1} ||
            diff == Square{1, 0} || diff == Square{1, 1}) {
            if (!b.piece_at(to)) {
                return Move{SimpleMove{from, to}, boost::none_t()};
            } else if (b.piece_at(to)->color != as) {
                return Move{SimpleMove{from, to}, to};
            }
        } else if ((diff == Square{0, -2} || diff == Square{0, 2}) &&
                    !b.has_moved(from)) {
            if (diff.col == 2) {
                if (!b.piece_at({from.row, from.col + 1}) &&
                    !b.piece_at({from.row, from.col + 2}) &&
                    !b.has_moved({from.row, from.col + 3})) {
                    Board b1 = b;
                    b1.move(from, {from.row, from.col + 1});
                    if (!in_check(b1, as)) {
                        return Move{Castle{from, KINGSIDE}};
                    }
                }
            } else {
                if (!b.piece_at({from.row, from.col - 1}) &&
                    !b.piece_at({from.row, from.col - 2}) &&
                    !b.piece_at({from.row, from.col - 3}) &&
                    !b.has_moved({from.row, from.col - 4})) {
                    Board b1 = b;
                    b1.move(from, {from.row, from.col - 1});
                    if (!in_check(b1, as)) {
                        return Move{Castle{from, QUEENSIDE}};
                    }
                }
            }

        }
        break;
    }
    return boost::none_t();
}
