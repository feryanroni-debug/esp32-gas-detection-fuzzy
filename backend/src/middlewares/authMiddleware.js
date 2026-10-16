const jwt = require('jsonwebtoken');

module.exports = function authMiddleware(req, res, next) {
    const authHeader = req.headers['authorization'];
    if (!authHeader || !authHeader.startsWith('Bearer ')) {
        return res.status(401).json({
            status: 'error',
            message: 'Token otentikasi tidak ditemukan. Harap login terlebih dahulu.'
        });
    }

    const token = authHeader.split(' ')[1];
    const jwtSecret = process.env.JWT_SECRET || 'super_secret_gas_jwt_key_2026_change_in_production';

    try {
        const decoded = jwt.verify(token, jwtSecret);
        req.user = decoded;
        next();
    } catch (err) {
        return res.status(403).json({
            status: 'error',
            message: 'Token otentikasi kedaluwarsa atau tidak valid.'
        });
    }
};
