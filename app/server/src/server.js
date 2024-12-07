const express = require('express');
const http = require('http');
const { Server } = require('socket.io');
const { SerialPort } = require('serialport');
const { DelimiterParser } = require('@serialport/parser-delimiter');
const protobuf = require('protobufjs');
const path = require('path');

const app = express();
const server = http.createServer(app);

const { encode, decode } = require('./cobs/index.js');

const listenPort = 3000;

const status = {
  timeStamp: Date.now(),
  targetPortPath: null,
  err: null,
  connected: false,
  linkValid: false,
  availablePorts: []
}

protobuf.load('src/message.proto', function(err, root) {
  if (err) {
    throw err;
  }
  console.log(err);

  var DataPackage = root.lookupType("DataPackage");

  var payload = { 
    "potentiometer": 1,
    "generator": 2
  };

  var errMsg = DataPackage.verify(payload);
  
  if (errMsg) {
    throw Error(errMsg);
  }

  // // Create a new message
  // var message = DataPackage.create(payload); // or use .fromObject if conversion is necessary

  // // Encode a message to an Uint8Array (browser) or Buffer (node)
  // var buffer = DataPackage.encode(message).finish();

  // // Decode an Uint8Array (browser) or Buffer (node) to a message
  // var message = DataPackage.decode(buffer);

});

/*
Functionality:
- Not configured / configured
- Running / Maintenance
- Running:
    Serialport connected and recieving data, broadcasting data out on websockets
    Source (serial port)
    Destination (websocket, grafana, influx, sql etc...)
    Re-attempt serial connection. 

    Commands:
      listports
      setportpath
      listprotofiles (find path...)
      set .protopath

    status: connected, disconnected, path, error

    errors
      no path specified
      no .protofile
      bad data

*/

// need to specify the connecting client address:port in the server
const io = new Server(server, {
  cors: {
    origin: "http://localhost:5173"
  }
});

// const port = new SerialPort({ 
//   path: "/",
//   baudRate: 112500,
//   autoOpen: false, 
// });

app.get('/', (req, res) => {
  res.sendFile(__dirname + '/index.html');
});

server.listen(listenPort, () => {
  console.log('server listening on *:', listenPort);
});

io.on('connection', (socket) => {
  console.log('a client connected');
  
  socket.on("getPortList", async (arg, ackServer) => {
    console.log("recieved an arg for cfg event - arg:", arg);
    var ports = await listSerialPorts();
    updateStatus();
    ackServer(0); // returns an ack to the server
  });

  socket.on("setPort", (data, ackServer) => {
    console.log("setPortPath:", data);
    status.targetPortPath = data;
    ackServer(0); // returns an ack to the server
    openPort();
  });

  socket.on("disconnect", (reason) => {
    console.log('client disconnected - reason:',reason);
  });
});

function openPort() {
  port = new SerialPort({ 
    path: status.targetPortPath,
    baudRate: 112500,
  });

  port.on('open', () => {
    console.log('port opened');
    status.connected = true;
    status.err = null;
    parser = port.pipe(new DelimiterParser({ delimiter: [0x00], includeDelimiter: false }))
    parser.on('data', onDataReceived);
    updateStatus(); 
  });
  
  port.on('error', (err) => {
    console.log('Error: ', err.message);
    status.err = err.message;
    status.connected = false;
    updateStatus();
  });
}

function updateStatus() {
  status.timeStamp = Date.now();
  io.sockets.emit("status", status);
}

function onDataReceived (data) {
  console.log('Data:', data);
  decoded = decode(data);
  console.log('decoded:', decoded);
  io.sockets.emit("data", decoded);
  protobuf.load('src/message.proto', function(err, root) {
    if (err) {
      console.error('Failed to load proto file:', err);
      return;
    }

    var DataPackage = root.lookupType("DataPackage");

    try {
      var message = DataPackage.decode(decoded);
      console.log('Decoded DataPackage:', message);
      // io.sockets.emit("dataPackage", message);
    } catch (decodeErr) {
      console.error('Failed to decode DataPackage:', decodeErr);
    }
  });
}

function listSerialPorts() {
  return SerialPort.list().then((ports, err) => {
    if(err) {
      console.log("Error accessing serial port - error message: ",err.message);
      return
    } else {
      // no error
    }
    
    if (ports.length === 0) {
      // is this an error?
      console.log("No ports discovered");
    }
    
    // Do something with the port list that was discovered
    for (let i=0; i<ports.length; i++) {
      // Something
      console.log("port discovered: [", String(i), "] ", ports[i].path)
      status.availablePorts[i]=ports[i].path;
    }
    return ports;
  })
}