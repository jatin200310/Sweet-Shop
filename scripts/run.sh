#!/bin/bash

# Sweet Shop Application Run Script
# Starts the backend server and opens the frontend

set -e  # Exit on error

echo "================================"
echo "Sweet Shop Application Launcher"
echo "================================"
echo ""

# Get script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$( dirname "$SCRIPT_DIR" )"
BACKEND_DIR="$PROJECT_ROOT/backend"
FRONTEND_DIR="$PROJECT_ROOT/frontend"
BUILD_DIR="$BACKEND_DIR/build"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if build directory exists
if [ ! -d "$BUILD_DIR" ]; then
    echo -e "${YELLOW}Build directory not found. Running build script...${NC}"
    cd "$SCRIPT_DIR"
    bash build.sh
fi

# Find the executable
EXECUTABLE=""
if [ -f "$BUILD_DIR/bin/sweet_shop" ]; then
    EXECUTABLE="$BUILD_DIR/bin/sweet_shop"
elif [ -f "$BUILD_DIR/sweet_shop" ]; then
    EXECUTABLE="$BUILD_DIR/sweet_shop"
else
    echo -e "${RED}ERROR: Backend executable not found${NC}"
    echo "Please run: ./scripts/build.sh"
    exit 1
fi

echo -e "${GREEN}Found executable: $EXECUTABLE${NC}"
echo ""

# Check if port 8080 is already in use
if lsof -Pi :8080 -sTCP:LISTEN -t >/dev/null 2>&1 ; then
    echo -e "${YELLOW}WARNING: Port 8080 is already in use${NC}"
    echo "Kill existing process? (y/n)"
    read -r response
    if [ "$response" = "y" ]; then
        lsof -ti:8080 | xargs kill -9 2>/dev/null || true
        sleep 1
    fi
fi

# Start the backend server
echo "Starting backend server..."
"$EXECUTABLE" &
BACKEND_PID=$!

# Save PID to file for later cleanup
echo $BACKEND_PID > "$SCRIPT_DIR/.backend.pid"

echo -e "${GREEN}Backend started (PID: $BACKEND_PID)${NC}"
echo ""

# Wait for server to start
sleep 2

# Check if backend is running
if ! kill -0 $BACKEND_PID 2>/dev/null; then
    echo -e "${RED}ERROR: Backend failed to start${NC}"
    exit 1
fi

echo "Backend is running on http://localhost:8080"
echo ""

# Open frontend in default browser
FRONTEND_FILE="$FRONTEND_DIR/public/index.html"
if [ -f "$FRONTEND_FILE" ]; then
    echo "Opening frontend..."
    if command -v xdg-open &> /dev/null; then
        xdg-open "file://$FRONTEND_FILE" 2>/dev/null &
    elif command -v open &> /dev/null; then
        open "file://$FRONTEND_FILE" 2>/dev/null &
    elif command -v start &> /dev/null; then
        start "file://$FRONTEND_FILE" 2>/dev/null &
    else
        echo -e "${YELLOW}Could not open browser automatically${NC}"
        echo "Open manually: file://$FRONTEND_FILE"
    fi
else
    echo -e "${YELLOW}Frontend file not found at: $FRONTEND_FILE${NC}"
fi

echo ""
echo "================================"
echo -e "${GREEN}Application Running!${NC}"
echo "================================"
echo ""
echo "Backend: http://localhost:8080"
echo "Frontend: file://$FRONTEND_FILE"
echo ""
echo "Press Ctrl+C to stop the server"
echo ""

# Keep script running
wait $BACKEND_PID