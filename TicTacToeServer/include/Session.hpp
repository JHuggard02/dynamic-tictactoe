#pragma once

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
#include "Game.hpp"

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;
using json = nlohmann::json;

enum class PlayerState{
    NoGame, 
    Waiting, 
    Joined,
};

struct Player {
    PlayerState state = PlayerState::NoGame;
    std::string username;
    std::string gameID;
};

class SessionHandler;

class Session : public std::enable_shared_from_this<Session> {
    websocket::stream<tcp::socket> ws_;
    beast::flat_buffer buffer_;
    Player player;
    std::weak_ptr<SessionHandler> handler_;
    std::deque <std::string> writeQueue_;
    std::mutex mutex_;

    
public:
    Session(tcp::socket socket, std::shared_ptr<SessionHandler> handler);

    void start();
    void send(const std::string& message);
    void write();
    void update_status(PlayerState status);

    void setUsername(const std::string& username) {player.username = username; }
    void setGameID(const std::string& gameID) {player.gameID = gameID; }

    void resetPlayer();

    std::string getUsername() { return player.username; }
    std::string getGameID() { return player.gameID; }

private:
    void on_accept(beast::error_code ec);
    void doRead();
};