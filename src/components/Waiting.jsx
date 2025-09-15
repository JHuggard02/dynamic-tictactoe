import ActionButton from "./Actionbutton";
import PlayersTable from "./PlayersTable";

export default function Waiting({startGame, players}){

    return (
        <div className="text-center text-black flex flex-col items-center space-y-4 justify-center">
            <h2>Waiting To Start...</h2>

            <PlayersTable players={players} />

            <ActionButton onClick={startGame}>Start</ActionButton>
        </div>
    )
}