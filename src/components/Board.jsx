import BackButton from "./BackButton";


export default function Board({getWinnerMessage, board, makeMove, getCellClass, winner, currentPlayer, username, resetGame, connected, reset}){
    return(
         <div>
            <div className="flex mb-4 justify-center">
              <BackButton onClick={reset}/>
            </div>
            
            <div className="text-center mb-6">
                <h2 className={`text-2xl font-semibold ${
                    winner === "Draw" ? "text-yellow-600" : 
                    winner !== " " ? "text-green-600" : "text-gray-700"
                }`}>
                    {getWinnerMessage()}
                </h2>
            </div>

        <div className="flex flex-col items-center mb-6">
          {board.map((row, i) => (
            <div key={i} className="flex">
              {row.map((cell, j) => (
                <div
                  key={j}
                  onClick={(currentPlayer == username) ? () => makeMove(i, j) : () => {}}
                  className={getCellClass(cell)}
                >
                  {cell === " " ? "" : cell}
                </div>
              ))}
            </div>
          ))}
        </div>

          <div className="flex justify-center">
             <button
              onClick={resetGame}
              disabled={!connected}
              className="px-6 py-3 bg-gradient-to-r from-purple-500 to-blue-500 text-white font-semibold rounded-lg shadow-md hover:from-purple-600 hover:to-blue-600 transition-all duration-200 disabled:opacity-50 disabled:cursor-not-allowed"
            >
              New Game
            </button>
          </div>
       
        </div>
    )
}