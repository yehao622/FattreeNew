# HPC Simulation Platform

A scalable, cloud-native platform for running High-Performance Computing network simulations, built on top of OMNeT++ simulation framework.

## 🚀 Project Vision

Transform academic HPC simulation tools into production-ready SaaS platform demonstrating:
- **Microservices Architecture** with RESTful APIs and GraphQL
- **Cloud-Native Deployment** on AWS with containerization
- **Modern DevOps Practices** with CI/CD and Infrastructure as Code
- **Scalable Data Processing** with PostgreSQL and async job queues

## 🏗️ Architecture Overview
```
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
```

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

## 💼 Professional Skills Demonstrated

This project showcases modern software engineering practices:
- **Full-Stack Development**: REST/GraphQL APIs, database design, containerization
- **Cloud Architecture**: AWS services, microservices, scalable infrastructure  
- **DevOps Practices**: CI/CD, Infrastructure as Code, monitoring
- **Agile Development**: Feature branches, sprint planning, user stories
- **Domain Expertise**: HPC systems, network simulation, performance analysis

## 📁 Project Structure

```
hpc-simulation-platform/
├── README.md                    # Project overview
├── docs/                       # Documentation
│   ├── architecture.md         # System architecture
│   ├── api-design.md          # API specifications
│   ├── development-log.md     # Development progress
│   └── deployment-guide.md    # Deployment instructions
├── legacy-simulator/          # Original OMNeT++ HPC simulator
├── api-gateway/              # RESTful API service
├── simulation-worker/        # Containerized simulation engine
├── infrastructure/           # Terraform configurations
├── docker-compose.yml       # Local development environment
└── .github/                 # CI/CD and issue templates
```

## 🚀 Quick Start

### Prerequisites
- Docker & Docker Compose
- Node.js 16+
- PostgreSQL 14+

### Local Development
```bash
# Clone repository
git clone https://github.com/yourusername/hpc-simulation-platform.git
cd hpc-simulation-platform

# Start development environment
docker-compose up -d

# Install API dependencies
cd api-gateway
npm install
npm run dev
```

## 🤝 Contributing

This project follows Agile/SCRUM methodology:
1. Check current sprint goals in [docs/current-status.md](docs/current-status.md)
2. Create feature branch from `develop`
3. Follow commit conventions: `feat:`, `fix:`, `docs:`, `refactor:`
4. Submit PR with detailed description
5. Ensure all tests pass and code review approval

## 📈 Performance Metrics

Current platform capabilities:
- **Concurrent Simulations**: Up to 10 parallel jobs
- **API Response Time**: < 100ms for job submission
- **Simulation Throughput**: 1000+ network nodes per simulation
- **Data Processing**: Real-time metrics collection and storage

## 📋 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 📞 Contact

For questions about this project or professional inquiries:
- **Email**: hyedailyuse@gmail.com

---