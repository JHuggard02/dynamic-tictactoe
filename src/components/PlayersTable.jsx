import React from "react";

export default function PlayersTable({ players }) {
  return (
    <div className="overflow-x-auto">
      <table className="min-w-full border border-gray-300 rounded-lg">
        <thead className="bg-gray-100">
          <tr>
            <th className="px-4 py-2 border-b text-left">#</th>
            <th className="px-4 py-2 border-b text-left">Player Name</th>
          </tr>
        </thead>
        <tbody>
          {players.length > 0 ? (
            players.map((player, index) => (
              <tr
                key={index}
                className="odd:bg-white even:bg-gray-50 hover:bg-gray-100"
              >
                <td className="px-4 py-2 border-b">{index + 1}</td>
                <td className="px-4 py-2 border-b">{player}</td>
              </tr>
            ))
          ) : (
            <tr>
              <td
                colSpan="2"
                className="px-4 py-6 text-center text-gray-500 border-b"
              >
                No players yet
              </td>
            </tr>
          )}
        </tbody>
      </table>
    </div>
  );
}
