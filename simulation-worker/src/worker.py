#!/usr/bin/env python3
"""
Fixed Simple Simulation Worker - JSON parsing issue resolved
"""

import os
import json
import time
import random
import logging
import psycopg2
from datetime import datetime

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

class SimpleSimulationWorker:
    def __init__(self):
        self.worker_id = f"simple-worker-{os.getpid()}-{int(time.time())}"
        self.db_connection = None
        self.running = False
        
    def initialize(self):
        """Initialize database connection"""
        try:
            db_url = os.getenv('DATABASE_URL', 'postgresql://hpc_user:hpc_password@postgres:5432/hpc_simulation')
            self.db_connection = psycopg2.connect(db_url)
            self.db_connection.autocommit = True
            logger.info(f"✅ Connected to PostgreSQL")
            logger.info(f"🔧 Worker initialized: {self.worker_id}")
            return True
        except Exception as e:
            logger.error(f"💥 Failed to initialize worker: {e}")
            return False
    
    def process_queue(self):
        """Main queue processing loop"""
        self.running = True
        logger.info("🚀 Starting simple queue processing...")
        
        while self.running:
            try:
                # Get job from database queue
                job_id = self.get_job_from_database()
                
                if job_id:
                    logger.info(f"🎯 Processing job: {job_id}")
                    self.process_job(job_id)
                else:
                    logger.info("😴 No jobs found, sleeping...")
                    time.sleep(5)
                
            except KeyboardInterrupt:
                logger.info("🛑 Received shutdown signal")
                self.running = False
                break
            except Exception as e:
                logger.error(f"💥 Queue processing error: {e}")
                time.sleep(10)
    
    def get_job_from_database(self):
        """Get next job from database queue"""
        try:
            cursor = self.db_connection.cursor()
            
            # Get the oldest unclaimed job
            cursor.execute("""
                UPDATE job_queue 
                SET claimed_at = NOW(), worker_id = %s
                WHERE id = (
                    SELECT id FROM job_queue 
                    WHERE claimed_at IS NULL 
                    ORDER BY queued_at ASC 
                    LIMIT 1
                    FOR UPDATE SKIP LOCKED
                )
                RETURNING job_id
            """, (self.worker_id,))
            
            result = cursor.fetchone()
            return result[0] if result else None
            
        except Exception as e:
            logger.error(f"💥 Database queue error: {e}")
            return None
    
    def process_job(self, job_id):
        """Process a simulation job"""
        try:
            # Get job details - FIXED JSON PARSING
            cursor = self.db_connection.cursor()
            cursor.execute("""
                SELECT 
                    sj.id, sj.name, sj.simulation_time, sj.num_compute_nodes, 
                    sj.work_type, sj.data_size_mb, sj.custom_parameters,
                    tt.parameters as topology_params, 
                    wp.parameters as workload_params
                FROM simulation_jobs sj
                LEFT JOIN topology_templates tt ON sj.topology_id = tt.id
                LEFT JOIN workload_patterns wp ON sj.workload_id = wp.id
                WHERE sj.id = %s AND sj.status = 'queued'
            """, (job_id,))
            
            job = cursor.fetchone()
            if not job:
                logger.warning(f"❌ Job {job_id} not found or not queued")
                return
            
            # Extract job data safely
            job_name = job[1]
            sim_time = float(job[2])
            compute_nodes = job[3]
            work_type = job[4]
            data_size = float(job[5])
            
            # Handle JSON fields safely - THIS IS THE FIX
            try:
                if job[6]:  # custom_parameters
                    if isinstance(job[6], str):
                        custom_params = json.loads(job[6])
                    else:
                        custom_params = job[6] if job[6] else {}  # Already a dict
                else:
                    custom_params = {}
            except:
                custom_params = {}
            
            try:
                if job[7]:  # topology_params
                    if isinstance(job[7], str):
                        topology_params = json.loads(job[7])
                    else:
                        topology_params = job[7] if job[7] else {}  # Already a dict
                else:
                    topology_params = {}
            except:
                topology_params = {}
            
            try:
                if job[8]:  # workload_params
                    if isinstance(job[8], str):
                        workload_params = json.loads(job[8])
                    else:
                        workload_params = job[8] if job[8] else {}  # Already a dict
                else:
                    workload_params = {}
            except:
                workload_params = {}
            
            logger.info(f"📊 Job details: {job_name}, {sim_time}s, {compute_nodes} nodes, {work_type} workload")
            
            # Update status to running
            cursor.execute("""
                UPDATE simulation_jobs 
                SET status = 'running', worker_id = %s, started_at = NOW()
                WHERE id = %s
            """, (self.worker_id, job_id))
            
            # Add log
            cursor.execute("""
                INSERT INTO job_logs (job_id, log_level, message, component)
                VALUES (%s, 'INFO', %s, 'worker')
            """, (job_id, f'Job started by {self.worker_id}'))
            
            # Run simulation
            results = self.run_simulation(job_id, job_name, sim_time, compute_nodes, work_type, data_size)
            
            # Update job with results
            cursor.execute("""
                UPDATE simulation_jobs 
                SET status = 'completed', 
                    completed_at = NOW(),
                    total_throughput = %s,
                    average_latency = %s,
                    max_queue_length = %s
                WHERE id = %s
            """, (results['throughput'], results['latency'], results['queue_length'], job_id))
            
            # Add completion log
            cursor.execute("""
                INSERT INTO job_logs (job_id, log_level, message, component)
                VALUES (%s, 'INFO', %s, 'worker')
            """, (job_id, f'Simulation completed. Throughput: {results["throughput"]:.2f} MB/s, Latency: {results["latency"]:.2f} ms'))
            
            # Remove from queue
            cursor.execute("DELETE FROM job_queue WHERE job_id = %s", (job_id,))
            
            logger.info(f"✅ Job {job_id} completed successfully - Throughput: {results['throughput']:.2f} MB/s")
            
        except Exception as e:
            logger.error(f"💥 Job processing error for {job_id}: {e}")
            import traceback
            logger.error(f"📍 Full traceback: {traceback.format_exc()}")
            
            try:
                cursor.execute("""
                    UPDATE simulation_jobs 
                    SET status = 'failed', completed_at = NOW(), error_message = %s
                    WHERE id = %s
                """, (str(e), job_id))
                cursor.execute("DELETE FROM job_queue WHERE job_id = %s", (job_id,))
                cursor.execute("""
                    INSERT INTO job_logs (job_id, log_level, message, component)
                    VALUES (%s, 'ERROR', %s, 'worker')
                """, (job_id, f'Job failed: {str(e)}'))
            except Exception as cleanup_error:
                logger.error(f"💥 Failed to update failed job status: {cleanup_error}")
    
    def run_simulation(self, job_id, job_name, sim_time, compute_nodes, work_type, data_size):
        """Run the actual simulation"""
        logger.info(f"🧮 Running simulation: {job_name} for {sim_time} seconds")
        
        cursor = self.db_connection.cursor()
        
        # Simulation parameters
        steps = max(5, int(sim_time * 2))  # 2 steps per second minimum
        step_duration = sim_time / steps
        
        # Generate realistic results based on parameters
        base_throughput = compute_nodes * 0.8  # MB/s per node
        base_latency = 20.0  # Base latency in ms
        
        # Workload type effects
        if work_type == 'read':
            throughput_multiplier = 1.2
            latency_multiplier = 0.8
        elif work_type == 'write':
            throughput_multiplier = 0.8
            latency_multiplier = 1.3
        else:  # mixed
            throughput_multiplier = 1.0
            latency_multiplier = 1.0
        
        max_queue_length = 0
        total_throughput = 0
        total_latency = 0
        
        # Run simulation steps
        for step in range(steps):
            current_time = step * step_duration
            
            # Add some randomness
            throughput_variation = random.uniform(0.8, 1.2)
            latency_variation = random.uniform(0.9, 1.1)
            
            # Calculate current metrics
            current_throughput = base_throughput * throughput_multiplier * throughput_variation
            current_latency = base_latency * latency_multiplier * latency_variation
            queue_length = max(0, int(random.gauss(compute_nodes / 4, 2)))
            
            max_queue_length = max(max_queue_length, queue_length)
            total_throughput += current_throughput
            total_latency += current_latency
            
            # Store some metrics
            if step % max(1, steps // 10) == 0:  # Store ~10 data points
                cursor.execute("""
                    INSERT INTO simulation_metrics 
                    (job_id, metric_type, component_type, timestamp_sec, value, unit)
                    VALUES (%s, %s, %s, %s, %s, %s)
                """, (job_id, 'throughput', 'network', current_time, current_throughput, 'MB/s'))
                
                cursor.execute("""
                    INSERT INTO simulation_metrics 
                    (job_id, metric_type, component_type, timestamp_sec, value, unit)
                    VALUES (%s, %s, %s, %s, %s, %s)
                """, (job_id, 'latency', 'network', current_time, current_latency, 'ms'))
                
                # Log progress
                progress = (step + 1) / steps * 100
                logger.info(f"📈 Job {job_id} progress: {progress:.1f}%")
                
                cursor.execute("""
                    INSERT INTO job_logs (job_id, log_level, message, component, simulation_time)
                    VALUES (%s, 'INFO', %s, 'simulator', %s)
                """, (job_id, f'Simulation progress: {progress:.1f}% complete', current_time))
            
            # Sleep to simulate processing time
            time.sleep(max(0.1, step_duration))
        
        # Calculate final averages
        avg_throughput = total_throughput / steps
        avg_latency = total_latency / steps
        
        results = {
            'throughput': round(avg_throughput, 6),
            'latency': round(avg_latency, 6),
            'queue_length': max_queue_length
        }
        
        logger.info(f"📊 Simulation results: {results}")
        return results
    
    def shutdown(self):
        """Shutdown worker"""
        logger.info("🛑 Shutting down worker...")
        self.running = False
        if self.db_connection:
            self.db_connection.close()

def main():
    logger.info("🚀 Starting Fixed Simple Simulation Worker")
    
    worker = SimpleSimulationWorker()
    
    if not worker.initialize():
        logger.error("💥 Failed to initialize worker")
        return
    
    try:
        worker.process_queue()
    except KeyboardInterrupt:
        logger.info("🛑 Received shutdown signal")
    finally:
        worker.shutdown()

if __name__ == "__main__":
    main()