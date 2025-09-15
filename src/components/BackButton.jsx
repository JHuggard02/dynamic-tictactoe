export default function BackButton({ onClick }) {
  return (
    <button
      onClick={onClick}
      className="px-4 py-2 bg-gray-200 text-gray-700 font-medium rounded-lg shadow-sm hover:bg-gray-300 transition-colors duration-200"
    >
      ← Back
    </button>
  );
}
