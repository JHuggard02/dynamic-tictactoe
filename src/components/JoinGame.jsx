import { useState } from "react";
import InputField from "./InputField";
import ActionButton from "./Actionbutton";
import BackButton from "./BackButton";

export default function JoinGame({ updateGameID, updateUsername, username, gameID, createGame, joinGame, rows, setRows, cols, setCols, winCond, setWinCond }) {
  const [mode, setMode] = useState("none"); 
  

  const boardRange = Array.from({ length: 16 }, (_, i) => i + 3); // 3-18

  const back = () => {
    setMode("none");
  }

  const NumberSelect = ({ label, value, onChange }) => (
    <div className="flex flex-col items-center mb-2">
      <label className="mb-1 font-medium text-black">{label}</label>
      <select value={value} onChange={onChange} className="border px-2 py-1 rounded w-24 text-center">
        {boardRange.map((num) => (
          <option key={num} value={num}>{num}</option>
        ))}
      </select>
    </div>
  );

  return (
    <div className="flex flex-col items-center justify-center space-y-4">
      {mode === "none" && (
        <div className="space-x-4">
          <ActionButton onClick={() => setMode("create")}>Create</ActionButton>
          <ActionButton onClick={() => setMode("join")}>Join</ActionButton>
        </div>
      )}

      {mode === "create" && (
        <div className="flex flex-col items-center space-y-4">
          <BackButton onClick={back} />
          <div className="flex flex-col items-center space-y-2">
            <InputField value={username} onChange={updateUsername} placeholder="Username" />
            <InputField value={gameID} onChange={updateGameID} placeholder="Game ID" />
          </div>

          <div className="flex space-x-4">
            <NumberSelect label="Rows" value={rows} onChange={(e) => setRows(e.target.value)} />
            <NumberSelect label="Cols" value={cols} onChange={(e) => setCols(e.target.value)} />
            <NumberSelect label="Win Condition" value={winCond} onChange={(e) => setWinCond(e.target.value)} />
          </div>

          

          <ActionButton onClick={createGame}>Create</ActionButton>
        </div>
      )}

      {mode === "join" && (
        <div className="flex flex-col items-center space-y-2">
          <BackButton onClick={back} />
          <InputField value={username} onChange={updateUsername} placeholder="Username" />
          <InputField value={gameID} onChange={updateGameID} placeholder="Game ID" />
          <ActionButton onClick={joinGame}>Join</ActionButton>
        </div>
      )}
    </div>
  );
}
