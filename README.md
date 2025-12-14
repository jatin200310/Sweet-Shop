# Sweet Shop - Inventory Management System

A full-stack web application for managing a sweet shop inventory with user authentication, product management, and purchase tracking.

## Project Structure

```
sweet-shop/
├── backend/                 # C++ REST API backend
│   ├── include/            # Header files
│   │   ├── Auth.h
│   │   ├── Database.h
│   │   ├── JWT.h
│   │   ├── Sweet.h
│   │   └── crow.h
│   ├── src/                # Implementation files
│   │   ├── Auth.cpp
│   │   ├── Database.cpp
│   │   ├── JWT.cpp
│   │   ├── Sweet.cpp
│   │   ├── main.cpp
│   │   └── tests/          # Unit tests
│   └── CMakeLists.txt      # CMake build configuration
├── frontend/               # Web frontend
│   ├── public/
│   │   └── index.html      # Main HTML page
│   └── src/
│       ├── api.js          # API client functions
│       ├── script.js       # Application logic
│       └── style.css       # Styling
├── database/               # Database files
│   ├── schema.sql          # Database schema
│   └── seed_data.sql       # Sample data
├── scripts/                # Utility scripts
│   ├── build.sh            # Build backend
│   ├── run.sh              # Run application
│   └── setup_database.sh   # Setup database
└── README.md              # This file
```

## Requirements

### Backend
- C++17 or higher
- CMake 3.10+
- MySQL 8.0+
- OpenSSL development libraries
- GCC or Clang compiler

### Frontend
- Modern web browser (Chrome, Firefox, Safari, Edge)
- Live Server extension (optional, for development)

## Installation

### 1. Clone or Download Project
```bash
cd sweet-shop
```

### 2. Setup Database

Linux/Mac:
```bash
chmod +x scripts/setup_database.sh
./scripts/setup_database.sh
```

Windows (PowerShell):
```powershell
mysql -u root -p < database/schema.sql
mysql -u root -p < database/seed_data.sql
```

### 3. Build Backend

Linux/Mac:
```bash
chmod +x scripts/build.sh
./scripts/build.sh
```

Windows (Command Prompt):
```cmd
cd backend
mkdir build
cd build
cmake ..
cmake --build .
```

### 4. Configure Backend

Update database credentials in `backend/src/main.cpp`:
```cpp
Database db("127.0.0.1", "root", "your_password", "sweet_shop", 3306);
```

## Running the Application

### Option 1: Using Run Script (Linux/Mac)
```bash
chmod +x scripts/run.sh
./scripts/run.sh
```

This will:
1. Build the backend
2. Start the backend server on http://localhost:8080
3. Open the frontend in your default browser

### Option 2: Manual Startup

Terminal 1 - Start Backend:
```bash
./backend/build/sweet_shop
```

Terminal 2 - Open Frontend:
- Open `frontend/public/index.html` in a web browser
- Or use Live Server: right-click on index.html → "Open with Live Server"

## API Endpoints

### Authentication
- `POST /api/auth/register` - Register new user
- `POST /api/auth/login` - Login user
- `POST /api/auth/validate` - Validate JWT token

### Sweets
- `GET /api/sweets` - Get all sweets
- `GET /api/sweets/<id>` - Get sweet by ID
- `POST /api/sweets` - Create new sweet (admin only)
- `PUT /api/sweets/<id>` - Update sweet (admin only)
- `DELETE /api/sweets/<id>` - Delete sweet (admin only)
- `POST /api/sweets/<id>/purchase` - Purchase sweet
- `POST /api/sweets/<id>/restock` - Restock sweet (admin only)

### Purchases
- `GET /api/purchases/history` - Get user's purchase history
- `GET /api/purchases/<id>` - Get purchase details

### Admin
- `GET /api/admin/stats` - Get dashboard statistics (admin only)
- `GET /api/admin/sales` - Get sales report (admin only)

## Default Users

After running `setup_database.sh`, these users are available:

| Username | Password | Email | Role |
|----------|----------|-------|------|
| admin | (hashed) | admin@sweetshop.com | Admin |
| customer1 | (hashed) | customer1@example.com | User |
| customer2 | (hashed) | customer2@example.com | User |

Note: Passwords are hashed in the database. Refer to `database/seed_data.sql` for actual hash values.

## Features

### User Features
- User registration and login with JWT authentication
- Browse sweet products with search and filtering
- Purchase sweets with real-time inventory updates
- View purchase history
- Category-based filtering
- Price display with currency formatting

### Admin Features
- Admin dashboard access
- Add new sweet products
- Update product details (name, description, price, quantity)
- Delete products
- Restock products
- View sales statistics and reports
- User management

## Technology Stack

### Backend
- **Language**: C++17
- **Web Framework**: Crow (lightweight HTTP server)
- **Database**: MySQL
- **Authentication**: JWT (JSON Web Tokens)
- **Encryption**: OpenSSL for HMAC-SHA256
- **Build System**: CMake

### Frontend
- **HTML5**: Structure and layout
- **CSS3**: Modern responsive styling with gradients
- **JavaScript (Vanilla)**: Application logic and API calls
- **No External Dependencies**: Pure vanilla implementation

## Development

### Building from Source

```bash
cd backend
mkdir -p build
cd build
cmake ..
cmake --build .
```

### Running Tests

```bash
cd backend/build
ctest
```

### Code Structure

**Backend Architecture:**
- `Database` class: Handles MySQL connections and queries
- `Auth` class: User authentication and JWT token management
- `SweetManager` class: Sweet product operations
- `JWT` class: Token encoding, decoding, and verification
- `main.cpp`: REST API endpoints using Crow framework

**Frontend Architecture:**
- `SweetShopApp` class: Main application logic
- `api.js`: Centralized API client functions
- `script.js`: UI event handlers and interactions
- `style.css`: Responsive grid-based layout

## Configuration

### Database Configuration
Edit `backend/src/main.cpp`:
```cpp
Database db("HOST", "USER", "PASSWORD", "DATABASE", PORT);
```

### JWT Secret
Change the secret in `backend/src/main.cpp`:
```cpp
Auth auth(db, "your-secret-key-here");
```

### Server Port
Default port: `8080`
Modify in `backend/src/main.cpp` and `frontend/src/api.js`

## Troubleshooting

### MySQL Connection Failed
- Ensure MySQL is running: `sudo service mysql start` (Linux)
- Check credentials in `backend/src/main.cpp`
- Verify database exists: `mysql -u root -p -e "SHOW DATABASES;"`

### Port 8080 Already in Use
```bash
# Linux/Mac
lsof -ti:8080 | xargs kill -9

# Windows
netstat -ano | findstr :8080
taskkill /PID <PID> /F
```

### Backend Won't Build
- Ensure MySQL development headers are installed
- Install OpenSSL: `sudo apt-get install libssl-dev` (Linux)
- Verify CMake version: `cmake --version`

### Frontend Shows "Cannot Connect to API"
- Check if backend is running on http://localhost:8080
- Check browser console for CORS errors
- Verify firewall settings

## Security Notes

⚠️ **This is a demonstration project. For production use:**
- Change default JWT secret
- Implement HTTPS/TLS
- Add CSRF protection
- Implement rate limiting
- Use parameterized queries (already implemented)
- Add input validation
- Implement proper error handling
- Use environment variables for credentials

## License

This project is provided as-is for educational purposes.

## Support

For issues or questions, please review:
1. Code comments in source files
2. API endpoint documentation above
3. Troubleshooting section
4. Error messages in browser console and terminal

## Future Enhancements

- [ ] User profile management
- [ ] Order management system
- [ ] Payment processing integration
- [ ] Email notifications
- [ ] Product reviews and ratings
- [ ] Inventory alerts
- [ ] Advanced analytics dashboard
- [ ] Multi-language support
- [ ] Mobile app
- [ ] Docker containerization