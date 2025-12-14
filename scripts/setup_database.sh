# Open VS Code with the workspace
#!/bin/bash

# Sweet Shop Database Setup Script
# Creates database, tables, and loads initial data

set -e  # Exit on error

echo "================================"
echo "Sweet Shop Database Setup"
echo "================================"
echo ""

# Get script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$( dirname "$SCRIPT_DIR" )"
DATABASE_DIR="$PROJECT_ROOT/database"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if database files exist
if [ ! -f "$DATABASE_DIR/schema.sql" ]; then
    echo -e "${RED}ERROR: schema.sql not found in $DATABASE_DIR${NC}"
    exit 1
fi

if [ ! -f "$DATABASE_DIR/seed_data.sql" ]; then
    echo -e "${RED}ERROR: seed_data.sql not found in $DATABASE_DIR${NC}"
    exit 1
fi

# Prompt for MySQL credentials
echo "MySQL Database Connection Setup"
echo "================================"
echo ""

read -p "MySQL Host (default: localhost): " DB_HOST
DB_HOST=${DB_HOST:-localhost}

read -p "MySQL Port (default: 3306): " DB_PORT
DB_PORT=${DB_PORT:-3306}

read -p "MySQL Username (default: root): " DB_USER
DB_USER=${DB_USER:-root}

read -sp "MySQL Password: " DB_PASSWORD
echo ""

# Test connection
echo ""
echo "Testing database connection..."
if mysql -h "$DB_HOST" -P "$DB_PORT" -u "$DB_USER" -p"$DB_PASSWORD" -e "SELECT 1" >/dev/null 2>&1; then
    echo -e "${GREEN}Connection successful!${NC}"
else
    echo -e "${RED}ERROR: Could not connect to MySQL${NC}"
    echo "Please check your credentials and try again"
    exit 1
fi

echo ""
echo "Creating database and tables..."
echo "================================"

# Create database and run schema
if mysql -h "$DB_HOST" -P "$DB_PORT" -u "$DB_USER" -p"$DB_PASSWORD" < "$DATABASE_DIR/schema.sql"; then
    echo -e "${GREEN}✓ Database schema created successfully${NC}"
else
    echo -e "${RED}ERROR: Failed to create database schema${NC}"
    exit 1
fi

# Load seed data
echo ""
echo "Loading sample data..."
if mysql -h "$DB_HOST" -P "$DB_PORT" -u "$DB_USER" -p"$DB_PASSWORD" < "$DATABASE_DIR/seed_data.sql"; then
    echo -e "${GREEN}✓ Sample data loaded successfully${NC}"
else
    echo -e "${RED}ERROR: Failed to load sample data${NC}"
    exit 1
fi

echo ""
echo "================================"
echo -e "${GREEN}Database Setup Complete!${NC}"
echo "================================"
echo ""
echo "Database Details:"
echo "  Host: $DB_HOST"
echo "  Port: $DB_PORT"
echo "  User: $DB_USER"
echo "  Database: sweet_shop"
echo ""
echo "Sample Users:"
echo "  Username: admin"
echo "  Username: customer1"
echo "  Username: customer2"
echo ""
echo "All passwords are hashed. To reset, update the database directly."
echo ""