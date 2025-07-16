# Quick Start Guide

## Prerequisites

- **Docker** and **Docker Compose** (latest version)
- **Node.js** 16+ (for local development)
- **Git** (for version control)
- **VS Code** (recommended IDE)

## Local Development Setup

### 1. Clone Repository
```bash
git clone https://github.com/yehao622/hpc-simulation-platform.git
cd hpc-simulation-platform
```

### 2. Environment Configuration
```bash
# Copy environment template
# cp .env.example .env
# Edit .env with your local settings (optional for development)
# Default values work for local Docker setup
```

### 3. Start Development Environment
```bash
# Start all services with Docker Compose
docker-compose up -d

# Check service status
docker-compose ps

# View logs
docker-compose logs -f api-gateway
```

### 4. Verify Installation
```bash
# API Health Check
curl http://localhost:3000/api/health

# Expected response:
{
  "status": "healthy",
  "timestamp": "2025-01-13T...",
  "services": {
    "database": "connected",
    "redis": "connected"
  }
}
```

### 5. Access Services

| Service | URL | Credentials |
|---------|-----|-------------|
| API Gateway | http://localhost:3000 | - |
| API Documentation | http://localhost:3000/api/docs | - |
| Database (pgAdmin) | http://localhost:5050 | admin@hpc-simulation.com / admin |
| Redis (Commander) | http://localhost:8081 | - |
| Grafana (Monitoring) | http://localhost:3001 | admin / admin |

## API Usage Examples

### 1. Create User Account
```bash
curl -X POST http://localhost:3000/api/v1/users/register \
  -H "Content-Type: application/json" \
  -d '{
    "email": "researcher@university.edu",
    "username": "researcher1",
    "password": "securepassword123",
    "firstName": "John",
    "lastName": "Doe",
    "organization": "University Research Lab"
  }'
```

### 2. Login and Get Token
```bash
curl -X POST http://localhost:3000/api/v1/users/login \
  -H "Content-Type: application/json" \
  -d '{
    "email": "researcher@university.edu",
    "password": "securepassword123"
  }'
```

### 3. Submit Simulation Job
```bash
# Save token from login response
TOKEN="your-jwt-token-here"

curl -X POST http://localhost:3000/api/v1/simulations \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{
    "name": "Test Fat-tree Simulation",
    "description": "Testing basic fat-tree topology with read workload",
    "topologyId": 1,
    "workloadId": 1,
    "simulationTime": 10.0,
    "numComputeNodes": 16,
    "numStorageNodes": 8
  }'
```

### 4. Check Job Status
```bash
JOB_ID="simulation-job-uuid"

curl -X GET http://localhost:3000/api/v1/simulations/$JOB_ID \
  -H "Authorization: Bearer $TOKEN"
```

## Development Workflow

### 1. Feature Development
```bash
# Create feature branch
git checkout develop
git pull origin develop
git checkout -b feature/your-feature-name

# Make changes
# Test locally with Docker Compose

# Commit and push
git add .
git commit -m "feat: implement your feature"
git push origin feature/your-feature-name

# Create Pull Request on GitHub
```

### 2. Local Testing
```bash
# Run API tests
cd api-gateway
npm test

# Run worker tests
cd simulation-worker
python -m pytest

# Integration tests with Docker
docker-compose -f docker-compose.test.yml up --build
```

### 3. Database Operations
```bash
# Run migrations
docker-compose exec api-gateway npm run db:migrate

# Seed test data
docker-compose exec api-gateway npm run db:seed

# Access database directly
docker-compose exec postgres psql -U hpc_user -d hpc_simulation
```

## Troubleshooting

### Common Issues

**1. Port Conflicts**
```bash
# Check what's using port 3000
lsof -i :3000

# Kill process if needed
kill -9 <PID>
```

**2. Database Connection Issues**
```bash
# Reset database
docker-compose down -v
docker-compose up -d postgres
# Wait for database to be ready
docker-compose up -d
```

**3. OMNeT++ Compilation Issues**
```bash
# Rebuild simulation worker
docker-compose build simulation-worker --no-cache
```

**4. View Service Logs**
```bash
# All services
docker-compose logs

# Specific service
docker-compose logs api-gateway
docker-compose logs simulation-worker
```

### Performance Tuning

**1. Increase Worker Concurrency**
```bash
# Edit .env
WORKER_CONCURRENCY=4

# Restart services
docker-compose restart simulation-worker
```

**2. Database Performance**
```bash
# Monitor query performance
docker-compose exec postgres pg_stat_statements
```

## Production Deployment

### 1. AWS Setup (coming in Phase 3)
```bash
# Install Terraform
# Configure AWS credentials
# Deploy infrastructure
terraform init
terraform plan
terraform apply
```

### 2. Environment Configuration
```bash
# Production environment variables
NODE_ENV=production
DATABASE_URL=postgresql://user:pass@rds-endpoint/db
REDIS_URL=redis://elasticache-endpoint:6379
```

## Additional Resources

- **API Documentation**: http://localhost:3000/api/docs
- **GitHub Repository**: https://github.com/yehao622/hpc-simulation-platform
- **OMNeT++ Documentation**: https://omnetpp.org/documentation/
- **Docker Compose Reference**: https://docs.docker.com/compose/

## Support

For issues or questions:
1. Check existing [GitHub Issues](https://github.com/yehao622/hpc-simulation-platform/issues)
2. Create new issue with detailed description
3. Contact: your.email@example.com
