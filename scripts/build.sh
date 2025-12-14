#!/bin/bash

# Sweet Shop Backend Build Script
# This script builds the C++ backend using CMake

set -e  # Exit on error

echo "================================"
echo "Sweet Shop Backend Build Script"
echo "================================"
echo ""

# Get script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$( dirname "$SCRIPT_DIR" )"
BACKEND_DIR="$PROJECT_ROOT/backend"
BUILD_DIR="$BACKEND_DIR/build"

echo "Project root: $PROJECT_ROOT"
echo "Backend directory: $BACKEND_DIR"
echo "Build directory: $BUILD_DIR"
echo ""

# Check if CMakeLists.txt exists
if [ ! -f "$BACKEND_DIR/CMakeLists.txt" ]; then
    echo "ERROR: CMakeLists.txt not found in $BACKEND_DIR"
    exit 1
fi

# Create build directory if it doesn't exist
echo "Creating build directory..."
mkdir -p "$BUILD_DIR"

# Change to build directory
cd "$BUILD_DIR"

# Run CMake
echo "Running CMake..."
if ! cmake ..; then
    echo "ERROR: CMake configuration failed"
    exit 1
fi

# Build the project
echo "Building project..."
if ! cmake --build .; then
    echo "ERROR: Build failed"
    exit 1
fi

# Check if executable was created
if [ -f "$BUILD_DIR/bin/sweet_shop" ] || [ -f "$BUILD_DIR/sweet_shop" ]; then
    echo ""
    echo "================================"
    echo "BUILD SUCCESSFUL!"
    echo "================================"
    echo ""
    if [ -f "$BUILD_DIR/bin/sweet_shop" ]; then
        echo "Executable: $BUILD_DIR/bin/sweet_shop"
    else
        echo "Executable: $BUILD_DIR/sweet_shop"
    fi
    echo ""
    echo "To run the server, execute:"
    echo "  $BUILD_DIR/bin/sweet_shop"
    echo "  or"
    echo "  ./run.sh"
else
    echo "ERROR: Executable not found after build"
    exit 1
fi