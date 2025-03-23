import { io } from 'socket.io-client';

// "undefined" means the URL will be computed from the `window.location` object

// Client connects to server located at localhost:3000

const URL = process.env.NODE_ENV === 'production' ? undefined : '127.0.0.1:3000';

export const socket = io(URL, {
    autoConnect: false
  });