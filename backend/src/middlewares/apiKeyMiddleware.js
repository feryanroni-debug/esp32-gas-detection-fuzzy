module.exports = function apiKeyMiddleware(req, res, next) {
    const apiKey = req.headers['x-api-key'];
    const validKey = process.env.ESP32_API_KEY || 'secret_esp32_gas_token_2026';

    if (!apiKey || apiKey !== validKey) {
        return res.status(401).json({
            status: 'error',
            message: 'Akses ditolak: X-API-KEY tidak valid atau tidak disertakan.'
        });
    }

    next();
};
