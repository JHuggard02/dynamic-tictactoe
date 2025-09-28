#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <iostream>
#include <string>

#include "../include/SessionHandler.hpp"
#include "../include/Session.hpp"
#include "../include/Game.hpp"

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;
using json = nlohmann::json;


int main() {
    try {
        net::io_context ioc{1};
        tcp::endpoint endpoint{net::ip::make_address("127.0.0.1"), 8083};
        tcp::acceptor acceptor{ioc, endpoint};

        auto handler = std::make_shared<SessionHandler>();

        std::cout << "WebSocket server running on ws://127.0.0.1:8083\n";

        std::function<void()> do_accept;

        do_accept = [&](){
            acceptor.async_accept([&](beast::error_code ec, tcp::socket socket){
                if (!ec) {
                    std::cout << "Client Connected\n";
                    handler->newSession(std::move(socket));
                }else{
                    std::cerr << "Accept error: " << ec.message() << "\n";
                }
                do_accept();
            });
        };

        do_accept();
        ioc.run();
    } catch (std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
    }
}
