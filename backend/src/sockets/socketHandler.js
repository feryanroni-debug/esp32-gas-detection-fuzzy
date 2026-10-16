module.exports = function socketHandler(io) {
    io.on('connection', (socket) => {
        console.log(`🔌 Klien terhubung ke WebSocket: ${socket.id}`);

        socket.on('disconnect', () => {
            console.log(`❌ Klien terputus dari WebSocket: ${socket.id}`);
        });

        // Event permintaan uji alarm manual dari web
        socket.on('test_buzzer', (data) => {
            console.log('🔔 Uji alarm diterima dari web:', data);
            io.emit('alarm_triggered', {
                testedBy: socket.id,
                timestamp: new Date().toISOString()
            });
        });
    });
};
