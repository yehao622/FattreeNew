// api-gateway/src/routes/authRoutes.ts
import { Router } from 'express';
import { register, login, getProfile, refreshToken } from '../controllers/authController';
import { authenticateToken, authRateLimit } from '../middleware/authMiddleware';

const router = Router();

/**
 * @route   POST /api/v1/auth/register
 * @desc    Register a new user
 * @access  Public
 * @body    { email, username, password, firstName, lastName, organization? }
 */
router.post('/register', authRateLimit, register);

/**
 * @route   POST /api/v1/auth/login
 * @desc    Login user and get JWT token
 * @access  Public
 * @body    { email, password }
 */
router.post('/login', authRateLimit, login);

/**
 * @route   GET /api/v1/auth/profile
 * @desc    Get current user profile
 * @access  Private
 * @headers Authorization: Bearer <token>
 */
router.get('/profile', authenticateToken, getProfile);

/**
 * @route   POST /api/v1/auth/refresh
 * @desc    Refresh JWT token
 * @access  Private
 * @headers Authorization: Bearer <token>
 */
router.post('/refresh', authenticateToken, refreshToken);

/**
 * @route   POST /api/v1/auth/logout
 * @desc    Logout user (client-side token removal)
 * @access  Private
 * @note    For stateless JWT, logout is handled client-side
 */
router.post('/logout', authenticateToken, (req, res) => {
  console.log('🚪 User logged out:', req.user?.email);
  res.status(200).json({
    message: 'Logout successful',
    note: 'Please remove the token from client storage'
  });
});

export default router;
