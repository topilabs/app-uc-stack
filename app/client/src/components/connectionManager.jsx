import React from 'react';
import { socket } from '../socket';
import { FormControlLabel, Switch } from '@mui/material';

export function ConnectionManager({ isConnected }) {

  // reads the state of the switch
  function handleChange(event) {
    if (event.target.checked) {
      socket.connect();
    } else {
      socket.disconnect();
    }
  }

  return (
    <FormControlLabel 
    control={
    <Switch
    checked={ isConnected }
    onChange={ handleChange }
    inputProps={{ 'aria-label': 'controlled' }}
    />
    } label="Connect"
    /> 
  );
}