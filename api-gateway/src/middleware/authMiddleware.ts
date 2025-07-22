// api-gateway/src/middleware/authMiddleware.ts - FIXED VERSION
import { Request, Response, NextFunction } from 'express';

// Use require() to bypass TypeScript JWT type issues
const jwt = require('jsonwebtoken');

declare global {
  namespace Express {
    interface Request {
      user?: {
        userId: number;
        email: string;
      };
    }
  }
}

export interface AuthenticatedRequest extends Request {
  user: {
    userId: number;
    email: string;
  };
}

export const authenticateToken = async (
  req: Request,
  res: Response,
  next: NextFunction
): Promise<void> => {
  try {
    const authHeader = req.headers.authorization;
    const token = authHeader && authHeader.split(' ')[1];

    if (!token) {
      res.status(401).json({
        error: 'Unauthorized',
        message: 'Access token is required'
      });
      return;
    }

    const jwtSecret = process.env.JWT_SECRET;
    if (!jwtSecret) {
      console.error('💥 JWT_SECRET not configured');
      res.status(500).json({
        error: 'Internal server error',
        message: 'Authentication service not properly configured'
      });
      return;
    }

    try {
      const decoded: any = jwt.verify(token, jwtSecret);

      req.user = {
        userId: decoded.userId,
        email: decoded.email
      };

      console.log(`🔓 Authenticated user: ${decoded.email} (ID: ${decoded.userId})`);
      next();

    } catch (jwtError: any) {
      if (jwtError.name === 'TokenExpiredError') {
        res.status(401).json({
          error: 'Token expired',
          message: 'Access token has expired. Please login again.'
        });
        return;
      }

      if (jwtError.name === 'JsonWebTokenError') {
        res.status(401).json({
          error: 'Invalid token',
          message: 'Access token is invalid'
        });
        return;
      }

      throw jwtError;
    }

  } catch (error: any) {
    console.error('💥 Authentication middleware error:', error);
    res.status(500).json({
      error: 'Internal server error',
      message: 'Authentication failed'
    });
  }
};

// Rate limiting middleware for auth endpoints
export const authRateLimit = (req: Request, res: Response, next: NextFunction) => {
  const ip = req.ip || req.connection.remoteAddress || 'unknown';
  
  console.log(`🚦 Auth rate limit check for IP: ${ip}`);
  
  // Simple pass-through for now - in production use express-rate-limit
  next();
};

// Optional authentication middleware
export const optionalAuth = async (
  req: Request,
  res: Response,
  next: NextFunction
): Promise<void> => {
  try {
    const authHeader = req.headers.authorization;
    const token = authHeader && authHeader.split(' ')[1];

    if (!token) {
      next();
      return;
    }

    const jwtSecret = process.env.JWT_SECRET;
    if (!jwtSecret) {
      next();
      return;
    }

    try {
      const decoded: any = jwt.verify(token, jwtSecret);
      req.user = {
        userId: decoded.userId,
        email: decoded.email
      };
      console.log(`🔓 Optional auth - user: ${decoded.email}`);
    } catch (tokenError) {
      console.log('🔒 Optional auth - no valid token');
    }

    next();

  } catch (error) {
    console.error('💥 Optional auth middleware error:', error);
    next();
  }
};

// Role-based authorization middleware
export const requireRole = (allowedRoles: string[]) => {
  return async (req: Request, res: Response, next: NextFunction): Promise<void> => {
    try {
      const userId = req.user?.userId;

      if (!userId) {
        res.status(401).json({
          error: 'Unauthorized',
          message: 'Authentication required'
        });
        return;
      }

      const { Pool } = require('pg');
      const pool = new Pool({
        connectionString: process.env.DATABASE_URL
      });

      const result = await pool.query(
        'SELECT role FROM users WHERE id = $1',
        [userId]
      );

      if (result.rows.length === 0) {
        res.status(404).json({
          error: 'User not found',
          message: 'User account not found'
        });
        return;
      }

      const userRole = result.rows[0].role;

      if (!allowedRoles.includes(userRole)) {
        res.status(403).json({
          error: 'Forbidden',
          message: `Access denied. Required role: ${allowedRoles.join(' or ')}`
        });
        return;
      }

      console.log(`✅ Role check passed: ${userRole} in [${allowedRoles.join(', ')}]`);
      next();

    } catch (error) {
      console.error('💥 Role authorization error:', error);
      res.status(500).json({
        error: 'Internal server error',
        message: 'Authorization check failed'
      });
    }
  };
};