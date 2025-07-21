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
