#!/bin/bash
# Final Working Fix - JWT TypeScript Workaround

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

echo -e "${BLUE}🔧 Final JWT TypeScript Workaround${NC}"
echo "=================================="

# Stop containers
log_info "Stopping containers..."
docker-compose down

# Remove problematic files
log_info "Cleaning up..."
rm -f api-gateway/src/controllers/authController.ts
rm -f api-gateway/src/middleware/authMiddleware.ts

# Ensure directories exist
mkdir -p api-gateway/src/{controllers,middleware,routes}

# Create JWT workaround auth controller
log_info "Creating JWT workaround auth controller..."
cat > api-gateway/src/controllers/authController.ts << 'AUTHEOF'
import { Request, Response } from 'express';
import bcrypt from 'bcryptjs';
import { Pool } from 'pg';
import Joi from 'joi';

// Use require() to completely bypass TypeScript JWT type issues
const jwt = require('jsonwebtoken');

let dbPool: Pool;

const getDbPool = (): Pool => {
  if (!dbPool) {
    dbPool = new Pool({
      connectionString: process.env.DATABASE_URL
    });
  }
  return dbPool;
};

const registerSchema = Joi.object({
  email: Joi.string().email().required(),
  username: Joi.string().alphanum().min(3).max(30).required(),
  password: Joi.string().min(8).required(),
  firstName: Joi.string().min(1).max(100).required(),
  lastName: Joi.string().min(1).max(100).required(),
  organization: Joi.string().max(255).optional()
});

const loginSchema = Joi.object({
  email: Joi.string().email().required(),
  password: Joi.string().required()
});

// Generate JWT token using require() to bypass TypeScript types
const generateToken = (userId: number, email: string): string => {
  const jwtSecret = process.env.JWT_SECRET;
  if (!jwtSecret) {
    throw new Error('JWT_SECRET environment variable is not set');
  }
  
  const payload: any = { userId, email };
  const options: any = { expiresIn: '24h' };
  
  return jwt.sign(payload, jwtSecret, options);
};

export const register = async (req: Request, res: Response): Promise<void> => {
  try {
    console.log('🔐 User registration attempt:', req.body.email);

    const { error, value } = registerSchema.validate(req.body);
    if (error) {
      res.status(400).json({
        error: 'Validation failed',
        details: error.details.map(detail => detail.message)
      });
      return;
    }

    const { email, username, password, firstName, lastName, organization } = value;

    const pool = getDbPool();
    const existingUser = await pool.query(
      'SELECT id FROM users WHERE email = $1 OR username = $2',
      [email, username]
    );

    if (existingUser.rows.length > 0) {
      res.status(409).json({
        error: 'User already exists',
        message: 'Email or username is already registered'
      });
      return;
    }

    const passwordHash = await bcrypt.hash(password, 12);

    const result = await pool.query(
      `INSERT INTO users (email, username, password_hash, first_name, last_name, organization, role)
       VALUES ($1, $2, $3, $4, $5, $6, $7) 
       RETURNING id, email, username, first_name, last_name, organization, role, created_at`,
      [email, username, passwordHash, firstName, lastName, organization || null, 'researcher']
    );

    const newUser = result.rows[0];
    const token = generateToken(newUser.id, newUser.email);

    console.log('✅ User registered successfully:', newUser.email);

    res.status(201).json({
      message: 'User registered successfully',
      user: {
        id: newUser.id,
        email: newUser.email,
        username: newUser.username,
        firstName: newUser.first_name,
        lastName: newUser.last_name,
        organization: newUser.organization,
        role: newUser.role,
        createdAt: newUser.created_at
      },
      token
    });

  } catch (error) {
    console.error('💥 Registration error:', error);
    res.status(500).json({
      error: 'Internal server error',
      message: 'Failed to register user'
    });
  }
};

export const login = async (req: Request, res: Response): Promise<void> => {
  try {
    console.log('🔑 Login attempt:', req.body.email);

    const { error, value } = loginSchema.validate(req.body);
    if (error) {
      res.status(400).json({
        error: 'Validation failed',
        details: error.details.map(detail => detail.message)
      });
      return;
    }

    const { email, password } = value;

    const pool = getDbPool();
    const result = await pool.query(
      `SELECT id, email, username, password_hash, first_name, last_name, 
              organization, role, is_active, created_at 
       FROM users WHERE email = $1`,
      [email]
    );

    if (result.rows.length === 0) {
      res.status(401).json({
        error: 'Authentication failed',
        message: 'Invalid email or password'
      });
      return;
    }

    const user = result.rows[0];

    if (!user.is_active) {
      res.status(401).json({
        error: 'Account disabled',
        message: 'Your account has been deactivated'
      });
      return;
    }

    const isValidPassword = await bcrypt.compare(password, user.password_hash);
    if (!isValidPassword) {
      res.status(401).json({
        error: 'Authentication failed',
        message: 'Invalid email or password'
      });
      return;
    }

    const token = generateToken(user.id, user.email);

    console.log('✅ User logged in successfully:', user.email);

    res.status(200).json({
      message: 'Login successful',
      user: {
        id: user.id,
        email: user.email,
        username: user.username,
        firstName: user.first_name,
        lastName: user.last_name,
        organization: user.organization,
        role: user.role,
        createdAt: user.created_at
      },
      token
    });

  } catch (error) {
    console.error('💥 Login error:', error);
    res.status(500).json({
      error: 'Internal server error',
      message: 'Failed to authenticate user'
    });
  }
};

export const getProfile = async (req: Request, res: Response): Promise<void> => {
  try {
    const userId = (req as any).user?.userId;
    
    if (!userId) {
      res.status(401).json({
        error: 'Unauthorized',
        message: 'Invalid or missing authentication token'
      });
      return;
    }

    console.log('👤 Profile request for user:', userId);

    const pool = getDbPool();
    const result = await pool.query(
      `SELECT id, email, username, first_name, last_name, organization, 
              role, is_active, created_at, updated_at
       FROM users WHERE id = $1`,
      [userId]
    );

    if (result.rows.length === 0) {
      res.status(404).json({
        error: 'User not found',
        message: 'User profile not found'
      });
      return;
    }

    const user = result.rows[0];

    const statsResult = await pool.query(
      `SELECT 
         COUNT(*) as total_jobs,
         COUNT(CASE WHEN status = 'completed' THEN 1 END) as completed_jobs,
         COUNT(CASE WHEN status = 'running' THEN 1 END) as running_jobs,
         COUNT(CASE WHEN status = 'failed' THEN 1 END) as failed_jobs
       FROM simulation_jobs WHERE user_id = $1`,
      [userId]
    );

    const stats = statsResult.rows[0];

    res.status(200).json({
      user: {
        id: user.id,
        email: user.email,
        username: user.username,
        firstName: user.first_name,
        lastName: user.last_name,
        organization: user.organization,
        role: user.role,
        isActive: user.is_active,
        createdAt: user.created_at,
        updatedAt: user.updated_at
      },
      statistics: {
        totalJobs: parseInt(String(stats.total_jobs), 10),
        completedJobs: parseInt(String(stats.completed_jobs), 10),
        runningJobs: parseInt(String(stats.running_jobs), 10),
        failedJobs: parseInt(String(stats.failed_jobs), 10)
      }
    });

  } catch (error) {
    console.error('💥 Profile error:', error);
    res.status(500).json({
      error: 'Internal server error',
      message: 'Failed to fetch user profile'
    });
  }
};

export const refreshToken = async (req: Request, res: Response): Promise<void> => {
  try {
    const userId = (req as any).user?.userId;
    const userEmail = (req as any).user?.email;
    
    if (!userId || !userEmail) {
      res.status(401).json({
        error: 'Unauthorized',
        message: 'Invalid authentication token'
      });
      return;
    }

    const newToken = generateToken(userId, userEmail);

    console.log('🔄 Token refreshed for user:', userEmail);

    res.status(200).json({
      message: 'Token refreshed successfully',
      token: newToken
    });

  } catch (error) {
    console.error('💥 Token refresh error:', error);
    res.status(500).json({
      error: 'Internal server error',
      message: 'Failed to refresh token'
    });
  }
};
AUTHEOF

# Create middleware with same workaround
log_info "Creating JWT workaround middleware..."
cat > api-gateway/src/middleware/authMiddleware.ts << 'MIDEOF'
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

    const decoded: any = jwt.verify(token, jwtSecret);

    req.user = {
      userId: decoded.userId,
      email: decoded.email
    };

    console.log(`🔓 Authenticated user: ${decoded.email} (ID: ${decoded.userId})`);
    
    next();

  } catch (error: any) {
    if (error.name === 'TokenExpiredError') {
      res.status(401).json({
        error: 'Token expired',
        message: 'Access token has expired. Please login again.'
      });
      return;
    }

    if (error.name === 'JsonWebTokenError') {
      res.status(401).json({
        error: 'Invalid token',
        message: 'Access token is invalid'
      });
      return;
    }

    console.error('💥 Authentication middleware error:', error);
    res.status(500).json({
      error: 'Internal server error',
      message: 'Authentication failed'
    });
  }
};

// Rate limiting middleware for auth endpoints - REQUIRED EXPORT
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
MIDEOF

# Build and start
log_info "Building and starting..."
docker-compose build --no-cache api-gateway
docker-compose up -d postgres redis
sleep 10
docker-compose up -d api-gateway
sleep 15

# Test
log_info "Testing endpoints..."
if curl -s http://localhost:3000/api/health | grep -q "healthy"; then
    log_success "✅ API is working!"
    
    echo
    echo "Test registration:"
    echo 'curl -X POST http://localhost:3000/api/v1/auth/register \'
    echo '  -H "Content-Type: application/json" \'
    echo '  -d '"'"'{"email":"test@example.com","username":"testuser","password":"testpass123","firstName":"Test","lastName":"User"}'"'"
    
    echo
    echo "Run full test suite:"
    echo "./api_test_script.sh"
    
else
    log_warning "API not responding yet. Check logs:"
    echo "docker-compose logs api-gateway"
fi

echo
echo -e "${GREEN}🎉 JWT TypeScript workaround applied!${NC}"
echo
echo "Key changes made:"
echo "• Used require('jsonwebtoken') instead of import"
echo "• Added 'any' types to bypass TypeScript checking"
echo "• Simplified token generation with explicit object creation"
echo
echo "This should completely resolve the JWT type issues!"