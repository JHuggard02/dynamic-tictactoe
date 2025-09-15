import { StrictMode } from 'react'
import { createRoot } from 'react-dom/client'
import './index.css'
import Tictactoe from './pages/tictactoe'

createRoot(document.getElementById('root')).render(
  <StrictMode>
    <Tictactoe />
  </StrictMode>,
)
