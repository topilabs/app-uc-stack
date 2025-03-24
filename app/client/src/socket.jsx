import { io } from 'socket.io-client';

// Socket.IO connection to the Node.js server (legacy)
const URL = process.env.NODE_ENV === 'production' ? undefined : '127.0.0.1:3000';
export const socket = io(URL, {
    autoConnect: false
});

// WebSocket connection to the Pico
const PICO_WS_URL = 'ws://192.168.7.1:9000';
let picoWebSocket = null;

// Connect to the Pico WebSocket server
export function connectToPico(onMessage, onConnect, onDisconnect) {
    if (picoWebSocket) {
        picoWebSocket.close();
    }
    
    picoWebSocket = new WebSocket(PICO_WS_URL);
    
    picoWebSocket.onopen = () => {
        console.log('WebSocket connection established with Pico');
        if (onConnect) onConnect();
    };
    
    picoWebSocket.onmessage = (message) => {
        // Handle binary data (Protocol Buffer)
        if (message.data instanceof Blob) {
            const reader = new FileReader();
            reader.onload = () => {
                const arrayBuffer = reader.result;
                // Process the Protocol Buffer message
                if (onMessage) onMessage(new Uint8Array(arrayBuffer));
            };
            reader.readAsArrayBuffer(message.data);
        }
    };
    
    picoWebSocket.onclose = () => {
        console.log('WebSocket connection closed');
        if (onDisconnect) onDisconnect();
        picoWebSocket = null;
    };
    
    picoWebSocket.onerror = (error) => {
        console.error('WebSocket error:', error);
    };
    
    return picoWebSocket;
}

// Send data to the Pico
export function sendToPico(data) {
    if (picoWebSocket && picoWebSocket.readyState === WebSocket.OPEN) {
        picoWebSocket.send(data);
        return true;
    }
    return false;
}

// Check if connected to Pico
export function isConnectedToPico() {
    return picoWebSocket && picoWebSocket.readyState === WebSocket.OPEN;
}

// Disconnect from Pico
export function disconnectFromPico() {
    if (picoWebSocket) {
        picoWebSocket.close();
        picoWebSocket = null;
    }
}
