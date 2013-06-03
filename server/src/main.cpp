#include "game.hpp"
#include "server.hpp"

using namespace std::placeholders;

int main() {
    Game game;
    boost::asio::io_service io;
    Server server(io);
    server.set_read_callback(
            std::bind(&Game::message_handler, game, _1, _2, _3));
    server.run();
    io.run();
    return 0;
}
