-- Database Schema for HPC Simulation Platform
-- File: api-gateway/database/schema.sql

-- Enable UUID extension
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

-- Create ENUM types
CREATE TYPE job_status_enum AS ENUM ('queued', 'running', 'completed', 'failed', 'cancelled');
CREATE TYPE work_type_enum AS ENUM ('read', 'write', 'mixed');
CREATE TYPE topology_type_enum AS ENUM ('fat_tree', 'mesh', 'torus', 'custom');

-- Users table
CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    email VARCHAR(255) UNIQUE NOT NULL,
    username VARCHAR(100) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    first_name VARCHAR(100),
    last_name VARCHAR(100),
    organization VARCHAR(255),
    role VARCHAR(50) DEFAULT 'researcher',
    created_at TIMESTAMP DEFAULT NOW(),
    updated_at TIMESTAMP DEFAULT NOW(),
    is_active BOOLEAN DEFAULT TRUE
);

-- Network topology templates
CREATE TABLE topology_templates (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    type topology_type_enum NOT NULL,
    description TEXT,
    parameters JSONB NOT NULL, -- Store topology-specific parameters
    created_by INTEGER REFERENCES users(id),
    is_public BOOLEAN DEFAULT FALSE,
    created_at TIMESTAMP DEFAULT NOW()
);

-- Workload patterns
CREATE TABLE workload_patterns (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    description TEXT,
    parameters JSONB NOT NULL, -- Store workload configuration
    created_by INTEGER REFERENCES users(id),
    is_public BOOLEAN DEFAULT FALSE,
    created_at TIMESTAMP DEFAULT NOW()
);

-- Main simulation jobs table
CREATE TABLE simulation_jobs (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    user_id INTEGER REFERENCES users(id) ON DELETE CASCADE,
    name VARCHAR(255) NOT NULL,
    description TEXT,
    status job_status_enum DEFAULT 'queued',
    
    -- Simulation parameters
    topology_id INTEGER REFERENCES topology_templates(id),
    workload_id INTEGER REFERENCES workload_patterns(id),
    simulation_time DECIMAL(10,6) NOT NULL DEFAULT 10.0, -- in seconds
    random_seed INTEGER,
    
    -- Network configuration
    num_compute_nodes INTEGER NOT NULL DEFAULT 16,
    num_storage_nodes INTEGER NOT NULL DEFAULT 8,
    num_core_switches INTEGER NOT NULL DEFAULT 2,
    num_aggr_switches INTEGER NOT NULL DEFAULT 8,
    num_edge_switches INTEGER NOT NULL DEFAULT 8,
    
    -- Performance parameters
    infiniband_bandwidth DECIMAL(10,2) DEFAULT 25.0, -- Gbps
    pcie_bandwidth DECIMAL(10,2) DEFAULT 24.0, -- Gbps
    sas_bandwidth DECIMAL(10,2) DEFAULT 10.0, -- Gbps
    
    -- Workload parameters
    work_type work_type_enum DEFAULT 'read',
    data_size_mb DECIMAL(10,2) DEFAULT 128.0,
    read_probability DECIMAL(3,2) DEFAULT 0.5,
    request_rate DECIMAL(10,6) DEFAULT 0.001, -- requests per second
    
    -- Custom parameters (for advanced users)
    custom_parameters JSONB,
    
    -- Execution tracking
    worker_id VARCHAR(100), -- Docker container ID
    started_at TIMESTAMP,
    completed_at TIMESTAMP,
    error_message TEXT,
    
    -- Results summary
    total_throughput DECIMAL(15,6), -- MB/s
    average_latency DECIMAL(15,6), -- seconds
    max_queue_length INTEGER,
    
    -- Metadata
    created_at TIMESTAMP DEFAULT NOW(),
    updated_at TIMESTAMP DEFAULT NOW()
);

-- Simulation results - time series data
CREATE TABLE simulation_metrics (
    id BIGSERIAL PRIMARY KEY,
    job_id UUID REFERENCES simulation_jobs(id) ON DELETE CASCADE,
    metric_type VARCHAR(50) NOT NULL, -- 'throughput', 'latency', 'queue_length', etc.
    component_type VARCHAR(50), -- 'compute_node', 'storage_node', 'switch', etc.
    component_id VARCHAR(100), -- e.g., 'cn[0]', 'oss[1]', 'edge[2]'
    timestamp_sec DECIMAL(15,6) NOT NULL, -- simulation time
    value DECIMAL(20,8) NOT NULL,
    unit VARCHAR(20), -- 'Mbps', 'ms', 'count', etc.
    
    -- Indexing for time-series queries
    created_at TIMESTAMP DEFAULT NOW()
);

-- Job execution logs
CREATE TABLE job_logs (
    id BIGSERIAL PRIMARY KEY,
    job_id UUID REFERENCES simulation_jobs(id) ON DELETE CASCADE,
    log_level VARCHAR(10) NOT NULL, -- 'INFO', 'WARN', 'ERROR', 'DEBUG'
    message TEXT NOT NULL,
    component VARCHAR(100), -- Which component generated the log
    simulation_time DECIMAL(15,6), -- When in simulation time this occurred
    created_at TIMESTAMP DEFAULT NOW()
);

-- API tokens for authentication
CREATE TABLE api_tokens (
    id SERIAL PRIMARY KEY,
    user_id INTEGER REFERENCES users(id) ON DELETE CASCADE,
    token_hash VARCHAR(255) UNIQUE NOT NULL,
    name VARCHAR(100) NOT NULL,
    expires_at TIMESTAMP,
    last_used_at TIMESTAMP,
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT NOW()
);

-- Job queue for async processing
CREATE TABLE job_queue (
    id BIGSERIAL PRIMARY KEY,
    job_id UUID REFERENCES simulation_jobs(id) ON DELETE CASCADE,
    priority INTEGER DEFAULT 0,
    retry_count INTEGER DEFAULT 0,
    max_retries INTEGER DEFAULT 3,
    queued_at TIMESTAMP DEFAULT NOW(),
    claimed_at TIMESTAMP,
    worker_id VARCHAR(100)
);

-- Create indexes for performance
CREATE INDEX idx_simulation_jobs_user_status ON simulation_jobs(user_id, status);
CREATE INDEX idx_simulation_jobs_status_created ON simulation_jobs(status, created_at);
CREATE INDEX idx_simulation_metrics_job_type ON simulation_metrics(job_id, metric_type);
CREATE INDEX idx_simulation_metrics_timestamp ON simulation_metrics(job_id, timestamp_sec);
CREATE INDEX idx_job_logs_job_level ON job_logs(job_id, log_level);
CREATE INDEX idx_job_queue_priority ON job_queue(priority DESC, queued_at);
CREATE INDEX idx_users_email ON users(email);
CREATE INDEX idx_api_tokens_hash ON api_tokens(token_hash);

-- Create updated_at trigger function
CREATE OR REPLACE FUNCTION update_updated_at_column()
RETURNS TRIGGER AS $$
BEGIN
    NEW.updated_at = NOW();
    RETURN NEW;
END;
$$ language 'plpgsql';

-- Add triggers for updated_at
CREATE TRIGGER update_users_updated_at BEFORE UPDATE ON users
    FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
    
CREATE TRIGGER update_simulation_jobs_updated_at BEFORE UPDATE ON simulation_jobs
    FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

-- Insert default topology templates
INSERT INTO topology_templates (name, type, description, parameters, created_by, is_public) VALUES
('Standard Fat-Tree', 'fat_tree', 'Default 3-tier fat-tree topology for HPC clusters', 
 '{"k_port": 8, "core_switches": 2, "aggr_switches": 8, "edge_switches": 8}', 
 NULL, true),
('Small Fat-Tree', 'fat_tree', 'Smaller fat-tree for testing and development', 
 '{"k_port": 4, "core_switches": 1, "aggr_switches": 4, "edge_switches": 4}', 
 NULL, true),
('Large Fat-Tree', 'fat_tree', 'Large-scale fat-tree for production workloads', 
 '{"k_port": 16, "core_switches": 4, "aggr_switches": 16, "edge_switches": 16}', 
 NULL, true);

-- Insert default workload patterns
INSERT INTO workload_patterns (name, description, parameters, created_by, is_public) VALUES
('Read-Heavy Workload', 'Typical read-intensive HPC workload (80% reads)', 
 '{"read_probability": 0.8, "request_rate": 0.001, "data_size_mb": 128}', 
 NULL, true),
('Write-Heavy Workload', 'Write-intensive workload for checkpointing', 
 '{"read_probability": 0.2, "request_rate": 0.0005, "data_size_mb": 512}', 
 NULL, true),
('Balanced Workload', 'Balanced read/write pattern', 
 '{"read_probability": 0.5, "request_rate": 0.001, "data_size_mb": 256}', 
 NULL, true),
('High-Frequency Small I/O', 'Metadata-intensive workload', 
 '{"read_probability": 0.6, "request_rate": 0.01, "data_size_mb": 4}', 
 NULL, true);

-- Create views for common queries
CREATE VIEW active_jobs AS
SELECT 
    j.*,
    u.username,
    u.organization,
    t.name as topology_name,
    w.name as workload_name
FROM simulation_jobs j
JOIN users u ON j.user_id = u.id
LEFT JOIN topology_templates t ON j.topology_id = t.id
LEFT JOIN workload_patterns w ON j.workload_id = w.id
WHERE j.status IN ('queued', 'running');

CREATE VIEW job_statistics AS
SELECT 
    DATE_TRUNC('day', created_at) as date,
    status,
    COUNT(*) as job_count,
    AVG(EXTRACT(EPOCH FROM (completed_at - started_at))) as avg_duration_seconds
FROM simulation_jobs
WHERE created_at >= NOW() - INTERVAL '30 days'
GROUP BY DATE_TRUNC('day', created_at), status
ORDER BY date DESC;

-- Grant permissions (adjust as needed for your setup)
-- GRANT SELECT, INSERT, UPDATE, DELETE ON ALL TABLES IN SCHEMA public TO api_user;
-- GRANT USAGE, SELECT ON ALL SEQUENCES IN SCHEMA public TO api_user;
