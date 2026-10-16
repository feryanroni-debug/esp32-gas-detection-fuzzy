const bcrypt = require('bcryptjs');
const jwt = require('jsonwebtoken');
const db = require('../config/database');

const JWT_SECRET = process.env.JWT_SECRET || 'super_secret_gas_jwt_key_2026_change_in_production';

exports.register = (req, res) => {
    const { name, email, password, role = 'operator' } = req.body;

    if (!name || !email || !password) {
        return res.status(400).json({
            status: 'error',
            message: 'Nama, email, dan password wajib diisi.'
        });
    }

    bcrypt.hash(password, 10, (err, hash) => {
        if (err) {
            return res.status(500).json({ status: 'error', message: 'Gagal mengenkripsi kata sandi.' });
        }

        const sql = `INSERT INTO users (name, email, password_hash, role) VALUES (?, ?, ?, ?)`;
        db.run(sql, [name, email.toLowerCase(), hash, role], function (err) {
            if (err) {
                if (err.message.includes('UNIQUE')) {
                    return res.status(400).json({ status: 'error', message: 'Email sudah terdaftar.' });
                }
                return res.status(500).json({ status: 'error', message: err.message });
            }

            return res.status(201).json({
                status: 'success',
                message: 'Pengguna berhasil didaftarkan.',
                userId: this.lastID
            });
        });
    });
};

exports.login = (req, res) => {
    const { email, password } = req.body;

    if (!email || !password) {
        return res.status(400).json({ status: 'error', message: 'Email dan password wajib diisi.' });
    }

    const sql = `SELECT * FROM users WHERE email = ?`;
    db.get(sql, [email.toLowerCase()], (err, user) => {
        if (err) {
            return res.status(500).json({ status: 'error', message: err.message });
        }
        if (!user) {
            return res.status(401).json({ status: 'error', message: 'Email atau password tidak sesuai.' });
        }

        bcrypt.compare(password, user.password_hash, (err, isMatch) => {
            if (err || !isMatch) {
                return res.status(401).json({ status: 'error', message: 'Email atau password tidak sesuai.' });
            }

            const token = jwt.sign(
                { id: user.id, name: user.name, email: user.email, role: user.role },
                JWT_SECRET,
                { expiresIn: '24h' }
            );

            return res.json({
                status: 'success',
                message: 'Login berhasil.',
                token: token,
                user: {
                    id: user.id,
                    name: user.name,
                    email: user.email,
                    role: user.role
                }
            });
        });
    });
};

exports.me = (req, res) => {
    return res.json({
        status: 'success',
        user: req.user
    });
};
