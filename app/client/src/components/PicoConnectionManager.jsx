import React, { useState } from 'react';
import { connectToPico, disconnectFromPico, isConnectedToPico } from '../socket';
import { Button, Typography, Box, CircularProgress } from '@mui/material';

export function PicoConnectionManager({ onConnect, onDisconnect, onMessage }) {
  const [connecting, setConnecting] = useState(false);
  const [connected, setConnected] = useState(false);
  const [error, setError] = useState(null);

  const handleConnect = () => {
    setConnecting(true);
    setError(null);
    
    try {
      connectToPico(
        // onMessage callback
        (data) => {
          if (onMessage) onMessage(data);
        },
        // onConnect callback
        () => {
          setConnecting(false);
          setConnected(true);
          if (onConnect) onConnect();
        },
        // onDisconnect callback
        () => {
          setConnecting(false);
          setConnected(false);
          if (onDisconnect) onDisconnect();
        }
      );
    } catch (err) {
      setConnecting(false);
      setError(`Connection error: ${err.message}`);
    }
  };

  const handleDisconnect = () => {
    disconnectFromPico();
    setConnected(false);
  };

  return (
    <Box sx={{ mb: 3 }}>
      <Typography variant="h6">Pico WebSocket Connection</Typography>
      
      <Box sx={{ display: 'flex', alignItems: 'center', gap: 2, mb: 1 }}>
        <Typography>
          Status: {connected ? 'Connected' : (connecting ? 'Connecting...' : 'Disconnected')}
        </Typography>
        
        {!connected && !connecting && (
          <Button 
            variant="contained" 
            color="primary"
            onClick={handleConnect}
          >
            Connect to Pico
          </Button>
        )}
        
        {connecting && (
          <CircularProgress size={24} />
        )}
        
        {connected && (
          <Button 
            variant="contained" 
            color="secondary"
            onClick={handleDisconnect}
          >
            Disconnect
          </Button>
        )}
      </Box>
      
      {error && (
        <Typography color="error">{error}</Typography>
      )}
    </Box>
  );
}
