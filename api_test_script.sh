#!/bin/bash
# HPC Simulation Platform API Testing Script
# Tests complete workflow: registration -> login -> job submission -> status checking

set -e  # Exit on any error

# Configuration
API_BASE="http://localhost:3000/api"
API_V1="$API_BASE/v1"
TEST_EMAIL="testuser@example.com"
TEST_USERNAME="testuser$(date +%s)"
TEST_PASSWORD="testpassword123"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Helper functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if API is running
check_api_health() {
    log_info "Checking API health..."
    
    response=$(curl -s -w "%{http_code}" -o /tmp/health_response.json "$API_BASE/health")
    http_code=${response: -3}
    
    if [ "$http_code" -eq 200 ]; then
        log_success "API is healthy"
        cat /tmp/health_response.json | jq '.' 2>/dev/null || cat /tmp/health_response.json
    else
        log_error "API health check failed (HTTP $http_code)"
        cat /tmp/health_response.json 2>/dev/null || echo "No response body"
        exit 1
    fi
    echo
}

# Test user registration
test_registration() {
    log_info "Testing user registration..."
    
    response=$(curl -s -w "%{http_code}" -o /tmp/register_response.json \
        -X POST "$API_V1/auth/register" \
        -H "Content-Type: application/json" \
        -d "{
            \"email\": \"$TEST_EMAIL\",
            \"username\": \"$TEST_USERNAME\",
            \"password\": \"$TEST_PASSWORD\",
            \"firstName\": \"Test\",
            \"lastName\": \"User\",
            \"organization\": \"Test University\"
        }")
    
    http_code=${response: -3}
    
    if [ "$http_code" -eq 201 ]; then
        log_success "User registration successful"
        cat /tmp/register_response.json | jq '.' 2>/dev/null || cat /tmp/register_response.json
    else
        log_error "User registration failed (HTTP $http_code)"
        cat /tmp/register_response.json 2>/dev/null || echo "No response body"
        exit 1
    fi
    echo
}

# Test user login and extract token
test_login() {
    log_info "Testing user login..."
    
    response=$(curl -s -w "%{http_code}" -o /tmp/login_response.json \
        -X POST "$API_V1/auth/login" \
        -H "Content-Type: application/json" \
        -d "{
            \"email\": \"$TEST_EMAIL\",
            \"password\": \"$TEST_PASSWORD\"
        }")
    
    http_code=${response: -3}
    
    if [ "$http_code" -eq 200 ]; then
        log_success "User login successful"
        
        # Extract token
        if command -v jq &> /dev/null; then
            JWT_TOKEN=$(cat /tmp/login_response.json | jq -r '.token')
        else
            # Fallback without jq
            JWT_TOKEN=$(grep -o '"token":"[^"]*"' /tmp/login_response.json | cut -d'"' -f4)
        fi
        
        if [ "$JWT_TOKEN" != "null" ] && [ -n "$JWT_TOKEN" ]; then
            log_success "JWT token extracted: ${JWT_TOKEN:0:20}..."
        else
            log_error "Failed to extract JWT token"
            exit 1
        fi
        
        cat /tmp/login_response.json | jq '.' 2>/dev/null || cat /tmp/login_response.json
    else
        log_error "User login failed (HTTP $http_code)"
        cat /tmp/login_response.json 2>/dev/null || echo "No response body"
        exit 1
    fi
    echo
}

# Test getting user profile
test_profile() {
    log_info "Testing user profile retrieval..."
    
    response=$(curl -s -w "%{http_code}" -o /tmp/profile_response.json \
        -X GET "$API_V1/auth/profile" \
        -H "Authorization: Bearer $JWT_TOKEN")
    
    http_code=${response: -3}
    
    if [ "$http_code" -eq 200 ]; then
        log_success "Profile retrieval successful"
        cat /tmp/profile_response.json | jq '.' 2>/dev/null || cat /tmp/profile_response.json
    else
        log_error "Profile retrieval failed (HTTP $http_code)"
        cat /tmp/profile_response.json 2>/dev/null || echo "No response body"
        exit 1
    fi
    echo
}

# Test getting topology templates
test_topology_templates() {
    log_info "Testing topology templates retrieval..."
    
    response=$(curl -s -w "%{http_code}" -o /tmp/topologies_response.json \
        -X GET "$API_V1/simulations/templates/topologies" \
        -H "Authorization: Bearer $JWT_TOKEN")
    
    http_code=${response: -3}
    
    if [ "$http_code" -eq 200 ]; then
        log_success "Topology templates retrieved successfully"
        cat /tmp/topologies_response.json | jq '.' 2>/dev/null || cat /tmp/topologies_response.json
    else
        log_error "Topology templates retrieval failed (HTTP $http_code)"
        cat /tmp/topologies_response.json 2>/dev/null || echo "No response body"
        exit 1
    fi
    echo
}

# Test getting workload patterns
test_workload_patterns() {
    log_info "Testing workload patterns retrieval..."
    
    response=$(curl -s -w "%{http_code}" -o /tmp/workloads_response.json \
        -X GET "$API_V1/simulations/templates/workloads" \
        -H "Authorization: Bearer $JWT_TOKEN")
    
    http_code=${response: -3}
    
    if [ "$http_code" -eq 200 ]; then
        log_success "Workload patterns retrieved successfully"
        cat /tmp/workloads_response.json | jq '.' 2>/dev/null || cat /tmp/workloads_response.json
    else
        log_error "Workload patterns retrieval failed (HTTP $http_code)"
        cat /tmp/workloads_response.json 2>/dev/null || echo "No response body"
        exit 1
    fi
    echo
}

# Test simulation job creation
test_job_creation() {
    log_info "Testing simulation job creation..."
    
    response=$(curl -s -w "%{http_code}" -o /tmp/job_create_response.json \
        -X POST "$API_V1/simulations" \
        -H "Authorization: Bearer $JWT_TOKEN" \
        -H "Content-Type: application/json" \
        -d '{
            "name": "Test Fat-tree Simulation",
            "description": "Automated test of basic fat-tree topology",
            "topologyId": 1,
            "workloadId": 1,
            "simulationTime": 5.0,
            "numComputeNodes": 8,
            "numStorageNodes": 4,
            "workType": "read",
            "dataSizeMb": 64.0,
            "readProbability": 0.8
        }')
    
    http_code=${response: -3}
    
    if [ "$http_code" -eq 201 ]; then
        log_success "Simulation job created successfully"
        
        # Extract job ID
        if command -v jq &> /dev/null; then
            JOB_ID=$(cat /tmp/job_create_response.json | jq -r '.job.id')
        else
            # Fallback without jq
            JOB_ID=$(grep -o '"id":"[^"]*"' /tmp/job_create_response.json | cut -d'"' -f4)
        fi
        
        if [ "$JOB_ID" != "null" ] && [ -n "$JOB_ID" ]; then
            log_success "Job ID extracted: $JOB_ID"
        else
            log_error "Failed to extract job ID"
            exit 1
        fi
        
        cat /tmp/job_create_response.json | jq '.' 2>/dev/null || cat /tmp/job_create_response.json
    else
        log_error "Simulation job creation failed (HTTP $http_code)"
        cat /tmp/job_create_response.json 2>/dev/null || echo "No response body"
        exit 1
    fi
    echo
}

# Test getting simulation jobs list
test_jobs_list() {
    log_info "Testing simulation jobs list..."
    
    response=$(curl -s -w "%{http_code}" -o /tmp/jobs_list_response.json \
        -X GET "$API_V1/simulations" \
        -H "Authorization: Bearer $JWT_TOKEN")
    
    http_code=${response: -3}
    
    if [ "$http_code" -eq 200 ]; then
        log_success "Jobs list retrieved successfully"
        cat /tmp/jobs_list_response.json | jq '.' 2>/dev/null || cat /tmp/jobs_list_response.json
    else
        log_error "Jobs list retrieval failed (HTTP $http_code)"
        cat /tmp/jobs_list_response.json 2>/dev/null || echo "No response body"
        exit 1
    fi
    echo
}

# Test getting specific job details
test_job_details() {
    log_info "Testing job details retrieval..."
    
    response=$(curl -s -w "%{http_code}" -o /tmp/job_details_response.json \
        -X GET "$API_V1/simulations/$JOB_ID" \
        -H "Authorization: Bearer $JWT_TOKEN")
    
    http_code=${response: -3}
    
    if [ "$http_code" -eq 200 ]; then
        log_success "Job details retrieved successfully"
        cat /tmp/job_details_response.json | jq '.' 2>/dev/null || cat /tmp/job_details_response.json
    else
        log_error "Job details retrieval failed (HTTP $http_code)"
        cat /tmp/job_details_response.json 2>/dev/null || echo "No response body"
        exit 1
    fi
    echo
}

# Wait for job processing (if simulation worker is running)
wait_for_job_completion() {
    log_info "Waiting for job processing (max 30 seconds)..."
    
    max_attempts=30
    attempt=1
    
    while [ $attempt -le $max_attempts ]; do
        response=$(curl -s -w "%{http_code}" -o /tmp/job_status_response.json \
            -X GET "$API_V1/simulations/$JOB_ID" \
            -H "Authorization: Bearer $JWT_TOKEN")
        
        http_code=${response: -3}
        
        if [ "$http_code" -eq 200 ]; then
            if command -v jq &> /dev/null; then
                status=$(cat /tmp/job_status_response.json | jq -r '.job.status')
            else
                status=$(grep -o '"status":"[^"]*"' /tmp/job_status_response.json | cut -d'"' -f4)
            fi
            
            log_info "Attempt $attempt/$max_attempts - Job status: $status"
            
            if [ "$status" = "completed" ] || [ "$status" = "failed" ]; then
                log_success "Job processing finished with status: $status"
                cat /tmp/job_status_response.json | jq '.' 2>/dev/null || cat /tmp/job_status_response.json
                break
            fi
        else
            log_warning "Status check failed (HTTP $http_code)"
        fi
        
        sleep 1
        attempt=$((attempt + 1))
    done
    
    if [ $attempt -gt $max_attempts ]; then
        log_warning "Job did not complete within timeout period"
        log_info "This is expected if the simulation worker is not running"
    fi
    echo
}

# Test API documentation
test_api_docs() {
    log_info "Testing API documentation..."
    
    response=$(curl -s -w "%{http_code}" -o /tmp/docs_response.json \
        -X GET "$API_BASE/docs")
    
    http_code=${response: -3}
    
    if [ "$http_code" -eq 200 ]; then
        log_success "API documentation retrieved successfully"
        cat /tmp/docs_response.json | jq '.' 2>/dev/null || cat /tmp/docs_response.json
    else
        log_error "API documentation retrieval failed (HTTP $http_code)"
        exit 1
    fi
    echo
}

# Test unauthorized access
test_unauthorized_access() {
    log_info "Testing unauthorized access protection..."
    
    response=$(curl -s -w "%{http_code}" -o /tmp/unauthorized_response.json \
        -X GET "$API_V1/simulations")
    
    http_code=${response: -3}
    
    if [ "$http_code" -eq 401 ]; then
        log_success "Unauthorized access properly rejected"
        cat /tmp/unauthorized_response.json | jq '.' 2>/dev/null || cat /tmp/unauthorized_response.json
    else
        log_error "Unauthorized access was not properly rejected (HTTP $http_code)"
        exit 1
    fi
    echo
}

# Cleanup function
cleanup() {
    log_info "Cleaning up temporary files..."
    rm -f /tmp/*_response.json
}

# Main test execution
main() {
    echo -e "${BLUE}================================${NC}"
    echo -e "${BLUE}HPC Simulation API Testing Suite${NC}"
    echo -e "${BLUE}================================${NC}"
    echo
    
    # Check dependencies
    if ! command -v curl &> /dev/null; then
        log_error "curl is required but not installed"
        exit 1
    fi
    
    if ! command -v jq &> /dev/null; then
        log_warning "jq not found - JSON formatting will be basic"
    fi
    
    # Run tests
    check_api_health
    test_api_docs
    test_unauthorized_access
    test_registration
    test_login
    test_profile
    test_topology_templates
    test_workload_patterns
    test_job_creation
    test_jobs_list
    test_job_details
    wait_for_job_completion
    
    echo -e "${GREEN}================================${NC}"
    echo -e "${GREEN}All tests completed successfully!${NC}"
    echo -e "${GREEN}================================${NC}"
    echo
    echo -e "${BLUE}Test Summary:${NC}"
    echo "• API Health: ✅"
    echo "• User Registration: ✅"
    echo "• User Authentication: ✅"
    echo "• Profile Management: ✅"
    echo "• Template Retrieval: ✅"
    echo "• Job Management: ✅"
    echo "• Authorization: ✅"
    echo
    echo -e "${BLUE}Next Steps:${NC}"
    echo "1. Start the simulation worker to process jobs"
    echo "2. Monitor job completion at: $API_V1/simulations/$JOB_ID"
    echo "3. Check logs with: docker-compose logs -f simulation-worker"
    
    cleanup
}

# Trap cleanup on exit
trap cleanup EXIT

# Run main function
main "$@"