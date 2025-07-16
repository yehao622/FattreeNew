import express from 'express';
import cors from 'cors';
import helmet from 'helmet';

console.log('🚀 Starting HPC Simulation API Gateway...');

const app = express();
const port = parseInt(process.env.PORT || '3000', 10);

console.log(`📡 Configuring server on port ${port}`);

// Basic middleware
app.use(helmet());
app.use(cors());
app.use(express.json());

console.log('🔧 Middleware configured');

// Health check endpoint (simplest possible)
app.get('/api/health', (req, res) => {
  console.log('💓 Health check requested');
  res.json({
    status: 'healthy',
    timestamp: new Date().toISOString(),
    version: '1.0.0',
    environment: process.env.NODE_ENV || 'development',
    database: 'not_tested_yet',
    redis: 'not_tested_yet'
  });
});

// Root endpoint
app.get('/', (req, res) => {
  console.log('🏠 Root endpoint requested');
  res.json({
    message: 'HPC Simulation Platform API',
    status: 'running',
    timestamp: new Date().toISOString()
  });
});

// Simple test endpoint
app.get('/api/test', (req, res) => {
  console.log('🧪 Test endpoint requested');
  res.json({
    message: 'API Gateway is working!',
    environment: {
      NODE_ENV: process.env.NODE_ENV,
      PORT: process.env.PORT,
      DATABASE_URL: process.env.DATABASE_URL ? 'configured' : 'not_configured',
      REDIS_URL: process.env.REDIS_URL ? 'configured' : 'not_configured'
    }
  });
});

// 404 handler
app.use((req, res) => {
  console.log(`❌ 404 - Route not found: ${req.method} ${req.path}`);
  res.status(404).json({ error: 'Route not found' });
});

// Error handler
app.use((err: any, req: any, res: any, next: any) => {
  console.error('💥 Server error:', err);
  res.status(500).json({ error: 'Internal server error' });
});

// Start server
const server = app.listen(port, '0.0.0.0', () => {
  console.log(`✅ HPC Simulation API running on port ${port}`);
  console.log(`📚 Health check: http://localhost:${port}/api/health`);
  console.log(`🏠 Home: http://localhost:${port}/`);
  console.log(`🧪 Test: http://localhost:${port}/api/test`);
});

// Graceful shutdown
process.on('SIGTERM', () => {
  console.log('🛑 SIGTERM received, shutting down gracefully');
  server.close(() => {
    console.log('✅ Process terminated');
  });
});

export default app;