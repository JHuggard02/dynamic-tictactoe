  export default function InputField({ value, onChange, placeholder }){
    return (
        <input
        type="text"
        value={value}
        onChange={onChange}
        placeholder={placeholder}
        className="border px-2 py-1 rounded mb-2 w-64"
        />
    )
  }