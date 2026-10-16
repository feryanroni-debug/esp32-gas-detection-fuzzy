const express = require('express');
const router = express.Router();
const telemetryController = require('../controllers/telemetryController');
const apiKeyMiddleware = require('../middlewares/apiKeyMiddleware');

// Endpoint Ingest dari ESP32 (dilindungi API Key)
router.post('/telemetry', apiKeyMiddleware, telemetryController.saveTelemetry);

// Endpoint Pengambilan Data (Publik / Web Dashboard)
router.get('/telemetry/latest', telemetryController.getLatestTelemetry);
router.get('/telemetry/history', telemetryController.getTelemetryHistory);
router.get('/telemetry/alarms', telemetryController.getAlarmEvents);

module.exports = router;
