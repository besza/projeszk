#ifndef SERVER_HPP
#define SERVER_HPP

#include <memory>
#include <boost/asio.hpp>

class Server
{
public:
    Server(boost::asio::io_service&);

    void set_read_callback(std::function<void(Server&, int, std::string)>);
    void run();

    void broadcast(const std::string&);
    void send(int player, const std::string&);
private:
    boost::asio::ip::tcp::acceptor acceptor;
    std::function<void(Server&, int, std::string)> read_callback;
    int players;
    std::shared_ptr<boost::asio::ip::tcp::socket> player1, player2;
    boost::asio::streambuf buf1, buf2;

    void accept_next();
    void accept_handler(std::shared_ptr<boost::asio::ip::tcp::socket>,
                        const boost::system::error_code&);
    void read_handler(std::shared_ptr<boost::asio::ip::tcp::socket>,
                      int,
                      const boost::system::error_code&);
    void write_handler(std::shared_ptr<boost::asio::ip::tcp::socket>,
                       const boost::system::error_code&);
};

#endif
