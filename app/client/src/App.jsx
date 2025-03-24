import { useState, useEffect } from 'react';
import './App.css';

// Socket.IO for legacy connection
import { socket } from './socket';

// Components
import { ConnectionManager } from './components/connectionManager';
import { PortManager } from './components/portManager';
import { ShowData } from './components/showData';
import { PicoConnectionManager } from './components/PicoConnectionManager';

// Protocol Buffers
import * as protobuf from 'protobufjs';

function App() {
  // Legacy Socket.IO state
  const initStatus = {
    targetPortPath: null,
    err: null,
    connected: false,
    linkValid: false,
    availablePorts: ['None']
  };
  const [isConnected, setIsConnected] = useState(socket.connected);
  const [status, setStatus] = useState(initStatus);
  const [data, setData] = useState([]);
  
  // Pico WebSocket state
  const [picoConnected, setPicoConnected] = useState(false);
  const [picoData, setPicoData] = useState(null);
  const [protoRoot, setProtoRoot] = useState(null);
  const [decodedProtoData, setDecodedProtoData] = useState(null);

  // Load Protocol Buffer definition
  useEffect(() => {
    // Create a simple proto definition matching the one on the Pico
    const protoDefinition = `
      syntax = "proto3";
      message DataPackage {
        uint32 potentiometer = 1;
        uint32 generator = 2;
      }
    `;
    
    try {
      protobuf.parse(protoDefinition, { keepCase: true }).then(root => {
        setProtoRoot(root);
        console.log("Protocol buffer definition loaded");
      });
    } catch (err) {
      console.error("Failed to parse Protocol Buffer definition:", err);
    }
  }, []);

  // Legacy Socket.IO connection
  useEffect(() => {
    function onConnect() {
      setIsConnected(true);
    }

    function onDisconnect() {
      setIsConnected(false);
    }

    function onStatusEvent(value) {
      setStatus(value);
    }

    function onDataEvent(value) {
      setData(value);
    }

    socket.on('connect', onConnect);
    socket.on('disconnect', onDisconnect);
    socket.on('status', onStatusEvent);
    socket.on('data', onDataEvent);

    return () => {
      socket.off('connect', onConnect);
      socket.off('disconnect', onDisconnect);
      socket.off('status', onStatusEvent);
      socket.off('data', onDataEvent);
    };
  }, []);

  // Handle Pico WebSocket connection events
  const handlePicoConnect = () => {
    setPicoConnected(true);
    console.log("Connected to Pico WebSocket server");
  };

  const handlePicoDisconnect = () => {
    setPicoConnected(false);
    setDecodedProtoData(null);
    console.log("Disconnected from Pico WebSocket server");
  };

  // Handle Pico WebSocket messages (Protocol Buffer data)
  const handlePicoMessage = (data) => {
    setPicoData(data);
    
    // Decode Protocol Buffer message if root is available
    if (protoRoot) {
      try {
        const DataPackage = protoRoot.lookupType("DataPackage");
        const message = DataPackage.decode(data);
        setDecodedProtoData(message);
        console.log("Decoded Protocol Buffer message:", message);
      } catch (err) {
        console.error("Failed to decode Protocol Buffer message:", err);
      }
    }
  };

  return (
    <div className="App">
      <h1>Microcontroller WebSocket Demo</h1>
      
      {/* Pico WebSocket Connection */}
      <div className="connection-section">
        <h2>Direct Pico Connection</h2>
        <PicoConnectionManager 
          onConnect={handlePicoConnect}
          onDisconnect={handlePicoDisconnect}
          onMessage={handlePicoMessage}
        />
        
        {picoConnected && (
          <div className="data-section">
            <h3>Pico Data</h3>
            {decodedProtoData ? (
              <div>
                <p>Potentiometer: {decodedProtoData.potentiometer}</p>
                <p>Generator: {decodedProtoData.generator}</p>
              </div>
            ) : (
              <p>Waiting for data...</p>
            )}
          </div>
        )}
      </div>
      
      <hr />
      
      {/* Legacy Connection */}
      <div className="legacy-section">
        <h2>Legacy Connection</h2>
        <ConnectionManager isConnected={isConnected}/>
        <p></p>
        <PortManager status={status} />
        <ShowData data={data} />
      </div>
    </div>
  );
}

export default App
