import http from 'http';
import { Server } from 'socket.io';
import app from './app';

const server = http.createServer(app);
export const io = new Server(server).listen(server);

io.on('connection', (socket) => {
  console.log('User connected: ', socket.id);
});

export default server;
