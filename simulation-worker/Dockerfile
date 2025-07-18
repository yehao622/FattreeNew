# Session 2 MVP: Focus on API, Mock Simulations
FROM ubuntu:20.04

# Prevent interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install essential dependencies
RUN apt-get update && apt-get install -y \
    python3 \
    python3-pip \
    python3-dev \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Install Python dependencies
COPY simulation-worker/requirements.txt /tmp/
RUN pip3 install --no-cache-dir -r /tmp/requirements.txt

# Create app user
RUN useradd --create-home --shell /bin/bash worker
USER worker
WORKDIR /home/worker/app

# Copy application code
COPY --chown=worker:worker simulation-worker/src/ ./src/
COPY --chown=worker:worker simulation-worker/requirements.txt ./

# Copy legacy simulator (for future integration)
COPY --chown=worker:worker legacy-simulator/ ./legacy-simulator/

# Create results directory
RUN mkdir -p /home/worker/app/results

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=30s --retries=3 \
  CMD python3 -c "print('Worker healthy')" || exit 1

# Start the worker
CMD ["python3", "src/worker.py"]