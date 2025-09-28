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

class Game : public std::enable_shared_from_this<Game> {
private:
    std::vector<std::vector<char>> board_;
    int turn_ = 0;
    std::vector<char> shapes_ = {'X', 'O', 'Y', 'Z', 'Q', 'T', 'A'};
    std::string winner_ = " ";
    int plays_ = 0;
    std::vector<std::shared_ptr<Session>> players_;
    std::mutex mutex_;
    int rows_ = 3;
    int cols_ = 3;
    int winCond_ = 3;
    int cells_ = 9;

public:
    Game() = default;

    Game(int rows, int cols, int winCond) : rows_(rows), cols_(cols), board_(rows, std::vector<char>(cols, ' ')), winCond_(winCond), cells_(rows * cols) {
        std::cout << "win Cond: " <<  winCond << std::endl;
    }

    void removePlayer(std::shared_ptr<Session>& id)
    {
        players_.erase(std::remove(players_.begin(), players_.end(), id)), players_.end();
    }

    bool noPlayers()
    {
        return players_.empty();
    }

    void handleMove(int row, int col) {
        if (board_[row][col] != ' ' || winner_ != " ") return;

        board_[row][col] = shapes_[turn_];
        plays_++;

        // Check win
        if (checkWin(row, col)) {
            winner_ = std::string(1, shapes_[turn_]);
        } else if (plays_ == full()) {
            winner_ = "Draw";
        }

        turn_ = (turn_ + 1) % players_.size();
    }

    void reset() {
        board_ = {{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}};
        turn_ = 0;
        winner_ = " ";
        plays_ = 0;
    }

    std::string getState() const {
        json boardJson = json::array();
        for (auto &row : board_) {
            json rowJson = json::array();
            for (auto &cell : row) rowJson.push_back(std::string(1, cell));
            boardJson.push_back(rowJson);
        }

        json data;
        data["board"] = boardJson;
        data["turn"] = turn_;
        data["curr-player"] = players_[turn_]->getUsername();
        data["winner"] = winner_;
        return data.dump();
    }

    void addPlayer(std::shared_ptr<Session> session)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        players_.push_back(session);
    }

    std::vector<std::string> getAllPlayers()
    {   
        std::vector<std::string> players;
        players.reserve(players_.size());
        for (auto player: players_){
            players.push_back(player->getUsername());
        }
        return players;
    }

private:
    bool checkWin(int row, int col) {
        static const std::vector<std::pair<int,int>> directions = {
            {1,0},{0,1},{1,1},{1,-1}
        };

        char symbol = shapes_[turn_];
        int nr, nc;
        for (auto [dr, dc] : directions) {
            int count = 1;

            nr = row;
            nc = col;
            while (nr + dr >= 0 && nr + dr < rows_ 
                && nc + dc >= 0 && nc + dc < cols_
                && board_[nr + dr][nc + dc] == shapes_[turn_]){
                    count ++;
                    nr += dr;
                    nc += dc;
                }

            nr = row;
            nc = col;

             while (nr - dr >= 0 && nr - dr < rows_ 
                && nc - dc >= 0 && nc - dc < cols_
                && board_[nr - dr][nc - dc] == shapes_[turn_]){
                    count ++;
                    nr -= dr;
                    nc -= dc;
                }

                
            if (count >= winCond_) return true;
        }
        return false;
    }

    bool full()
    {
        return plays_ == cells_;
    }

    
};