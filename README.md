# Multi-Player Tic-Tac-Toe

A real-time multiplayer tic-tac-toe game supporting up to 7 players on customizable boards with configurable win conditions. Built with C++ WebSocket backend and React frontend.

## Features

- **Multi-Player Support**: Up to 7 players can play simultaneously
- **Customizable Board**: Board sizes up to 17x17 cells
- **Flexible Win Conditions**: Configure win condition from 3 to 17 connections
- **Real-Time Communication**: WebSocket-based communication for instant updates
- **Room System**: Create or join game rooms with unique IDs
- **Player Management**: Username-based player identification
- **Game State Management**: Automatic turn management and win detection

## Game Symbols

Players are assigned unique symbols in order:
- Player 1: **X** (Blue)
- Player 2: **O** (Red) 
- Player 3: **Y** (Green)
- Player 4: **Z** (Purple)
- Player 5: **Q** (Yellow)
- Player 6: **T** (Pink)
- Player 7: **A** (Indigo)

## Architecture

### Backend (C++)
- **WebSocket Server**: Built with Boost.Beast for real-time communication
- **Session Management**: Handle multiple concurrent player connections
- **Game Logic**: Board state management and win condition checking
- **JSON Communication**: Structured message protocol using nlohmann/json

### Frontend (React)
- **Real-Time UI**: Instant board updates via WebSocket
- **Responsive Design**: Tailwind CSS styling with hover effects
- **Game States**: Join game, waiting room, and active game interfaces
- **Error Handling**: User-friendly error messages and connection status

## Prerequisites

### Backend Dependencies
- **CMake** 3.15 or higher
- **C++17** compatible compiler
- **Boost** (header-only libraries used)
- **nlohmann/json** library
- **pthread** support

### Frontend Dependencies
- **Node.js** and npm
- **React** application setup

## Installation & Setup

### Backend (C++ WebSocket Server)

1. **Install Dependencies**
   ```bash
   # macOS (using Homebrew)
   brew install boost nlohmann-json cmake
   
   # Ubuntu/Debian
   sudo apt-get install libboost-all-dev nlohmann-json3-dev cmake build-essential
   ```

2. **Build the Server**
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

3. **Run the Server**
   ```bash
   ./ws
   ```
   The server will start on `ws://127.0.0.1:8083`

### Frontend (React Application)

1. **Install Dependencies**
   ```bash
   npm install
   ```

2. **Start the Development Server**
   ```bash
   npm start
   ```
   The frontend will be available at `http://localhost:3000`

## How to Play

### Creating a Game
1. Enter your username
2. Choose a unique Game ID
3. Set board dimensions (rows × columns, max 17×17)
4. Set win condition (number of symbols in a row to win, max 17)
5. Click "Create Game"
6. Wait for other players to join
7. Click "Start Game" when ready

### Joining a Game
1. Enter your username
2. Enter the Game ID of an existing game
3. Click "Join Game"
4. Wait for the game creator to start the game

### Playing
- Players take turns placing their symbols on the board
- The current player is highlighted
- First player to get the required number of symbols in a row (horizontal, vertical, or diagonal) wins
- Use "Reset Game" to clear the board
- Use "Leave Game" to exit the current game

## API/Message Protocol

The WebSocket communication uses JSON messages with the following structure:

### Client to Server Messages

**Update Player (Create/Join Game)**
```json
{
  "update-player": true,
  "username": "player_name",
  "gameID": "room_123",
  "create": true/false,
  "join": true/false,
  "rows": "5",
  "cols": "5",
  "win-cond": "4"
}
```

**Make Move**
```json
{
  "update-game": true,
  "start": false,
  "move": [row, col]
}
```

**Start Game**
```json
{
  "update-game": true,
  "start": true
}
```

**Reset Game**
```json
{
  "reset": true
}
```

**Leave Game**
```json
{
  "leave": true
}
```

### Server to Client Messages

**Game State Update**
```json
{
  "board": [["X", " ", "O"], [" ", "X", " "], ["O", " ", "X"]],
  "turn": "X",
  "winner": " ",
  "curr-player": "player_name"
}
```

**Player List Update**
```json
{
  "update-players": true,
  "players": ["player1", "player2", "player3"]
}
```



## Configuration

### Default Settings
- **Server Port**: 8083
- **Default Board**: 3×3
- **Default Win Condition**: 3
- **Max Players**: 7
- **Max Board Size**: 17×17
- **Max Win Condition**: 17

### Customization
- Board dimensions and win conditions are configurable per game
- Player symbols and colors are defined in the frontend
- Server port can be modified in the server source code

## Troubleshooting

### Common Issues

**Server Won't Start**
- Check if port 8083 is available
- Ensure all dependencies are installed
- Verify Boost installation path in CMakeLists.txt

**Connection Failed**
- Ensure the WebSocket server is running
- Check firewall settings
- Verify the WebSocket URL in the frontend matches the server

**Build Errors**
- Ensure C++17 support is available
- Check CMake version (3.15+ required)
- Verify all dependencies are properly installed

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## License

This project is open source. See the license file for details.

