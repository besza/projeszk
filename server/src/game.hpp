#ifndef GAME_HPP
#define GAME_HPP

#include "chess.hpp"

class Server;

class Game
{
public:
    Game();

    void message_handler(Server&, int player, std::string);
private:
    bool playing;

    bool ready1, ready2;
    Color player1, player2;

    Board board;
    Color current_color;

    void reset_waiting();
    void reset_playing();

    bool& ready(int);
    Color& player_color(int);
    int other(int);

    void error(Server&, int player) const;
};

std::string show(Color);
std::string show(CastleDir);
std::string show(Square);
std::string show(MoveResult);

boost::optional<Square> read_square(const std::string&);

#endif
