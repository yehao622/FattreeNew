// api-gateway/src/types/index.ts
// Global type definitions for the API

export interface User {
  id: number;
  email: string;
  username: string;
  firstName: string;
  lastName: string;
  organization?: string;
  role: string;
  isActive: boolean;
  createdAt: Date;
  updatedAt: Date;
}

export interface SimulationJob {
  id: string;
  userId: number;
  name: string;
  description?: string;
  status: 'queued' | 'running' | 'completed' | 'failed' | 'cancelled';
  topologyId: number;
  workloadId: number;
  simulationTime: number;
  randomSeed?: number;
  
  // Network configuration
  numComputeNodes: number;
  numStorageNodes: number;
  numCoreSwitches: number;
  numAggrSwitches: number;
  numEdgeSwitches: number;
  
  // Performance parameters
  infinibandBandwidth: number;
  pcieBandwidth: number;
  sasBandwidth: number;
  
  // Workload parameters
  workType: 'read' | 'write' | 'mixed';
  dataSizeMb: number;
  readProbability: number;
  requestRate: number;
  
  // Custom parameters
  customParameters?: Record<string, any>;
  
  // Execution tracking
  workerId?: string;
  startedAt?: Date;
  completedAt?: Date;
  errorMessage?: string;
  
  // Results
  totalThroughput?: number;
  averageLatency?: number;
  maxQueueLength?: number;
  
  // Metadata
  createdAt: Date;
  updatedAt: Date;
}

export interface TopologyTemplate {
  id: number;
  name: string;
  type: 'fat_tree' | 'mesh' | 'torus' | 'custom';
  description?: string;
  parameters: Record<string, any>;
  createdBy?: number;
  isPublic: boolean;
  createdAt: Date;
}

export interface WorkloadPattern {
  id: number;
  name: string;
  description?: string;
  parameters: Record<string, any>;
  createdBy?: number;
  isPublic: boolean;
  createdAt: Date;
}

export interface SimulationMetric {
  id: number;
  jobId: string;
  metricType: string;
  componentType?: string;
  componentId?: string;
  timestampSec: number;
  value: number;
  unit?: string;
  createdAt: Date;
}

export interface JobLog {
  id: number;
  jobId: string;
  logLevel: 'INFO' | 'WARN' | 'ERROR' | 'DEBUG';
  message: string;
  component?: string;
  simulationTime?: number;
  createdAt: Date;
}

export interface ApiResponse<T = any> {
  message?: string;
  data?: T;
  error?: string;
  details?: string[];
}

export interface PaginatedResponse<T> {
  data: T[];
  pagination: {
    currentPage: number;
    totalPages: number;
    totalCount: number;
    hasNext: boolean;
    hasPrev: boolean;
  };
}

export interface AuthTokenPayload {
  userId: number;
  email: string;
  iat: number;
  exp: number;
}

// Express Request extensions
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
