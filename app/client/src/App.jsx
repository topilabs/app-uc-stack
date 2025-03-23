import { useState, useEffect } from 'react'
import './App.css'


// A lot of code pattern is borrowed from:
// https://socket.io/how-to/use-with-react
import { socket } from './socket';
import { ConnectionManager } from './components/connectionManager';
import { PortManager } from './components/portManager';
import { ShowData } from './components/showData';

/*
outgoing events:
getPortList
setPort(path)

incomming events:
status: targetPortPath, err, connected, linkValid, availablePorts
data
*/

function App() {
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

  return (
    <div className="App">
      <ConnectionManager isConnected={ isConnected }/>
      <p></p>
      <PortManager status={ status } />
      <ShowData data={ data } />
    </div>
  )
}

export default App
