#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <algorithm>
#include <string>
#include <nlohmann/json.hpp>
#include "Session.hpp"

using json = nlohmann::json;

class Game : public std::enable_shared_from_this<Game> {
private:
    std::vector<std::vector<char>> board_;
    std::vector<char> shapes_ = {'X', 'O', 'Y', 'Z', 'Q', 'T', 'A'};
    std::vector<std::shared_ptr<Session>> players_;
    std::mutex mutex_;

    int rows_ = 3;
    int cols_ = 3;
    int winCond_ = 3;
    int turn_ = 0;
    int plays_ = 0;
    int cells_ = 9;
    std::string winner_ = " ";

public:
    Game() = default;

    Game(int rows, int cols, int winCond) 
        : rows_(rows), cols_(cols), winCond_(winCond), 
          board_(rows, std::vector<char>(cols, ' ')), cells_(rows * cols) {}

    void addPlayer(const std::shared_ptr<Session>& session) {
        std::lock_guard lock(mutex_);
        players_.push_back(session);
    }

    void removePlayer(const std::shared_ptr<Session>& session) {
        std::lock_guard lock(mutex_);
        players_.erase(std::remove(players_.begin(), players_.end(), session), players_.end());
        if (turn_ >= static_cast<int>(players_.size())) turn_ = 0;
    }

    bool noPlayers() const {
        std::lock_guard lock(mutex_);
        return players_.empty();
    }

    void handleMove(int row, int col) {
        if (board_[row][col] != ' ' || winner_ != " ") return;

        board_[row][col] = shapes_[turn_];
        plays_++;

        if (checkWin(row, col)) winner_ = std::string(1, shapes_[turn_]);
        else if (full()) winner_ = "Draw";

        turn_ = (turn_ + 1) % players_.size();
    }

    void reset() {
        for (auto& row : board_) std::fill(row.begin(), row.end(), ' ');
        turn_ = 0;
        winner_ = " ";
        plays_ = 0;
    }

    std::string getState() const {
        std::lock_guard lock(mutex_);

        json boardJson = json::array();
        for (const auto& row : board_) {
            json rowJson;
            for (char c : row) rowJson.push_back(std::string(1, c));
            boardJson.push_back(rowJson);
        }

        json data;
        data["board"] = boardJson;
        data["turn"] = turn_;
        data["curr-player"] = !players_.empty() ? players_[turn_]->getUsername() : "";
        data["winner"] = winner_;
        return data.dump();
    }

    std::vector<std::string> getAllPlayers() const {
        std::lock_guard lock(mutex_);
        std::vector<std::string> names;
        names.reserve(players_.size());
        for (const auto& p : players_) names.push_back(p->getUsername());
        return names;
    }

private:
    bool full() const { return plays_ == cells_; }

    bool checkWin(int row, int col) const {
        static const std::vector<std::pair<int,int>> directions = {{1,0},{0,1},{1,1},{1,-1}};
        char symbol = shapes_[turn_];

        for (auto [dr, dc] : directions) {
            int count = 1;

            auto countDir = [&](int rStep, int cStep) {
                int nr = row + rStep;
                int nc = col + cStep;
                int c = 0;
                while (nr >= 0 && nr < rows_ && nc >= 0 && nc < cols_ && board_[nr][nc] == symbol) {
                    c++;
                    nr += rStep;
                    nc += cStep;
                }
                return c;
            };

            count += countDir(dr, dc);
            count += countDir(-dr, -dc);

            if (count >= winCond_) return true;
        }

        return false;
    }
};
