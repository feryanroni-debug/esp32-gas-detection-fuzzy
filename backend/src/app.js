const express = require('express');
const cors = require('cors');
const helmet = require('helmet');
const path = require('path');

const telemetryRoutes = require('./routes/telemetryRoutes');
const authRoutes = require('./routes/authRoutes');

const app = express();

// Middleware Global
app.use(helmet({
    contentSecurityPolicy: false // Izinkan CDN Tailwind & Socket.io di frontend
}));
app.use(cors());
app.use(express.json());
app.use(express.urlencoded({ extended: true }));

// Sajikan berkas statis frontend langsung dari root folder frontend
app.use(express.static(path.resolve(__dirname, '../../frontend')));

// Rute REST API
app.use('/api/v1', telemetryRoutes);
app.use('/api/v1/auth', authRoutes);

// Health check endpoint
app.get('/api/health', (req, res) => {
    res.json({
        status: 'ok',
        uptime: process.uptime(),
        timestamp: new Date().toISOString()
    });
});

module.exports = app;
