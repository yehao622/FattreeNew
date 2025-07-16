# Simulation Worker Dockerfile - OMNeT++ Environment
FROM ubuntu:20.04

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    g++ \
    bison \
    flex \
    perl \
    python3 \
    python3-pip \
    python3-dev \
    tcl8.6 \
    tk8.6 \
    libxml2-dev \
    zlib1g-dev \
    default-jre \
    doxygen \
    graphviz \
    libwebkit2gtk-4.0-37 \
    wget \
    curl \
    git \
    make \
    cmake \
    && rm -rf /var/lib/apt/lists/*

# Set environment variables
ENV OMNETPP_VERSION=6.0.1
ENV OMNETPP_ROOT=/opt/omnetpp
ENV PATH=$OMNETPP_ROOT/bin:$PATH

# Download and install OMNeT++
WORKDIR /tmp
RUN wget https://github.com/omnetpp/omnetpp/releases/download/omnetpp-${OMNETPP_VERSION}/omnetpp-${OMNETPP_VERSION}-linux-x86_64.tgz \
    && tar -xzf omnetpp-${OMNETPP_VERSION}-linux-x86_64.tgz \
    && mv omnetpp-${OMNETPP_VERSION} $OMNETPP_ROOT \
    && rm omnetpp-${OMNETPP_VERSION}-linux-x86_64.tgz

# Build OMNeT++
WORKDIR $OMNETPP_ROOT
#RUN ./configure WITH_QTENV=no WITH_OSG=no WITH_OSGEARTH=no \
#    && make -j$(nproc)

# Install Python dependencies
COPY requirements.txt /tmp/
RUN pip3 install --no-cache-dir -r /tmp/requirements.txt

# Create app user
RUN useradd --create-home --shell /bin/bash worker
USER worker
WORKDIR /home/worker

# Create application directory
RUN mkdir -p /home/worker/app
WORKDIR /home/worker/app

# Copy application code (will be mounted in development)
COPY --chown=worker:worker src/ ./src/
COPY --chown=worker:worker requirements.txt ./

# Copy legacy simulator (will be mounted in development)
RUN mkdir -p ./legacy-simulator
#COPY --chown=worker:worker legacy-simulator/ ./legacy-simulator/
RUN touch ./legacy-simulator/README.md

# Build the simulation if Makefile exists
WORKDIR /home/worker/app/legacy-simulator
RUN if [ -f Makefile ]; then make; fi

# Switch back to app directory
WORKDIR /home/worker/app

# Create results directory
RUN mkdir -p /home/worker/app/results

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=60s --retries=3 \
  CMD python3 -c "import redis; r=redis.Redis.from_url('${REDIS_URL}'); r.ping()" || exit 1

# Start the worker
CMD ["python3", "src/worker.py"]
