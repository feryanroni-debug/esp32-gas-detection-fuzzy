require('dotenv').config();
const http = require('http');
const { Server } = require('socket.io');

const app = require('./src/app');
const socketHandler = require('./src/sockets/socketHandler');

const PORT = process.env.PORT || 5050;

// Buat HTTP Server
const server = http.createServer(app);

// Inisialisasi Socket.IO
const io = new Server(server, {
    cors: {
        origin: '*',
        methods: ['GET', 'POST']
    }
});

// Simpan referensi io ke instance Express agar bisa diakses di controller
app.set('socketio', io);

// Pasang handler WebSocket
socketHandler(io);

// Jalankan Server
server.listen(PORT, () => {
    console.log(`====================================================`);
    console.log(`🚀 Server Backend aktif di http://localhost:${PORT}`);
    console.log(`📡 WebSocket siap menerima koneksi real-time`);
    console.log(`====================================================`);
});
