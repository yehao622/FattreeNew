import express from 'express';
import cors from 'cors';
import helmet from 'helmet';
import dotenv from 'dotenv';

// Load environment variables
dotenv.config();

const app = express();
const port = parseInt(process.env.PORT || '3000', 10);

// Basic middleware
app.use(helmet());
app.use(cors());
app.use(express.json());

// Health check endpoint
app.get('/api/health', (_req, res) => {
  res.json({
    status: 'healthy',
    timestamp: new Date().toISOString(),
    version: '1.0.0',
    environment: process.env.NODE_ENV || 'development'
  });
});

// Root endpoint
app.get('/', (_req, res) => {
  res.json({
    message: 'HPC Simulation Platform API',
    status: 'running',
    timestamp: new Date().toISOString()
  });
});

// API docs placeholder
app.get('/api/docs', (_req, res) => {
  res.json({
    message: 'HPC Simulation Platform API Documentation',
    version: '1.0.0',
    status: 'Coming soon in Session 2'
  });
});

// 404 handler
app.use((_req, res) => {
  res.status(404).json({ error: 'Route not found' });
});

// Start server
app.listen(port, () => {
  console.log(`🚀 HPC Simulation API running on port ${port}`);
  console.log(`📚 Health check: http://localhost:${port}/api/health`);
  console.log(`🏠 Home: http://localhost:${port}/`);
});

export default app;
