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


// // ---------------- GameRoom ----------------
// struct GameRoom {
//     std::string roomID;
//     std::vector<std::shared_ptr<websocket::stream<tcp::socket>>> players;
//     std::shared_ptr<TicTacToe> game = std::make_shared<TicTacToe>();
//     std::mutex mtx;
// };

// // ---------------- GameManager ----------------
// class GameManager {
// private:
//     std::unordered_map<std::string, std::shared_ptr<GameRoom>> rooms_;
//     std::mutex mtx_;

// public:
//     std::shared_ptr<GameRoom> joinRoom(const std::string& roomID, std::shared_ptr<websocket::stream<tcp::socket>> ws) {
//         std::lock_guard<std::mutex> lock(mtx_);
//         auto it = rooms_.find(roomID);
//         if (it != rooms_.end()) {
//             std::lock_guard<std::mutex> roomLock(it->second->mtx);
//             it->second->players.push_back(ws);
//             return it->second;
//         } else {
//             auto room = std::make_shared<GameRoom>();
//             room->roomID = roomID;
//             room->players.push_back(ws);
//             rooms_[roomID] = room;
//             return room;
//         }
//     }
// };

// // ---------------- Async WebSocket read ----------------
// void asyncRead(std::shared_ptr<websocket::stream<tcp::socket>> ws,
//                std::shared_ptr<GameRoom> room,
//                beast::flat_buffer buffer);

// void broadcastToRoom(std::shared_ptr<GameRoom> room, const std::string &msg) {
//     std::lock_guard<std::mutex> lock(room->mtx);
//     for (auto& player : room->players) {
//         player->async_write(net::buffer(msg),
//             [player](boost::system::error_code ec, std::size_t bytes_transferred){
//                 if (ec) std::cerr << "Write error: " << ec.message() << std::endl;
//             });
//     }
// }

// void asyncRead(std::shared_ptr<websocket::stream<tcp::socket>> ws,
//                std::shared_ptr<GameRoom> room,
//                beast::flat_buffer buffer)
// {
//     ws->async_read(buffer,
//         [ws, room, buffer = std::move(buffer)](boost::system::error_code ec, std::size_t) mutable{
//             if (ec) {
//                 std::cerr << "Read error: " << ec.message() << std::endl;
//                 return;
//             }

//             auto msg = beast::buffers_to_string(buffer.data());
//             buffer.consume(buffer.size());

//             try {
//                 auto data = json::parse(msg);

//                 if (data.contains("reset")) {
//                     room->game->reset();
//                 } else if (data.contains("move")) {
//                     auto move = data["move"].get<std::vector<int>>();
//                     room->game->handleMove(move[0], move[1]);
//                 }

//                 // Broadcast updated game state
//                 auto stateMsg = room->game->getState().dump();
//                 broadcastToRoom(room, stateMsg);

//             } catch (const std::exception &e) {
//                 std::cerr << "JSON parse error: " << e.what() << std::endl;
//             }

//             // Recursively read again
//             asyncRead(ws, room, std::move(buffer));
//         }
//     );
// }

// // ---------------- Handle new connection ----------------
// void handleConn(tcp::socket&& socket, GameManager& gm) {
//     auto ws = std::make_shared<websocket::stream<tcp::socket>>(std::move(socket));
//     ws->accept();

//     beast::flat_buffer buffer;
//     ws->read(buffer);  // First message should contain room ID
//     auto msg = beast::buffers_to_string(buffer.data());
//     buffer.consume(buffer.size());

//     auto data = json::parse(msg);
//     std::string roomID = data["room"];
//     auto room = gm.joinRoom(roomID, ws);

//     asyncRead(ws, room, beast::flat_buffer{});
// }

// // ---------------- Main ----------------



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
