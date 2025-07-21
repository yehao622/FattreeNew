#!/usr/bin/env python3
"""
Mock Simulation Worker for HPC Platform
Processes simulation jobs from Redis queue and generates realistic results
"""

import os
import json
import time
import random
import logging
import asyncio
import psycopg2
from datetime import datetime, timedelta
from typing import Dict, Any, Optional
from dataclasses import dataclass

# Import redis with fallback
try:
    import redis
except ImportError:
    print("⚠️ Redis not available, running in standalone mode")
    redis = None

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

@dataclass
class SimulationJob:
    job_id: str
    user_id: int
    name: str
    simulation_time: float
    network_config: Dict[str, Any]
    workload_config: Dict[str, Any]
    topology_params: Dict[str, Any]
    workload_params: Dict[str, Any]

class MockSimulationWorker:
    def __init__(self):
        self.worker_id = f"worker-{os.getpid()}-{int(time.time())}"
        self.redis_client = None
        self.db_connection = None
        self.running = False
        
    async def initialize(self):
        """Initialize Redis and database connections"""
        try:
            # Connect to Redis
            redis_url = os.getenv('REDIS_URL', 'redis://localhost:6379')
            self.redis_client = redis.Redis.from_url(redis_url, decode_responses=True)
            self.redis_client.ping()
            logger.info(f"✅ Connected to Redis: {redis_url}")
            
            # Connect to PostgreSQL
            db_url = os.getenv('DATABASE_URL', 'postgresql://hpc_user:hpc_password@localhost:5432/hpc_simulation')
            self.db_connection = psycopg2.connect(db_url)
            self.db_connection.autocommit = True
            logger.info(f"✅ Connected to PostgreSQL")
            
            logger.info(f"🔧 Worker initialized: {self.worker_id}")
            return True
            
        except Exception as e:
            logger.error(f"💥 Failed to initialize worker: {e}")
            return False
    
    async def process_queue(self):
        """Main queue processing loop"""
        self.running = True
        logger.info("🚀 Starting queue processing...")
        
        while self.running:
            try:
                # Block for up to 5 seconds waiting for a job
                job_data = self.redis_client.brpop('simulation_queue', timeout=5)
                
                if job_data:
                    queue_name, job_json = job_data
                    job_info = json.loads(job_json)
                    await self.process_simulation_job(job_info['jobId'])
                else:
                    # No jobs available, short sleep
                    await asyncio.sleep(1)
                    
            except KeyboardInterrupt:
                logger.info("🛑 Received shutdown signal")
                self.running = False
                break
            except Exception as e:
                logger.error(f"💥 Queue processing error: {e}")
                await asyncio.sleep(5)  # Wait before retrying
    
    async def process_simulation_job(self, job_id: str):
        """Process a single simulation job"""
        try:
            logger.info(f"🎯 Processing job: {job_id}")
            
            # Fetch job details from database
            job = await self.fetch_job_details(job_id)
            if not job:
                logger.error(f"❌ Job not found: {job_id}")
                return
            
            # Update job status to running
            await self.update_job_status(job_id, 'running', started_at=datetime.now())
            await self.add_job_log(job_id, 'INFO', f'Simulation started by worker {self.worker_id}', 'worker')
            
            # Run mock simulation
            results = await self.run_mock_simulation(job)
            
            # Update job with results
            await self.update_job_results(job_id, results)
            await self.update_job_status(job_id, 'completed', completed_at=datetime.now())
            await self.add_job_log(job_id, 'INFO', 'Simulation completed successfully', 'worker')
            
            logger.info(f"✅ Job completed: {job_id}")
            
        except Exception as e:
            logger.error(f"💥 Job processing error for {job_id}: {e}")
            await self.update_job_status(job_id, 'failed', error_message=str(e), completed_at=datetime.now())
            await self.add_job_log(job_id, 'ERROR', f'Simulation failed: {str(e)}', 'worker')
    
    async def fetch_job_details(self, job_id: str) -> Optional[SimulationJob]:
        """Fetch job details from database"""
        try:
            cursor = self.db_connection.cursor()
            cursor.execute("""
                SELECT 
                    sj.id, sj.user_id, sj.name, sj.simulation_time,
                    sj.num_compute_nodes, sj.num_storage_nodes, sj.num_core_switches,
                    sj.num_aggr_switches, sj.num_edge_switches, sj.infiniband_bandwidth,
                    sj.pcie_bandwidth, sj.sas_bandwidth, sj.work_type, sj.data_size_mb,
                    sj.read_probability, sj.request_rate, sj.custom_parameters,
                    tt.parameters as topology_params, wp.parameters as workload_params
                FROM simulation_jobs sj
                LEFT JOIN topology_templates tt ON sj.topology_id = tt.id
                LEFT JOIN workload_patterns wp ON sj.workload_id = wp.id
                WHERE sj.id = %s AND sj.status = 'queued'
            """, (job_id,))
            
            row = cursor.fetchone()
            if not row:
                return None
                
            return SimulationJob(
                job_id=row[0],
                user_id=row[1],
                name=row[2],
                simulation_time=float(row[3]),
                network_config={
                    'compute_nodes': row[4],
                    'storage_nodes': row[5],
                    'core_switches': row[6],
                    'aggr_switches': row[7],
                    'edge_switches': row[8],
                    'infiniband_bw': float(row[9]),
                    'pcie_bw': float(row[10]),
                    'sas_bw': float(row[11])
                },
                workload_config={
                    'work_type': row[12],
                    'data_size_mb': float(row[13]),
                    'read_probability': float(row[14]),
                    'request_rate': float(row[15])
                },
                topology_params=json.loads(row[17]) if row[17] else {},
                workload_params=json.loads(row[18]) if row[18] else {}
            )
            
        except Exception as e:
            logger.error(f"💥 Failed to fetch job details: {e}")
            return None
    
    async def run_mock_simulation(self, job: SimulationJob) -> Dict[str, Any]:
        """Run mock simulation and generate realistic results"""
        logger.info(f"🧮 Running mock simulation for {job.simulation_time}s")
        
        # Generate realistic metrics over time
        metrics = []
        logs = []
        
        # Simulation progress
        steps = int(job.simulation_time * 10)  # 10 data points per second
        step_duration = job.simulation_time / steps
        
        base_throughput = job.network_config['compute_nodes'] * 0.8  # MB/s per node
        base_latency = 0.05  # 50ms base latency
        max_queue_length = 0
        
        for step in range(steps):
            current_time = step * step_duration
            
            # Add some randomness and trends
            throughput_variation = random.uniform(0.8, 1.2)
            latency_variation = random.uniform(0.9, 1.3)
            
            # Network congestion effects
            congestion_factor = 1.0
            if job.workload_config['work_type'] == 'write':
                congestion_factor = 1.2
            elif job.workload_config['work_type'] == 'mixed':
                congestion_factor = 1.1
            
            current_throughput = base_throughput * throughput_variation / congestion_factor
            current_latency = base_latency * latency_variation * congestion_factor
            
            # Queue length simulation
            queue_length = max(0, int(random.gauss(job.network_config['compute_nodes'] / 4, 2)))
            max_queue_length = max(max_queue_length, queue_length)
            
            # Store metrics
            metrics.extend([
                {
                    'metric_type': 'throughput',
                    'component_type': 'network',
                    'component_id': 'aggregate',
                    'timestamp_sec': current_time,
                    'value': current_throughput,
                    'unit': 'MB/s'
                },
                {
                    'metric_type': 'latency',
                    'component_type': 'network',
                    'component_id': 'aggregate',
                    'timestamp_sec': current_time,
                    'value': current_latency,
                    'unit': 'ms'
                },
                {
                    'metric_type': 'queue_length',
                    'component_type': 'switch',
                    'component_id': 'core',
                    'timestamp_sec': current_time,
                    'value': queue_length,
                    'unit': 'count'
                }
            ])
            
            # Add some log messages
            if step % 20 == 0:  # Every 2 seconds
                logs.append({
                    'log_level': 'INFO',
                    'message': f'Simulation progress: {(step/steps)*100:.1f}% complete',
                    'component': 'simulator',
                    'simulation_time': current_time
                })
            
            # Simulate processing time (much faster than real simulation)
            await asyncio.sleep(0.01)  # 10ms per step
        
        # Store metrics and logs in database
        await self.store_metrics(job.job_id, metrics)
        await self.store_logs(job.job_id, logs)
        
        # Calculate final results
        total_throughput = sum(m['value'] for m in metrics if m['metric_type'] == 'throughput') / len([m for m in metrics if m['metric_type'] == 'throughput'])
        average_latency = sum(m['value'] for m in metrics if m['metric_type'] == 'latency') / len([m for m in metrics if m['metric_type'] == 'latency'])
        
        results = {
            'total_throughput': round(total_throughput, 6),
            'average_latency': round(average_latency, 6),
            'max_queue_length': max_queue_length
        }
        
        logger.info(f"📊 Simulation results: {results}")
        return results
    
    async def store_metrics(self, job_id: str, metrics: list):
        """Store simulation metrics in database"""
        try:
            cursor = self.db_connection.cursor()
            for metric in metrics:
                cursor.execute("""
                    INSERT INTO simulation_metrics 
                    (job_id, metric_type, component_type, component_id, timestamp_sec, value, unit)
                    VALUES (%s, %s, %s, %s, %s, %s, %s)
                """, (
                    job_id, metric['metric_type'], metric['component_type'],
                    metric['component_id'], metric['timestamp_sec'], metric['value'], metric['unit']
                ))
        except Exception as e:
            logger.error(f"💥 Failed to store metrics: {e}")
    
    async def store_logs(self, job_id: str, logs: list):
        """Store simulation logs in database"""
        try:
            cursor = self.db_connection.cursor()
            for log in logs:
                cursor.execute("""
                    INSERT INTO job_logs 
                    (job_id, log_level, message, component, simulation_time)
                    VALUES (%s, %s, %s, %s, %s)
                """, (
                    job_id, log['log_level'], log['message'], 
                    log['component'], log['simulation_time']
                ))
        except Exception as e:
            logger.error(f"💥 Failed to store logs: {e}")
    
    async def update_job_status(self, job_id: str, status: str, **kwargs):
        """Update job status in database"""
        try:
            cursor = self.db_connection.cursor()
            
            set_clauses = ['status = %s', 'worker_id = %s']
            values = [status, self.worker_id]
            
            if 'started_at' in kwargs:
                set_clauses.append('started_at = %s')
                values.append(kwargs['started_at'])
            
            if 'completed_at' in kwargs:
                set_clauses.append('completed_at = %s')
                values.append(kwargs['completed_at'])
            
            if 'error_message' in kwargs:
                set_clauses.append('error_message = %s')
                values.append(kwargs['error_message'])
            
            values.append(job_id)
            
            query = f"UPDATE simulation_jobs SET {', '.join(set_clauses)} WHERE id = %s"
            cursor.execute(query, values)
            
        except Exception as e:
            logger.error(f"💥 Failed to update job status: {e}")
    
    async def update_job_results(self, job_id: str, results: Dict[str, Any]):
        """Update job results in database"""
        try:
            cursor = self.db_connection.cursor()
            cursor.execute("""
                UPDATE simulation_jobs 
                SET total_throughput = %s, average_latency = %s, max_queue_length = %s
                WHERE id = %s
            """, (
                results['total_throughput'],
                results['average_latency'],
                results['max_queue_length'],
                job_id
            ))
        except Exception as e:
            logger.error(f"💥 Failed to update job results: {e}")
    
    async def add_job_log(self, job_id: str, level: str, message: str, component: str):
        """Add log entry for job"""
        try:
            cursor = self.db_connection.cursor()
            cursor.execute("""
                INSERT INTO job_logs (job_id, log_level, message, component)
                VALUES (%s, %s, %s, %s)
            """, (job_id, level, message, component))
        except Exception as e:
            logger.error(f"💥 Failed to add job log: {e}")
    
    async def shutdown(self):
        """Graceful shutdown"""
        logger.info("🛑 Shutting down worker...")
        self.running = False
        
        if self.db_connection:
            self.db_connection.close()
        
        if self.redis_client:
            self.redis_client.close()

async def main():
    worker = MockSimulationWorker()
    
    if not await worker.initialize():
        logger.error("💥 Failed to initialize worker")
        return
    
    try:
        await worker.process_queue()
    except KeyboardInterrupt:
        logger.info("🛑 Received shutdown signal")
    finally:
        await worker.shutdown()

if __name__ == "__main__":
    asyncio.run(main())