#include "game.hpp"

#include "server.hpp"
#include <sstream>
#include <boost/algorithm/string.hpp>

Game::Game()
{
    reset_waiting();
}

void Game::message_handler(Server& server, int player, std::string msg)
{
    std::vector<std::string> words;
    boost::trim(msg);
    boost::split(words, msg, boost::is_any_of("\t "), boost::token_compress_on);

    if (words.empty()) {
        error(server, player);
        return;
    }

    if (words[0] == "ready") {
        if (ready(player)) {
            error(server, player);
            return;
        }

        ready(player) = true;
        player_color(player) = ready(other(player)) ? BLACK : WHITE;
        server.send(player, "color " + show(player_color(player)));

        if (ready1 && ready2) {
            reset_playing();
            server.broadcast("start");
        }
    } else if (words[0] == "say") {
        std::stringstream ss;
        ss << "say" << player;
        for (size_t i = 1; i < words.size(); ++i) {
            ss << " " << words[i];
        }
        server.broadcast(ss.str());
    } else if (words[0] == "move") {
        if (!playing || current_color != player_color(player) ||
            words.size() != 3) {
            server.send(player, "error move");
            return;
        }

        boost::optional<Square> maybe_from = read_square(words[1]);
        boost::optional<Square> maybe_to = read_square(words[2]);
        if (!maybe_from || !maybe_to) {
            error(server, player);
            return;
        }

        boost::optional<MoveResult> maybe_move_result =
            try_move(board, player_color(player), *maybe_from, *maybe_to);
        if (!maybe_move_result) {
            server.send(player, "error move");
            return;
        }

        current_color = current_color == WHITE ? BLACK : WHITE;
        if (maybe_move_result->opponent_cannot_move) {
            reset_waiting();
        }

        server.broadcast(show(*maybe_move_result));
    } else if (words[0] == "resign") {
        if (!playing || current_color != player_color(player)) {
            error(server, player);
            return;
        }

        reset_waiting();
        server.broadcast("resign");
    } else {
        error(server, player);
    }
}

void Game::reset_waiting()
{
    playing = false;
    ready1 = ready2 = false;
}

void Game::reset_playing()
{
    playing = true;
    board = initial_position();
    current_color = WHITE;
}

bool& Game::ready(int player)
{
    return player == 1 ? ready1 : ready2;
}

Color& Game::player_color(int player)
{
    return player == 1 ? player1 : player2;
}

int Game::other(int player) {
    return 3 - player;
}

void Game::error(Server& server, int player) const
{
    server.send(player, "error command");
}

std::string show(Color c)
{
    return c == WHITE ? "white" : "black";
}

std::string show(CastleDir cd)
{
    return cd == KINGSIDE ? "kingside" : "queenside";
}

std::string show(Square s)
{
    std::stringstream ss;
    ss << char(s.col + 'a') << (8 - s.row);
    return ss.str();
}

struct ShowMoveVisitor : public boost::static_visitor<>
{
    bool hit;
    mutable std::string result;

    ShowMoveVisitor(bool b) : hit(b) {}

    void operator()(SimpleMove& m) const
    {
        std::stringstream ss;
        ss << (hit ? "hit " : "move ") << show(m.from) << " " << show(m.to);
        result = ss.str();
    }

    void operator()(Castle& m) const
    {
        std::stringstream ss;
        ss << "castle " << show(m.dir);
        result = ss.str();
    }

    void operator()(Promotion& m) const
    {
        std::stringstream ss;
        ss << (hit ? "promotion-hit " : "promotion ") <<
              show(m.from) << " " << show(m.to);
        result = ss.str();
    }
};

std::string show(MoveResult mr)
{
    auto v = ShowMoveVisitor(mr.move.hit);
    boost::apply_visitor(v, mr.move.movement);
    std::string str = v.result;
    if (mr.gave_check) {
        if (mr.opponent_cannot_move) {
            str += " checkmate";
        } else {
            str += " check";
        }
    } else if (mr.opponent_cannot_move) {
        str += " stalemate";
    }
    return str;
}

boost::optional<Square> read_square(const std::string& str)
{
    if (str.size() != 2 || str[0] < 'a' || str[0] > 'z' ||
        str[1] < '1' || str[1] > '8') {
        return boost::none_t();
    }

    return Square{7 - (str[1] - '1'), str[0] - 'a'};
}
