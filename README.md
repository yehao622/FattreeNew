# HPC Simulation Platform

A scalable, cloud-native platform for running High-Performance Computing network simulations, built on top of OMNeT++ simulation framework.

## 🚀 Project Vision

Transform academic HPC simulation tools into production-ready SaaS platform demonstrating:
- **Microservices Architecture** with RESTful APIs and GraphQL
- **Cloud-Native Deployment** on AWS with containerization
- **Modern DevOps Practices** with CI/CD and Infrastructure as Code
- **Scalable Data Processing** with PostgreSQL and async job queues

## 🏗️ Architecture Overview
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Web Client    │────│   API Gateway   │────│ Simulation      │
│   (React)       │    │  (Node.js +     │    │ Worker          │
└─────────────────┘    │   GraphQL)      │    │ (Python +       │
└─────────────────┘    │  OMNeT++)       │
│              └─────────────────┘
┌─────────────────┐               │
│   PostgreSQL    │               │
│   Database      │───────────────┘
└─────────────────┘

## 🛠️ Technology Stack

### Backend Services
- **API Gateway**: Node.js, Express, GraphQL, TypeScript
- **Simulation Engine**: Python, OMNeT++, Docker containers
- **Database**: PostgreSQL with time-series optimizations
- **Message Queue**: Redis/Celery for async job processing

### DevOps & Infrastructure  
- **Containerization**: Docker, Docker Compose
- **Cloud Platform**: AWS (ECS, RDS, S3, CloudWatch)
- **CI/CD**: GitHub Actions, automated testing and deployment
- **Infrastructure as Code**: Terraform
- **Monitoring**: Prometheus, Grafana

### Development Tools
- **Build Tools**: Webpack, Babel, ESLint, Prettier
- **Testing**: Jest, Supertest, pytest
- **Development**: Docker Compose for local environment

## 📊 Original Simulation Capabilities

Built upon a comprehensive OMNeT++ simulator featuring:
- **Fat-tree Network Topology** with configurable core/aggregation/edge layers
- **Storage System Modeling** including OSS/OST with realistic I/O patterns  
- **Multiple Interconnects**: InfiniBand, PCIe, SAS with accurate performance models
- **Workload Generation**: Configurable read/write patterns for HPC applications

## 🎯 Development Roadmap

### Phase 1: Core Platform (Weeks 1-2)
- [ ] RESTful API design and implementation
- [ ] Database schema and data models
- [ ] Basic containerization of simulation engine
- [ ] Local development environment setup

### Phase 2: Advanced Features (Weeks 3-4)  
- [ ] GraphQL API for complex queries
- [ ] Async job processing with status tracking
- [ ] Results visualization and analytics
- [ ] User authentication and authorization

### Phase 3: Production Deployment (Weeks 5-6)
- [ ] AWS infrastructure provisioning
- [ ] CI/CD pipeline implementation  
- [ ] Monitoring and logging setup
- [ ] Performance optimization and scaling

## 🚦 Current Status

🟡 **In Development** - Core platform architecture and API design