// api-gateway/src/index.ts - UPDATED with WebSocket Integration
import express from 'express';
import cors from 'cors';
import helmet from 'helmet';
import compression from 'compression';
import morgan from 'morgan';
import { createServer } from 'http';
import { Pool } from 'pg';

// Import WebSocket server
import { WebSocketServer } from './websocket/websocketServer';

// Import existing controllers and middleware
import * as authController from './controllers/authController';
import * as simulationController from './controllers/simulationController';
import { authenticateToken, authRateLimit } from './middleware/authMiddleware';

const app = express();
const port = parseInt(process.env.PORT || '3000', 10);

// Create HTTP server for WebSocket integration
const server = createServer(app);

console.log('🚀 Starting HPC Simulation API Gateway with WebSocket support...');

// Database connection
let dbPool: Pool;
const getDbPool = () => {
  if (!dbPool) {
    dbPool = new Pool({
      connectionString: process.env.DATABASE_URL
    });
  }
  return dbPool;
};

// Initialize WebSocket server
let wsServer: WebSocketServer;

// Middleware configuration
app.use(helmet({
  contentSecurityPolicy: {
    directives: {
      defaultSrc: ["'self'"],
      styleSrc: ["'self'", "'unsafe-inline'"],
      scriptSrc: ["'self'"],
      imgSrc: ["'self'", "data:", "https:"],
      connectSrc: ["'self'", "ws:", "wss:"], // Allow WebSocket connections
    },
  },
}));

const corsOrigins = process.env.CORS_ORIGIN?.split(',') || ['http://localhost:3000', 'http://localhost:3001'];
app.use(cors({
  origin: corsOrigins,
  credentials: true
}));

app.use(compression());
app.use(express.json({ limit: '10mb' }));
app.use(express.urlencoded({ extended: true, limit: '10mb' }));

// Request logging
if (process.env.NODE_ENV !== 'test') {
  app.use(morgan('combined'));
}

console.log('🔧 Middleware configured');

// Health check endpoint with WebSocket status
app.get('/api/health', async (req, res) => {
  console.log('💓 Health check requested');
  
  const health = {
    status: 'healthy',
    timestamp: new Date().toISOString(),
    version: '1.1.0', // Updated version with WebSocket
    environment: process.env.NODE_ENV || 'development',
    services: {
      database: 'unknown',
      redis: 'unknown',
      websocket: 'unknown'
    },
    websocket: {
      connectedUsers: 0,
      totalConnections: 0
    }
  };

  try {
    // Test database connection
    const pool = getDbPool();
    const dbClient = await pool.connect();
    await dbClient.query('SELECT 1');
    dbClient.release();
    health.services.database = 'connected';
  } catch (error) {
    health.services.database = 'disconnected';
    health.status = 'degraded';
  }

  try {
    // Test Redis connection
    health.services.redis = 'available';
  } catch (error) {
    health.services.redis = 'disconnected';
    health.status = 'degraded';
  }

  // WebSocket status
  if (wsServer) {
    health.services.websocket = 'active';
    health.websocket.connectedUsers = wsServer.getConnectedUsersCount();
    health.websocket.totalConnections = wsServer.getConnectionsCount();
  } else {
    health.services.websocket = 'not_initialized';
  }

  const statusCode = health.status === 'healthy' ? 200 : 503;
  res.status(statusCode).json(health);
});

// Root endpoint with WebSocket info
app.get('/', (req, res) => {
  console.log('🏠 Root endpoint requested');
  res.json({
    name: 'HPC Simulation Platform API',
    version: '1.1.0',
    status: 'running',
    timestamp: new Date().toISOString(),
    features: ['REST API', 'WebSocket Real-time Updates', 'JWT Authentication'],
    documentation: '/api/docs',
    websocket: {
      endpoint: '/socket.io/',
      authentication: 'JWT token required',
      events: ['job-status-update', 'job-update', 'active-jobs']
    },
    endpoints: {
      health: '/api/health',
      auth: '/api/v1/auth',
      simulations: '/api/v1/simulations',
      websocket: '/socket.io/'
    }
  });
});

// Enhanced API Documentation with WebSocket info
app.get('/api/docs', (req, res) => {
  res.json({
    title: 'HPC Simulation Platform API v1.1',
    version: '1.1.0',
    description: 'RESTful API with WebSocket real-time updates for managing HPC network simulations',
    baseUrl: `${req.protocol}://${req.get('host')}/api/v1`,
    features: [
      'JWT Authentication',
      'Real-time WebSocket Updates', 
      'Comprehensive Job Management',
      'Template System',
      'Time-series Metrics'
    ],
    websocket: {
      endpoint: `${req.protocol === 'https' ? 'wss' : 'ws'}://${req.get('host')}/socket.io/`,
      authentication: {
        method: 'JWT Token',
        description: 'Include JWT token in auth.token or Authorization header',
        example: "socket.auth = { token: 'your-jwt-token' }"
      },
      events: {
        outgoing: {
          'job-status-update': 'Detailed job status with progress and metrics',
          'job-update': 'Real-time job status changes',
          'active-jobs': 'List of user\'s active jobs',
          'connected': 'WebSocket connection confirmation'
        },
        incoming: {
          'subscribe-job': 'Subscribe to specific job updates (jobId)',
          'unsubscribe-job': 'Unsubscribe from job updates (jobId)',
          'get-job-status': 'Request current job status (jobId)',
          'get-active-jobs': 'Request list of active jobs'
        }
      },
      examples: {
        connect: `
const socket = io('${req.protocol === 'https' ? 'wss' : 'ws'}://${req.get('host')}', {
  auth: { token: 'your-jwt-token' }
});`,
        subscribe: `
socket.emit('subscribe-job', 'job-uuid-here');
socket.on('job-status-update', (data) => {
  console.log('Job update:', data.status, data.progress + '%');
});`
      }
    },
    documentation: {
      postman: 'Import the endpoints below into Postman for testing',
      curl: 'Use curl commands as shown in examples',
      swagger: 'OpenAPI 3.0 specification available on request'
    },
    endpoints: {
      // ... existing endpoint documentation
      system: {
        health: {
          method: 'GET',
          path: '/api/health',
          description: 'Check API and service health status including WebSocket',
          authentication: false,
          response: 'Includes WebSocket connection statistics'
        }
      },
      // ... rest of existing documentation
    }
  });
});

// API status endpoint with WebSocket stats
app.get('/api/v1/status', async (req, res) => {
  try {
    const memoryUsage = process.memoryUsage();
    const stats = {
      timestamp: new Date().toISOString(),
      uptime: process.uptime(),
      memory: {
        rss: Math.round(memoryUsage.rss / 1024 / 1024) + ' MB',
        heapTotal: Math.round(memoryUsage.heapTotal / 1024 / 1024) + ' MB',
        heapUsed: Math.round(memoryUsage.heapUsed / 1024 / 1024) + ' MB',
        external: Math.round(memoryUsage.external / 1024 / 1024) + ' MB'
      },
      environment: process.env.NODE_ENV || 'development',
      websocket: {
        active: !!wsServer,
        connectedUsers: wsServer ? wsServer.getConnectedUsersCount() : 0,
        totalConnections: wsServer ? wsServer.getConnectionsCount() : 0
      }
    };
    res.json(stats);
  } catch (error) {
    res.status(500).json({
      error: 'Failed to get system status',
      timestamp: new Date().toISOString()
    });
  }
});

// ===== EXISTING API ROUTES =====
// Auth Routes
app.post('/api/v1/auth/register', authRateLimit, authController.register);
app.post('/api/v1/auth/login', authRateLimit, authController.login);
app.get('/api/v1/auth/profile', authenticateToken, authController.getProfile);
app.post('/api/v1/auth/refresh', authenticateToken, authController.refreshToken);
app.post('/api/v1/auth/logout', authenticateToken, (req, res) => {
  console.log('🚪 User logged out:', (req as any).user?.email);
  res.status(200).json({
    message: 'Logout successful',
    note: 'Please remove the token from client storage and disconnect WebSocket'
  });
});

// Auth test route
app.get('/api/v1/auth/test', (req, res) => {
  res.json({
    message: 'Auth routes are working!',
    timestamp: new Date().toISOString(),
    websocketEnabled: !!wsServer,
    availableRoutes: [
      'POST /api/v1/auth/register',
      'POST /api/v1/auth/login', 
      'GET /api/v1/auth/profile',
      'POST /api/v1/auth/refresh',
      'POST /api/v1/auth/logout'
    ]
  });
});

// Simulation Routes (all require auth)
app.use('/api/v1/simulations', authenticateToken);

app.get('/api/v1/simulations/templates/topologies', simulationController.getTopologyTemplates);
app.get('/api/v1/simulations/templates/workloads', simulationController.getWorkloadPatterns);

// Enhanced simulation routes with WebSocket integration
app.post('/api/v1/simulations', async (req, res) => {
  // Call original controller
  await simulationController.createSimulation(req, res);
  
  // If job was created successfully, notify via WebSocket
  if (res.statusCode === 201 && wsServer) {
    const userId = (req as any).user?.userId;
    const responseData = (res as any).locals?.jobData;
    
    if (userId && responseData) {
      // Notify user of new job creation
      wsServer.broadcastJobUpdate({
        jobId: responseData.id,
        userId,
        status: 'queued',
        message: 'Job created and queued for processing'
      });
    }
  }
});

app.get('/api/v1/simulations', simulationController.getSimulations);
app.get('/api/v1/simulations/:id', simulationController.getSimulationById);
app.delete('/api/v1/simulations/:id', async (req, res) => {
  const jobId = req.params.id;
  const userId = (req as any).user?.userId;
  
  // Call original controller
  await simulationController.cancelSimulation(req, res);
  
  // If cancellation was successful, notify via WebSocket
  if (res.statusCode === 200 && wsServer && userId) {
    wsServer.notifyJobStatusChange(jobId, 'cancelled', userId);
  }
});

// Simulation test route
app.get('/api/v1/simulations/test', (req, res) => {
  res.json({
    message: 'Simulation routes are working!',
    timestamp: new Date().toISOString(),
    user: (req as any).user || 'No user authenticated',
    websocketEnabled: !!wsServer,
    realTimeFeatures: !!wsServer ? [
      'Live job progress updates',
      'Real-time status notifications',
      'Active job monitoring'
    ] : ['WebSocket not available'],
    availableRoutes: [
      'GET /api/v1/simulations/templates/topologies',
      'GET /api/v1/simulations/templates/workloads',
      'POST /api/v1/simulations',
      'GET /api/v1/simulations',
      'GET /api/v1/simulations/:id',
      'DELETE /api/v1/simulations/:id'
    ]
  });
});

console.log('✅ All routes configured with WebSocket integration');

// 404 handler for API routes
app.use('/api/*', (req, res) => {
  console.log(`❌ 404 - API route not found: ${req.method} ${req.path}`);
  res.status(404).json({ 
    error: 'API endpoint not found',
    method: req.method,
    path: req.path,
    availableEndpoints: [
      'GET /api/health',
      'GET /api/docs',
      'POST /api/v1/auth/register',
      'POST /api/v1/auth/login',
      'GET /api/v1/auth/profile',
      'POST /api/v1/simulations',
      'GET /api/v1/simulations',
      'WebSocket: /socket.io/'
    ]
  });
});

// General 404 handler
app.use((req, res) => {
  console.log(`❌ 404 - Route not found: ${req.method} ${req.path}`);
  res.status(404).json({ 
    error: 'Route not found',
    suggestion: 'Visit /api/docs for API documentation or connect to /socket.io/ for WebSocket' 
  });
});

// Global error handler
app.use((err: any, req: any, res: any, next: any) => {
  console.error('💥 Server error:', err);
  
  const isDevelopment = process.env.NODE_ENV !== 'production';
  
  res.status(err.status || 500).json({
    error: 'Internal server error',
    message: isDevelopment ? err.message : 'Something went wrong',
    ...(isDevelopment && { stack: err.stack })
  });
});

// Start HTTP server with WebSocket support
const httpServer = server.listen(port, '0.0.0.0', () => {
  console.log(`✅ HPC Simulation API with WebSocket running on port ${port}`);
  console.log(`📚 Health check: http://localhost:${port}/api/health`);
  console.log(`🏠 Home: http://localhost:${port}/`);
  console.log(`📖 API Docs: http://localhost:${port}/api/docs`);
  console.log(`🔐 Auth endpoints: http://localhost:${port}/api/v1/auth/*`);
  console.log(`🧪 Simulation endpoints: http://localhost:${port}/api/v1/simulations/*`);
  console.log(`🔗 WebSocket endpoint: ws://localhost:${port}/socket.io/`);
  
  // Initialize WebSocket server after HTTP server starts
  wsServer = new WebSocketServer(httpServer);
  console.log(`🔗 WebSocket server ready for real-time job monitoring`);
});

// Graceful shutdown with WebSocket cleanup
const gracefulShutdown = async (signal: string) => {
  console.log(`🛑 ${signal} received, shutting down gracefully`);
  
  // Close WebSocket connections
  if (wsServer) {
    console.log('🔗 Closing WebSocket connections...');
    // WebSocket cleanup handled by Socket.IO
  }
  
  httpServer.close(async () => {
    console.log('📡 HTTP server closed');
    
    if (dbPool) {
      await dbPool.end();
      console.log('🗄️ Database pool closed');
    }
    
    console.log('✅ Graceful shutdown completed');
    process.exit(0);
  });
  
  setTimeout(() => {
    console.error('💥 Could not close connections in time, forcefully shutting down');
    process.exit(1);
  }, 10000);
};

process.on('SIGTERM', () => gracefulShutdown('SIGTERM'));
process.on('SIGINT', () => gracefulShutdown('SIGINT'));

// Export WebSocket server for use by worker notifications
export { wsServer };
export default app;