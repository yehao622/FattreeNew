#!/bin/bash
# Quick test to diagnose the unauthorized access issue

API_BASE="http://localhost:3000/api"

echo "🔍 Diagnosing unauthorized access test..."

echo
echo "1. Testing basic API availability:"
curl -s -w "HTTP %{http_code}\n" http://localhost:3000/api/health -o /dev/null

echo
echo "2. Testing protected routes without auth:"
for route in "/api/v1/simulations" "/api/v1/auth/profile"; do
    echo "Testing: $route"
    response=$(curl -s -w "%{http_code}" -o /tmp/test_response.json "$API_BASE${route#/api}")
    http_code=${response: -3}
    echo "  HTTP Code: $http_code"
    echo "  Response: $(cat /tmp/test_response.json 2>/dev/null | head -c 200)..."
    echo
done

echo
echo "3. Testing what routes are actually available:"
for route in "/api/docs" "/api/v1/status" "/api/v1/simulations" "/api/v1/auth/login"; do
    echo "Testing: $route"
    response=$(curl -s -w "%{http_code}" -o /dev/null "$API_BASE${route#/api}")
    http_code=${response: -3}
    echo "  HTTP Code: $http_code"
done

echo
echo "4. Check if routes are loading properly:"
curl -s http://localhost:3000/api/docs | jq '.endpoints' 2>/dev/null || echo "API docs not available"