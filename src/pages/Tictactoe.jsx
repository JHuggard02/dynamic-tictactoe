import { useEffect, useState } from "react";
import Board from "../components/Board";
import JoinGame from "../components/JoinGame";
import Waiting from "../components/Waiting";

export default function App() {
  
  // Game State
  const [board, setBoard] = useState([
    [" ", " ", " "],
    [" ", " ", " "],
    [" ", " ", " "]
  ]);
  const [turn, setTurn] = useState("X");
  const [winner, setWinner] = useState(" ");
  const [currentPlayer, setCurrentPlayer] = useState("");

  // Player State
  const [username, setUserName] = useState("");
  const [gameID, setGameID] = useState("");
  const [players, setPlayers] = useState([]);

  // UI State
  const [joined, setJoined] = useState(false);
  const [waiting, setWaiting] = useState(false);
  const [room, setRoom] = useState(" ");
  const [error, setError] = useState("");

  // Connection State
  const [socket, setSocket] = useState(null);
  const [connected, setConnected] = useState(false);
  const [connectionStatus, setConnectionStatus] = useState("Connecting...");

  // Game Configuration
  const [rows, setRows] = useState("3");
  const [cols, setCols] = useState("3");
  const [winCond, setWinCond] = useState("3");


  const shapeStyles = {
    X: "text-blue-600 bg-blue-50",
    O: "text-red-600 bg-red-50",
    Y: "text-green-600 bg-green-50",
    Z: "text-purple-600 bg-purple-50",
    Q: "text-yellow-600 bg-yellow-50",
    T: "text-pink-600 bg-pink-50",
    A: "text-indigo-600 bg-indigo-50",
  };


  const clearError = () => {
    setError("");
  };

  const reset = () => {
    setCurrentPlayer("");
    setUserName("");
    setGameID("");
    setPlayers([]);
    setJoined(false);
    setWaiting(false);
    setWinner("");
    
    socket.send(JSON.stringify({ leave: true }));
  };

  const getWinnerMessage = () => {
    if (winner === "Draw") return "It's a Tie!";
    if (winner !== " ") return `Winner: ${currentPlayer}`;
    return `Current Turn: ${currentPlayer}`;
  };

  const getCellClass = (cellValue, winner, connected) => {
    let baseClass =
      "w-16 h-16 border-2 border-gray-800 flex items-center justify-center text-3xl font-bold cursor-pointer transition-all duration-200 ";

    if (shapeStyles[cellValue]) {
      baseClass += shapeStyles[cellValue];
    } else {
      baseClass += "bg-gray-100 hover:bg-gray-200";
    }

    if (winner !== " " || !connected) {
      baseClass += " cursor-not-allowed opacity-75";
    }

    return baseClass;
  };


  const updateUsername = (event) => {
    setUserName(event.target.value);
    console.log(username);
  };

  const updateGameID = (event) => {
    setGameID(event.target.value);
  };


  const makeMove = (row, col) => {
    clearError();
    if (!socket || !connected || board[row][col] !== " " || winner !== " ") return;
    
    console.log("Make Move");
    try {
      socket.send(JSON.stringify({ 
        "update-game": true, 
        start: false, 
        move: [row, col] 
      }));
    } catch (e) {
      console.error("Error sending move:", e);
    }
  };

  const startGame = () => {
    clearError();
    if (!socket || !connected) return;
    
    try {
      socket.send(JSON.stringify({ 
        "update-game": true, 
        start: true 
      }));
    } catch (e) {
      console.error("Error Starting Game: ", e);
    }
  };
  
  const resetGame = () => {
    clearError();
    if (!socket || !connected) return;
    
    try {
      socket.send(JSON.stringify({ reset: true }));
    } catch (e) {
      console.error("Error resetting game:", e);
    }
  };

  const createGame = () => {
    clearError();

    if (!username) {
      setError("Username must not be empty");
      return;
    }

    if (!gameID) {
      setError("You must choose a game ID");
    }

    if (!socket || !connected) return;

    try {
      socket.send(JSON.stringify({
        "update-player": true, 
        username, 
        gameID, 
        create: true, 
        join: false, 
        rows, 
        cols, 
        "win-cond": winCond
      }));
    } catch (e) {
      console.log("Error Sending User Info", e);
    }
  };

  const joinGame = () => {
    clearError();

    if (!username) {
      setError("Username must not be empty");
      return;
    }

    if (!gameID) {
      setError("You must choose a game ID");
    }

    if (!socket || !connected) return;

    try {
      socket.send(JSON.stringify({
        "update-player": true, 
        username, 
        gameID, 
        create: false, 
        join: true
      }));
    } catch (e) {
      console.log("Error Sending User Info", e);
    }
  };


  const handleWebSocketMessage = (event) => {
    try {
      const data = JSON.parse(event.data);
      console.log("MESSAGE");
      console.log("DATA: ", data);
      
      // Handle errors
      if (data.error) {
        setError(data.error);
      }
      
      // Handle player updates
      if (data["update-player"] && data["update-player"] == true) {
        setUserName(data.username);
        setGameID(data.gameID);
        setWaiting(data.waiting);
      } 
      else if (data["update-players"] && data["update-players"] == true) {
        setPlayers(data.players);
        console.log(players);
      } 
      else if (data["start-game"] && data["start-game"] == true) {
        setJoined(true);
        setWaiting(false);
      }
      
      // Handle game state updates
      if (data.board) {
        setBoard(data.board);
        setTurn(data.turn);
        setWinner(data.winner);
        setCurrentPlayer(data["curr-player"]);
      } else {
        console.log("NO BOARD");
      }
      
    } catch (e) {
      console.error("Error parsing message:", e);
    }
  };


  // WebSocket Connection Effect
  useEffect(() => {
    const ws = new WebSocket("ws://127.0.0.1:8083");
    
    ws.onopen = () => {
      setConnected(true);
      setConnectionStatus("Connected");
      console.log("Connected to WebSocket server");
    };
    
    ws.onmessage = handleWebSocketMessage;
    
    ws.onclose = () => {
      setConnected(false);
      setConnectionStatus("Disconnected");
      console.log("Disconnected from WebSocket server");
    };
    
    ws.onerror = (error) => {
      setConnected(false);
      setConnectionStatus("Connection Error");
      console.error("WebSocket error:", error);
    };
    
    setSocket(ws);
    
    return () => {
      if (ws.readyState === WebSocket.OPEN) {
        ws.close();
      }
    };
  }, []);

  // Board Update Effect
  useEffect(() => {
    console.log("Board Updated: ");
    console.log(board);
  }, [board]);



  return (
    <div className="min-h-screen bg-gradient-to-br from-purple-400 to-blue-600 flex items-center justify-center p-4">
      <div className="bg-white rounded-lg shadow-2xl p-8 win-w-[28rem]">
        
        {/* Header */}
        <h1 className="text-4xl font-bold text-center mb-2 text-gray-800">
          Tic Tac Toe
        </h1>

        {/* Room Display */}
        <div className="text-center mb-6">
          <h2>Room: {room}</h2>
        </div>
        
        {/* Connection Status */}
        <div className="text-center mb-6">
          <div className={`inline-block px-3 py-1 rounded-full text-sm font-medium ${
            connected ? 'bg-green-100 text-green-800' : 'bg-red-100 text-red-800'
          }`}>
            {connectionStatus}
          </div>
        </div>
        
        {/* Game Components */}
        {joined && (
          <Board 
            getWinnerMessage={getWinnerMessage} 
            board={board} 
            makeMove={makeMove} 
            getCellClass={getCellClass} 
            winner={winner} 
            currentPlayer={currentPlayer} 
            username={username} 
            resetGame={resetGame} 
            connected={connected} 
            reset={reset}
          />
        )}

        {waiting && (
          <Waiting 
            startGame={startGame} 
            players={players}
          />
        )}

        {(!joined && !waiting) && (
          <JoinGame 
            updateGameID={updateGameID} 
            updateUsername={updateUsername} 
            username={username} 
            gameID={gameID} 
            createGame={createGame} 
            joinGame={joinGame} 
            rows={rows} 
            setRows={setRows} 
            cols={cols} 
            setCols={setCols} 
            winCond={winCond} 
            setWinCond={setWinCond}
          />
        )}
               
        {/* Connection Info */}
        <div className="text-center space-y-4">
          {!connected && (
            <p className="text-red-600 text-sm">
              Make sure the WebSocket server is running on localhost:9001
            </p>
          )}
        </div>

        {/* Error Display */}
        {error && (
          <h1 className="mt-5 text-center text-red-600 text-sm">
            {error}
          </h1>
        )}
        
        {/* Footer */}
        <div className="mt-8 text-center text-sm text-gray-600">
          <p>Multiple players can connect and play together!</p>
        </div>
      </div>
    </div>
  );
}