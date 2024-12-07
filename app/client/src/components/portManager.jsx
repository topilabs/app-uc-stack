import React, { useState } from 'react';
import { socket } from '../socket';

import Select from '@mui/material/Select';
import MenuItem from '@mui/material/MenuItem';
import InputLabel from '@mui/material/InputLabel';
import FormControl from '@mui/material/FormControl';

import { Button } from '@mui/material';

export function PortManager({ status }) {
  const getInitialSelectValue = () => {
    const value = status.availablePorts[0];
    return value;
  };

  const [selectValue, setSelectValue] = useState(getInitialSelectValue);
  const [isLoading, setIsLoading] = useState(false);

  const handleChange = (event) => {
    setSelectValue(event.target.value);
    console.log("selectValue:", event.target.value);
    sendPortPath(event.target.value)
  };

  // SHOULD QUERY AVAILABLE PORTS ON CONNECTION, THEN SOMEHOW AUTO-UPDATE THAT LIST.
  // Could be triggered once a second, or maybe serial can be aware of hot-plugged device events
  
  function getPortList() {
      socket.timeout(5000).emit('getPortList', 0, (err, val) => {
        setIsLoading(false); // waits for an ack
        console.log("getPortList acknowledged");
      });
  }

  function sendPortPath(path) {
    console.log("sending port path:", path);
    socket.timeout(5000).emit('setPort', path, (err, val) => {
      console.log("setPort acknowledged");
    });
  } 

  return (
    <>
    <div>
      <Button variant="outlined" onClick={ getPortList }> Update Ports </Button>
      <FormControl sx={{ m: 1, minWidth: 120 }} size="small">
      <InputLabel id="select-port-label"> Port </InputLabel>
      
      <Select
        labelId="select-port-label"
        disabled={status.availablePorts.length === 1 && status.availablePorts[0] === "None"}
        label="Port"
        onChange={ handleChange }
      >
        {
        status.availablePorts.map(port => 
        <MenuItem value={port} key={port}>{port}</MenuItem>
        )}
      </Select>
      </FormControl>
    </div>

    <pre>
      { JSON.stringify(status, null, 2) }
    </pre>
  </>
  );
}