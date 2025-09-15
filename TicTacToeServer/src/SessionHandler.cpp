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



void SessionHandler::newSession(tcp::socket socket) {
    auto self = shared_from_this();
    auto session = std::make_shared<Session>(std::move(socket), self);

    {
        std::lock_guard<std::mutex> lk(mutex_);
        sessions_.push_back(session);
    }

    session->start();
}

void SessionHandler::handleInput(std::shared_ptr<Session> wc, const std::string& input) {
    try {
        auto jsonData = json::parse(input);
        std::cout << "Worked\n";
        if (jsonData.contains("update-player") && jsonData["update-player"].get<bool>()) {
            updatePlayer(wc, jsonData);
        } else if (jsonData.contains("update-game") && jsonData["update-game"].get<bool>()) {
            if (jsonData.contains("start") && jsonData["start"].get<bool>()){
                handleStartGame(wc);
            }else{
                std::vector<int> move = jsonData["move"];
                handleMove(wc, move);
            }
        }else if (jsonData.contains("reset") && jsonData["reset"].get<bool>()){
            handleReset(wc);
        }else if (jsonData.contains("leave") && jsonData["leave"].get<bool>()){
            handleLeave(wc);
        }
    } catch (const json::exception& e) {
        std::cerr << "JSON Error in handleInput: " << e.what() << std::endl;
    }
}

void SessionHandler::updatePlayer(std::shared_ptr<Session> wc, const json& data) {
    std::string username, gameID;
    try {
        username = data.value("username", "");
        gameID   = data.value("gameID", "");
        bool create = data.value("create", false);
        bool join   = data.value("join", false);

        if (create) {
            int rows = std::stoi(data["rows"].get<std::string>());
            int cols = std::stoi(data["cols"].get<std::string>());
            int winCond = std::stoi(data["win-cond"].get<std::string>());
            if (winCond > std::max(rows, cols)){
                json response;
                response["error"] = "Win is impossible";
                wc->send(response.dump());
                return;
            }
            int res = createGame(wc, gameID, rows, cols, winCond);
            if (res == -1) {
                json response;
                response["create-failed"] = true;
                wc->send(response.dump());
                return;
            }
        } else if (join) {
            int res = joinGame(wc, gameID);
            if (res == -1) {
                json response;
                response["join-failed"] = true;
                wc->send(response.dump());
                return;
            }
        }

        wc->setUsername(username);
        wc->setGameID(gameID);

        auto game = games_[gameID];
        game->addPlayer(wc);
        std::vector<std::string> players = game->getAllPlayers();

        wc->update_status(PlayerState::Waiting);
        json response;
        response["update-player"] = true;
        response["gameID"] = gameID;
        response["username"] = username;
        response["waiting"] = true;
        response["create-failed"] = false;
        response["join-failed"]   = false;

        wc->send(response.dump());

        json playerUpdate;
        playerUpdate["update-players"] = true;
        playerUpdate["players"] = players;

        broadcastAll(gameID, playerUpdate.dump());

    } catch (const json::exception& e) {
        std::cerr << "JSON Error in updatePlayer: " << e.what() << std::endl;
    }
}


int SessionHandler::createGame(std::shared_ptr<Session> wc, const std::string& gameID, int rows, int cols, int winCond) {
    std::lock_guard<std::mutex> lk(mutex_);
    if (games_.count(gameID)) {
        std::cout << "Game Already Exists\n";
        json response;
        response["error"] = "Game Already Exists";
        wc->send(response.dump());
        return -1;
    }
    auto new_Game = std::make_shared<Game>(rows, cols, winCond);
    games_[gameID] = new_Game;
    gamesToSessions_[gameID].push_back(wc);
    return 0;
}


int SessionHandler::joinGame(std::shared_ptr<Session> wc, const std::string& gameID) {
    std::lock_guard<std::mutex> lk(mutex_);
    if (!games_.count(gameID)) {
        std::cout << "No Game with this ID Exists\n";
        json response;
        response["error"] = "No Game with this ID Exists";
        wc->send(response.dump());
        return -1;
    }
    gamesToSessions_[gameID].push_back(wc);
    return 0;
}

void SessionHandler::handleStartGame(std::shared_ptr<Session> wc){
    std::string gameID = wc->getGameID();

    json response;
    response["start-game"] = true;

    broadcastAll(gameID, response.dump());

    auto game = games_[gameID];
    broadcastAll(gameID, game->getState());
}

void SessionHandler::handleMove(std::shared_ptr<Session> wc, const std::vector<int>& move)
{
    std::string gameID = wc->getGameID();
    auto game = games_[gameID];

    game->handleMove(move[0], move[1]);
    std::string response = game->getState();
    broadcastAll(gameID, response);
}

void SessionHandler::handleReset(std::shared_ptr<Session> wc)
{
    std::string gameID = wc->getGameID();
    auto game = games_[gameID];
    game->reset();

    broadcastAll(gameID, game->getState());
}

void SessionHandler::handleLeave(std::shared_ptr<Session> wc)
{
    std::string gameID = wc->getGameID();
    auto game = games_[gameID];

    game->removePlayer(wc);

    wc->resetPlayer();

    if (game->noPlayers()){
        auto gameIt = games_.find(gameID);
        if (gameIt != games_.end()){
            games_.erase(gameID);
        }

        auto gtsIt = gamesToSessions_.find(gameID);
        if (gtsIt != gamesToSessions_.end()){
            gamesToSessions_.erase(gameID);
        }

    }
}


void SessionHandler::broadcastAll(const std::string& gameID, const std::string& msg)
{
    for (auto session : gamesToSessions_[gameID]){
        session->send(msg);
    }
}