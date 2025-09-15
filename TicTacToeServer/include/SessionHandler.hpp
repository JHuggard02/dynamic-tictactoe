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
#include "Session.hpp"

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;
using json = nlohmann::json;



class SessionHandler : public std::enable_shared_from_this<SessionHandler> {
    std::vector<std::shared_ptr<Session>> sessions_;
    std::unordered_map<std::string, std::shared_ptr<Game>> games_;
    std::unordered_map<std::string, std::vector<std::shared_ptr<Session>>> gamesToSessions_;
    std::mutex mutex_;

public:
    SessionHandler() = default;

    void newSession(tcp::socket socket);
    void handleInput(std::shared_ptr<Session> wc, const std::string& input);

    int createGame(std::shared_ptr<Session> wc, const std::string& gameID, int rows, int cols, int winCond);
    int joinGame(std::shared_ptr<Session> wc, const std::string& gameID);
    void handleStartGame(std::shared_ptr<Session> wc);
    void handleMove(std::shared_ptr<Session> wc, const std::vector<int>& move);
    void handleReset(std::shared_ptr<Session> wc);
    void handleLeave(std::shared_ptr<Session> wc);

    void broadcastAll(const std::string& gameID, const std::string& msg);

private:
    void updatePlayer(std::shared_ptr<Session> wc, const nlohmann::json& data);
};
