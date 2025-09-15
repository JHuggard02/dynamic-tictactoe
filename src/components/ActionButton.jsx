 export default function ActionButton ({ children, onClick }){
    return (
        <button
        className="px-6 py-3 bg-gradient-to-r from-purple-500 to-blue-500 text-white font-semibold rounded-lg shadow-md hover:from-purple-600 hover:to-blue-600 transition-all duration-200 disabled:opacity-50 disabled:cursor-not-allowed"
        onClick={onClick}
        >
        {children}
        </button>
    )
 }