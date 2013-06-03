#include "server.hpp"

#include <iostream>

namespace asio = boost::asio;
namespace ip = boost::asio::ip;
namespace sys = boost::system;
using namespace std::placeholders;

Server::Server(asio::io_service& io) :
    acceptor(io, ip::tcp::endpoint(ip::tcp::v4(), 12345)),
    players(0)
{}

void Server::set_read_callback(std::function<void(Server&, int, std::string)> f)
{
    read_callback = f;
}

void Server::run()
{
    accept_next();
}

void Server::broadcast(const std::string& msg)
{
    send(1, msg);
    send(2, msg);
}

void Server::send(int player, const std::string& msg)
{
    std::shared_ptr<ip::tcp::socket> s;
    switch (player) {
    case 1:
        s = player1;
        break;
    case 2:
        s = player2;
        break;
    default:
        return;
    }
    if (!s) {
        return;
    }

    auto wh = std::bind(&Server::write_handler, this, s, _1);
    asio::async_write(*s, asio::buffer(msg + "\n"), wh);
}

void Server::accept_next()
{
    auto conn = std::make_shared<ip::tcp::socket>
                (ip::tcp::socket(acceptor.get_io_service()));
    auto handler = std::bind(&Server::accept_handler, this, conn, _1);
    acceptor.async_accept(*conn, handler);
}

void Server::accept_handler(std::shared_ptr<ip::tcp::socket> conn,
                            const sys::error_code& error)
{
    if (error) {
        std::cerr << "Error: " << error << std::endl;
        return;
    }

    auto wh = std::bind(&Server::write_handler, this, conn, _1);
    if (players < 2) {
        if (players == 0) {
            player1 = conn;
            auto rh = std::bind(&Server::read_handler, this, conn, 1, _1);
            asio::async_read_until(*player1, buf1, '\n', rh);
        } else {
            player2 = conn;
            auto rh = std::bind(&Server::read_handler, this, conn, 2, _1);
            asio::async_read_until(*player2, buf2, '\n', rh);
        }
        std::cout << "New connection.\n";
        ++players;
    } else {
        asio::async_write(
                *conn,
                asio::buffer("error full"),
                wh);
    }
    accept_next();
}

void Server::read_handler(std::shared_ptr<ip::tcp::socket> conn,
                          int player,
                          const sys::error_code& error)
{
    if (error) {
        std::cerr << "Error: " << error << std::endl;
        return;
    }

    asio::streambuf* buf = player == 1 ? &buf1 : &buf2;

    std::string msg((std::istreambuf_iterator<char>(buf)),
                    std::istreambuf_iterator<char>());

    if (read_callback) {
        read_callback(*this, player, msg);
    }

    auto rh = std::bind(&Server::read_handler, this, conn, player, _1);
    asio::async_read_until(*conn, *buf, '\n', rh);
}

void Server::write_handler(std::shared_ptr<ip::tcp::socket>,
                           const sys::error_code& error)
{
    if (error) {
        std::cerr << "Error: " << error << std::endl;
    }
}
