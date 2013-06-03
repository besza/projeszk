#include "chess.hpp"

#include <gtest/gtest.h>

bool operator ==(ColoredPiece a, ColoredPiece b)
{
    return a.color == b.color && a.piece == b.piece;
}

TEST(BoardOperations, MovingPieces)
{
    Board b;
    Square s = {3, 5};
    ASSERT_FALSE(b.piece_at(s));

    ColoredPiece cp = {WHITE, BISHOP};
    b.put(cp, s);
    ASSERT_FALSE(b.has_moved(s));
    ASSERT_EQ(cp.color, b.piece_at(s)->color);
    ASSERT_EQ(cp.piece, b.piece_at(s)->piece);

    Square s2 = {2, 6};
    b.move(s, s2);
    ASSERT_TRUE(b.has_moved(s));
    ASSERT_FALSE(b.piece_at(s));
    ASSERT_EQ(cp.color, b.piece_at(s2)->color);
    ASSERT_EQ(cp.piece, b.piece_at(s2)->piece);

    b.remove(s2);
    ASSERT_FALSE(b.piece_at(s2));
}

TEST(ChessLogic, MovesFromEmptySquares)
{
    Board b = initial_position();
    for (int row = 2; row <= 5; ++row) {
        for (int col = 0; col < 8; ++col) {
            ASSERT_FALSE(move(b, WHITE, {row, col}, {row + 1, col}));
        }
    }
}

TEST(ChessLogic, MovesWithWrongColor)
{
    Board b = initial_position();
    for (int col = 0; col < 8; ++col) {
        ASSERT_FALSE(move(b, WHITE, {1, col}, {2, col}));
    }
    for (int col = 0; col < 8; ++col) {
        ASSERT_FALSE(move(b, BLACK, {6, col}, {5, col}));
    }
}

TEST(ChessLogic, PawnMovement)
{
    Board b = initial_position();
    EXPECT_FALSE(move(b, WHITE, {6, 3}, {3, 3}));

    b.put({BLACK, PAWN}, {5, 3});
    EXPECT_FALSE(move(b, WHITE, {6, 3}, {5, 3}));

    auto m1 = move(b, WHITE, {6, 4}, {5, 3});
    Square s = {5, 3};
    ASSERT_TRUE(m1);
    EXPECT_EQ(s, m1->hit);

    auto m2 = move(b, BLACK, {1, 0}, {3, 0});
    ASSERT_TRUE(m2);
    EXPECT_FALSE(m2->hit);
}

TEST(ChessLogic, RookMovement)
{
    Board b = initial_position();
    EXPECT_FALSE(move(b, BLACK, {0, 0}, {3, 0}));

    b.remove({1, 0});
    auto m1 = move(b, BLACK, {0, 0}, {3, 0});
    ASSERT_TRUE(m1);
    EXPECT_FALSE(m1->hit);

    Square s = {4, 4};
    b.put({WHITE, ROOK}, {4, 0});
    b.put({BLACK, PAWN}, s);
    auto m2 = move(b, WHITE, {4, 0}, s);
    ASSERT_TRUE(m2);
    EXPECT_EQ(s, m2->hit);
}

TEST(ChessLogic, KnightMovement)
{
    Board b = initial_position();
    EXPECT_FALSE(move(b, BLACK, {0, 1}, {1, 3}));

    b.put({WHITE, KNIGHT}, {3, 3});
    EXPECT_TRUE(move(b, WHITE, {3, 3}, {1, 2}));
    EXPECT_TRUE(move(b, WHITE, {3, 3}, {1, 4}));
    EXPECT_TRUE(move(b, WHITE, {3, 3}, {5, 2}));
    EXPECT_TRUE(move(b, WHITE, {3, 3}, {5, 4}));
}

TEST(ChessLogic, BishopMovement)
{
    Board b = initial_position();
    b.put({WHITE, BISHOP}, {3, 2});
    EXPECT_TRUE(move(b, WHITE, {3, 2}, {1, 0}));
    EXPECT_TRUE(move(b, WHITE, {3, 2}, {1, 4}));
    EXPECT_TRUE(move(b, WHITE, {3, 2}, {5, 0}));
    EXPECT_TRUE(move(b, WHITE, {3, 2}, {5, 4}));
    EXPECT_FALSE(move(b, WHITE, {3, 2}, {4, 6}));
    EXPECT_FALSE(move(b, WHITE, {3, 2}, {3, 1}));
}

TEST(ChessLogic, QueenMovement)
{
    Board b = initial_position();
    b.put({WHITE, QUEEN}, {3, 2});

    EXPECT_TRUE(move(b, WHITE, {3, 2}, {1, 0}));
    EXPECT_TRUE(move(b, WHITE, {3, 2}, {1, 4}));
    EXPECT_TRUE(move(b, WHITE, {3, 2}, {5, 0}));
    EXPECT_TRUE(move(b, WHITE, {3, 2}, {5, 4}));
    EXPECT_FALSE(move(b, WHITE, {3, 2}, {4, 6}));
    EXPECT_TRUE(move(b, WHITE, {3, 2}, {3, 1}));

    EXPECT_TRUE(move(b, WHITE, {3, 2}, {1, 2}));
    EXPECT_FALSE(move(b, WHITE, {3, 2}, {0, 2}));
}

TEST(ChessLogic, KingMovement)
{
    Board b;
    b.put({WHITE, KING}, {3, 3});

    EXPECT_TRUE(move(b, WHITE, {3, 3}, {2, 2}));
    EXPECT_TRUE(move(b, WHITE, {3, 3}, {2, 3}));
    EXPECT_FALSE(move(b, WHITE, {3, 3}, {5, 3}));
}

TEST(ChessLogic, Promotion)
{
    Board b;
    b.put({WHITE, PAWN}, {1, 0});
    b.put({BLACK, PAWN}, {6, 3});
    b.put({BLACK, PAWN}, {5, 4});

    auto m1 = move(b, WHITE, {1, 0}, {0, 0});
    ASSERT_TRUE(m1);
    EXPECT_EQ(WHITE, boost::get<Promotion>(m1->movement).color);

    auto m2 = move(b, BLACK, {6, 3}, {7, 3});
    ASSERT_TRUE(m2);
    EXPECT_EQ(BLACK, boost::get<Promotion>(m2->movement).color);

    auto m3 = move(b, BLACK, {5, 4}, {6, 4});
    ASSERT_TRUE(m3);
    EXPECT_ANY_THROW(boost::get<Promotion>(m3->movement));
}

TEST(ChessLogic, Castling)
{
    Board b = initial_position();
    b.remove({0, 1}); b.remove({0, 2}); b.remove({0, 3}); b.remove({0, 5}); b.remove({0, 6});
    b.remove({7, 1}); b.remove({7, 2}); b.remove({7, 3}); b.remove({7, 5}); b.remove({7, 6});

    auto m1 = move(b, WHITE, {7, 4}, {7, 6});
    ASSERT_TRUE(m1);
    EXPECT_EQ(KINGSIDE, boost::get<Castle>(m1->movement).dir);
    auto m2 = move(b, WHITE, {7, 4}, {7, 2});
    ASSERT_TRUE(m2);
    EXPECT_EQ(QUEENSIDE, boost::get<Castle>(m2->movement).dir);
    auto m3 = move(b, BLACK, {0, 4}, {0, 6});
    ASSERT_TRUE(m3);
    EXPECT_EQ(KINGSIDE, boost::get<Castle>(m3->movement).dir);
    auto m4 = move(b, BLACK, {0, 4}, {0, 2});
    ASSERT_TRUE(m4);
    EXPECT_EQ(QUEENSIDE, boost::get<Castle>(m4->movement).dir);

    b.move({0, 4}, {0, 5}); b.move({0, 5}, {0, 4});
    b.move({7, 0}, {7, 1}); b.move({7, 1}, {7, 0});
    b.move({7, 7}, {7, 6}); b.move({7, 6}, {7, 7});

    EXPECT_FALSE(move(b, WHITE, {7, 4}, {7, 6}));
    EXPECT_FALSE(move(b, WHITE, {7, 4}, {7, 2}));
    EXPECT_FALSE(move(b, BLACK, {0, 4}, {0, 6}));
    EXPECT_FALSE(move(b, BLACK, {0, 4}, {0, 2}));
}

TEST(ChessLogic, CantCastleThroughCheck)
{
    Board b = initial_position();
    b.remove({0, 1}); b.remove({0, 2}); b.remove({0, 3}); b.remove({0, 5}); b.remove({0, 6});
    b.remove({7, 1}); b.remove({7, 2}); b.remove({7, 3}); b.remove({7, 5}); b.remove({7, 6});
    b.remove({6, 3}); b.remove({6, 5});
    b.remove({1, 3}); b.remove({1, 5});

    EXPECT_TRUE(move(b, WHITE, {7, 4}, {7, 6}));
    EXPECT_TRUE(move(b, WHITE, {7, 4}, {7, 2}));
    EXPECT_TRUE(move(b, BLACK, {0, 4}, {0, 6}));
    EXPECT_TRUE(move(b, BLACK, {0, 4}, {0, 2}));

    b.put({BLACK, ROOK}, {4, 3});
    b.put({BLACK, ROOK}, {4, 5});
    b.put({WHITE, ROOK}, {3, 3});
    b.put({WHITE, ROOK}, {3, 5});

    EXPECT_FALSE(move(b, WHITE, {7, 4}, {7, 6}));
    EXPECT_FALSE(move(b, WHITE, {7, 4}, {7, 2}));
    EXPECT_FALSE(move(b, BLACK, {0, 4}, {0, 6}));
    EXPECT_FALSE(move(b, BLACK, {0, 4}, {0, 2}));
}

TEST(ChessLogic, Check)
{
    Board b;
    b.put({WHITE, KING}, {3, 3});
    b.put({BLACK, KING}, {0, 0});
    b.put({BLACK, ROOK}, {3, 6});
    b.put({BLACK, QUEEN}, {7, 0});

    EXPECT_TRUE(in_check(b, WHITE));
    EXPECT_FALSE(in_check(b, BLACK));
}

TEST(ChessLogic, CantMoveIntoCheck)
{
    Board b;
    b.put({WHITE, KING}, {2, 3});
    EXPECT_TRUE(move(b, WHITE, {2, 3}, {3, 3}));
    EXPECT_TRUE(move(b, WHITE, {2, 3}, {3, 2}));

    b.put({BLACK, ROOK}, {3, 6});
    EXPECT_FALSE(move(b, WHITE, {2, 3}, {3, 3}));
    EXPECT_FALSE(move(b, WHITE, {2, 3}, {3, 2}));
}

TEST(ChessLogic, NoPossibleMoves)
{
    Board b;
    b.put({BLACK, KING}, {0, 0});
    b.put({WHITE, KING}, {1, 2});
    b.put({BLACK, PAWN}, {0, 2});
    b.put({WHITE, PAWN}, {2, 1});
    EXPECT_FALSE(can_move(b, BLACK));
    EXPECT_TRUE(can_move(b, WHITE));

    b.put({BLACK, PAWN}, {2, 0});
    EXPECT_TRUE(can_move(b, BLACK));
}

TEST(ChessLogic, Stalemate)
{
    Board b;
    b.put({BLACK, KING}, {0, 0});
    b.put({WHITE, KING}, {1, 2});
    b.put({BLACK, PAWN}, {0, 2});
    b.put({WHITE, PAWN}, {3, 1});

    auto mr1 = try_move(b, WHITE, {3, 1}, {2, 1});
    ASSERT_TRUE(mr1);
    ASSERT_FALSE(mr1->gave_check);
    ASSERT_TRUE(mr1->opponent_cannot_move);
}

TEST(ChessLogic, Checkmate)
{
    Board b;
    b.put({BLACK, KING}, {0, 0});
    b.put({WHITE, KING}, {1, 2});
    b.put({BLACK, PAWN}, {0, 2});
    b.put({WHITE, PAWN}, {2, 1});
    b.put({WHITE, ROOK}, {3, 3});

    auto mr1 = try_move(b, WHITE, {3, 3}, {3, 0});
    ASSERT_TRUE(mr1);
    ASSERT_TRUE(mr1->gave_check);
    ASSERT_TRUE(mr1->opponent_cannot_move);
}

TEST(MoveApplication, ApplySimpleMove)
{
    Board b;
    ColoredPiece cp1 = {WHITE, QUEEN}, cp2 = {BLACK, QUEEN};
    b.put(cp1, {1, 2}); b.put(cp2, {1, 4});

    apply(b, Move{SimpleMove{{1, 2}, {1, 3}}, boost::none_t()});
    EXPECT_FALSE(b.piece_at({1, 2}));
    EXPECT_EQ(cp1, b.piece_at({1, 3}));
    ASSERT_EQ(cp2, b.piece_at({1, 4}));

    apply(b, Move{SimpleMove{{1, 4}, {1, 5}}, Square{1, 3}});
    EXPECT_FALSE(b.piece_at({1, 3}));
    EXPECT_FALSE(b.piece_at({1, 4}));
    EXPECT_EQ(cp2, b.piece_at({1, 5}));
}

TEST(MoveApplication, ApplyCastling)
{
    Board b;
    ColoredPiece cp1 = {WHITE, KING}, cp2 = {WHITE, ROOK},
                 cp3 = {BLACK, KING}, cp4 = {BLACK, ROOK};
    b.put(cp1, {7, 4}); b.put(cp2, {7, 7});
    b.put(cp3, {0, 4}); b.put(cp4, {0, 0});

    apply(b, Move{Castle{Square{7, 4}, KINGSIDE}, boost::none_t()});
    EXPECT_FALSE(b.piece_at({7, 4}));
    EXPECT_FALSE(b.piece_at({7, 7}));
    EXPECT_EQ(cp1, b.piece_at({7, 6}));
    EXPECT_EQ(cp2, b.piece_at({7, 5}));

    apply(b, Move{Castle{Square{0, 4}, QUEENSIDE}, boost::none_t()});
    EXPECT_FALSE(b.piece_at({0, 4}));
    EXPECT_FALSE(b.piece_at({0, 0}));
    EXPECT_EQ(cp3, b.piece_at({0, 2}));
    EXPECT_EQ(cp4, b.piece_at({0, 3}));
}

TEST(MoveApplication, ApplyPromotion)
{
    Board b;
    ColoredPiece cp1 = {BLACK, PAWN}, cp2 = {WHITE, QUEEN}, cp3 = {BLACK, QUEEN};
    b.put(cp1, {1, 0}); b.put(cp2, {0, 1});

    apply(b, Move{Promotion{{1, 0}, {0, 1}, BLACK}, Square{0, 1}});
    EXPECT_FALSE(b.piece_at({1, 0}));
    EXPECT_EQ(cp3, b.piece_at({0, 1}));
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
