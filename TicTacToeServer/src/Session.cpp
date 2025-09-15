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
#include "../include/Game.hpp"
#include "../include/Session.hpp"
#include "../include/SessionHandler.hpp"


namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;
using json = nlohmann::json;

using namespace std;

Session::Session(tcp::socket socket, std::shared_ptr<SessionHandler> handler)
    : ws_(std::move(socket)), handler_(handler) {}

void Session::start() {
    ws_.async_accept(
        beast::bind_front_handler(&Session::on_accept, shared_from_this()));
}

void Session::send(const std::string& message){
    std::lock_guard<std::mutex> lock(mutex_);
    bool writeInProgress = !writeQueue_.empty();
    writeQueue_.push_back(message);
    if (!writeInProgress) write();
}

void Session::resetPlayer()
{
    player.gameID = "";
    player.username = "";
}

void Session::write() {
    auto self = shared_from_this();

  
    if (writeQueue_.empty()) {
        return;
    }
    std::string msg = writeQueue_.front();
    

    ws_.async_write(net::buffer(msg),
        [self](beast::error_code ec, std::size_t) {
            if (!ec) {
                bool more = false;
                {
                    std::lock_guard<std::mutex> lock(self->mutex_);
                    self->writeQueue_.pop_front();
                    more = !self->writeQueue_.empty();
                }
                if (more) self->write();
            } else {
                std::cerr << "Write error: " << ec.message() << "\n";
            }
        });
}


void Session::update_status(PlayerState status) {
    player.state = status;
}

void Session::on_accept(beast::error_code ec) {
    if (ec) {
        cerr << "Accept Failed: " << ec.message() << "\n";
        return;
    }
    doRead();
}

void Session::doRead() {
    ws_.async_read(buffer_, [self = shared_from_this()](beast::error_code ec, std::size_t) {
        if (!ec) {
            string message = beast::buffers_to_string(self->buffer_.data());
            self->buffer_.consume(self->buffer_.size());

            if (auto handler = self->handler_.lock()) {
                handler->handleInput(self, message); // now works
            }

            self->doRead();
        } else {
            cerr << "Read Error: " << ec.message() << "\n";
        }
    });
}
