# HPC Simulation Platform

A scalable, cloud-native platform for running High-Performance Computing network simulations, built on top of OMNeT++ simulation framework.

## 🚀 Project Status

### ✅ **Session 2 COMPLETE** - Core Platform Ready
- **Full Authentication System**: JWT-based registration, login, and protected routes
- **Complete Simulation API**: Job submission, monitoring, and results retrieval
- **Mock Simulation Engine**: Realistic HPC simulation processing with metrics
- **Production Infrastructure**: PostgreSQL, Redis, Docker orchestration
- **Professional Testing**: Comprehensive API validation and end-to-end workflows

### 🎯 **Session 3 PLANNED** - Advanced Features
- GraphQL API for complex queries and real-time subscriptions
- WebSocket connections for live job monitoring
- Enhanced analytics and performance optimization
- OMNeT++ integration for actual simulations

---

## 🏗️ Architecture Overview
```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   API Client    │────│   API Gateway   │────│ Simulation      │
│  (REST/GraphQL) │    │  (Node.js +     │    │ Worker          │
└─────────────────┘    │   TypeScript)   │    │ (Python +       │
                       └─────────────────┘    │  OMNeT++)       │
                                │              └─────────────────┘
                       ┌─────────────────┐               │
                       │   PostgreSQL    │               │
                       │   Database      │───────────────┘
                       └─────────────────┘
                                │
                       ┌─────────────────┐
                       │   Redis Queue   │
                       │   & Cache       │
                       └─────────────────┘
```

## 🛠️ Technology Stack

### Backend Services
- **API Gateway**: Node.js, Express, TypeScript, JWT Authentication
- **Simulation Engine**: Python async workers with realistic mock processing
- **Database**: PostgreSQL with comprehensive schema and time-series data
- **Message Queue**: Redis for async job processing and caching

### DevOps & Infrastructure  
- **Containerization**: Docker, Docker Compose with health checks
- **Development**: Hot reload, structured logging, comprehensive testing
- **Monitoring**: Built-in health endpoints and service status tracking
- **Documentation**: Interactive API docs at `/api/docs`

### Development Tools
- **API Testing**: Comprehensive test suite with realistic workflows
- **Development Environment**: Docker Compose for complete local setup
- **Code Quality**: TypeScript strict mode, ESLint, proper error handling

---

## 📊 Current Capabilities

### 🔐 **Authentication System**
- **User Registration**: Secure account creation with validation
- **JWT Login**: Token-based authentication with password hashing
- **Protected Routes**: Middleware-based authorization
- **Profile Management**: User statistics and account information

### 🚀 **Simulation Management**
- **Job Submission**: Submit HPC simulation jobs with comprehensive parameters
- **Real-time Monitoring**: Track job status from submission to completion
- **Results Retrieval**: Access detailed metrics, logs, and performance data
- **Template System**: Pre-configured network topologies and workload patterns

### 📈 **Analytics & Metrics**
- **Performance Tracking**: Throughput, latency, and queue length metrics
- **Time-series Data**: Historical performance analysis
- **Job Statistics**: User-specific job history and success rates
- **Structured Logging**: Comprehensive audit trail and debugging info

### 🏗️ **Infrastructure**
- **Multi-service Architecture**: Scalable microservices design
- **Database Integration**: PostgreSQL with optimized schema
- **Async Processing**: Redis-based job queue with worker pattern
- **Health Monitoring**: Service status and dependency tracking

---

## 🚀 Quick Start

### Prerequisites
- Docker & Docker Compose
- Node.js 16+ (for local development)
- Git (for version control)

### 1. Clone and Setup
```bash
git clone https://github.com/yehao622/hpc-simulation-platform.git
cd hpc-simulation-platform

# Start all services
docker-compose up -d

# Check service status
docker-compose ps
```

### 2. Verify Installation
```bash
# API Health Check
curl http://localhost:3000/api/health

# API Documentation
open http://localhost:3000/api/docs
```

### 3. Quick API Test
```bash
# Use the provided test script
chmod +x api_test_script.sh
./api_test_script.sh
```

## 📚 API Usage Examples

### Authentication Flow
```bash
# Register new user
curl -X POST http://localhost:3000/api/v1/auth/register \
  -H "Content-Type: application/json" \
  -d '{
    "email": "researcher@university.edu",
    "username": "researcher1",
    "password": "securepassword123",
    "firstName": "Jane",
    "lastName": "Researcher",
    "organization": "Research University"
  }'

# Login and get JWT token
curl -X POST http://localhost:3000/api/v1/auth/login \
  -H "Content-Type: application/json" \
  -d '{
    "email": "researcher@university.edu",
    "password": "securepassword123"
  }'
```

### Simulation Job Management
```bash
# Submit simulation job (requires JWT token)
TOKEN="your-jwt-token-here"

curl -X POST http://localhost:3000/api/v1/simulations \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "name": "Fat-tree Performance Analysis",
    "description": "Testing network performance under read-heavy workload",
    "topologyId": 1,
    "workloadId": 1,
    "simulationTime": 10.0,
    "numComputeNodes": 16,
    "numStorageNodes": 8,
    "workType": "read",
    "dataSizeMb": 128.0
  }'

# Monitor job status
JOB_ID="simulation-job-uuid"
curl -X GET http://localhost:3000/api/v1/simulations/$JOB_ID \
  -H "Authorization: Bearer $TOKEN"

# List all jobs
curl -X GET http://localhost:3000/api/v1/simulations \
  -H "Authorization: Bearer $TOKEN"
```

---

## 🎯 Development Roadmap

### ✅ Phase 1: Core Platform (COMPLETED)
- [x] RESTful API design and implementation
- [x] Database schema and user management
- [x] JWT authentication and authorization
- [x] Mock simulation engine with realistic results
- [x] Docker containerization and service orchestration
- [x] Comprehensive testing and documentation

### 🔄 Phase 2: Advanced Features (NEXT)
- [ ] GraphQL API for complex queries and relationships
- [ ] WebSocket connections for real-time job updates
- [ ] Enhanced analytics with time-series visualizations
- [ ] Performance optimization with Redis caching
- [ ] Monitoring integration with Prometheus/Grafana

### 🚀 Phase 3: Production Deployment (PLANNED)
- [ ] AWS infrastructure provisioning with Terraform
- [ ] CI/CD pipeline with GitHub Actions
- [ ] Auto-scaling and load balancing
- [ ] Advanced monitoring and alerting
- [ ] OMNeT++ simulator integration

---

## 💼 Professional Skills Demonstrated

This project showcases modern software engineering practices:

### **Backend Development**
- RESTful API design with comprehensive validation
- JWT authentication and authorization systems
- Database design with PostgreSQL optimization
- Microservices architecture with Docker
- Async job processing with Redis queues

### **DevOps & Infrastructure**
- Docker containerization and orchestration
- Service health monitoring and dependency management
- Structured logging and error handling
- Environment configuration and secrets management

### **Professional Practices**
- Agile development with sprint planning
- Git workflow with feature branches
- Comprehensive testing strategies
- Technical documentation and API specs
- Code quality with TypeScript and linting

### **Domain Expertise**
- HPC systems and network simulation
- Performance metrics and analysis
- Distributed systems design
- API versioning and backwards compatibility

---

## 📁 Project Structure

```
hpc-simulation-platform/
├── README.md                    # Project overview (this file)
├── docker-compose.yml          # Multi-service development environment
├── api_test_script.sh          # Comprehensive API testing script
├── docs/                       # Technical documentation
│   ├── development-log.md      # Detailed development progress
│   ├── current-status.md       # Current capabilities and next steps
│   └── quick-start.md         # Setup and usage guide
├── api-gateway/                # Node.js/TypeScript API service
│   ├── src/                   # Source code
│   │   ├── controllers/       # API route handlers
│   │   ├── middleware/        # Authentication and validation
│   │   ├── routes/           # API route definitions
│   │   └── types/            # TypeScript type definitions
│   ├── database/             # Database schema and initialization
│   └── package.json          # Node.js dependencies
├── simulation-worker/          # Python async simulation processor
│   ├── src/worker.py         # Mock simulation engine
│   ├── requirements.txt      # Python dependencies
│   └── Dockerfile           # Worker container configuration
├── legacy-simulator/          # Original OMNeT++ HPC simulator
└── .github/                  # CI/CD and issue templates
```