const API_BASE_URL = 'http://localhost:8080';

/**
 * Make API calls to the backend
 * @param {string} endpoint - API endpoint (e.g., '/api/auth/login')
 * @param {string} method - HTTP method (GET, POST, PUT, DELETE)
 * @param {object} data - Request body data (optional)
 * @param {string} token - JWT token for authentication (optional)
 * @returns {Promise} - Response data
 */
async function apiCall(endpoint, method = 'GET', data = null, token = null) {
    const options = {
        method: method,
        headers: {
            'Content-Type': 'application/json'
        }
    };

    // Add authorization header if token provided
    if (token) {
        options.headers['Authorization'] = `Bearer ${token}`;
    }

    // Add request body for POST, PUT, PATCH methods
    if (data && ['POST', 'PUT', 'PATCH'].includes(method)) {
        options.body = JSON.stringify(data);
    }

    try {
        const response = await fetch(API_BASE_URL + endpoint, options);
        
        // Handle non-JSON responses
        const contentType = response.headers.get('content-type');
        let responseData;
        
        if (contentType && contentType.includes('application/json')) {
            responseData = await response.json();
        } else {
            responseData = await response.text();
        }

        // Check if response is successful
        if (!response.ok) {
            const errorMessage = typeof responseData === 'object' 
                ? responseData.message || responseData.error || 'Unknown error'
                : responseData || `HTTP ${response.status}`;
            throw new Error(errorMessage);
        }

        return responseData;
    } catch (error) {
        console.error(`API call failed: ${endpoint}`, error);
        throw error;
    }
}

/**
 * Authentication API calls
 */
const AuthAPI = {
    /**
     * Register a new user
     * @param {string} username - Username
     * @param {string} email - Email address
     * @param {string} password - Password
     * @returns {Promise} - Contains token on success
     */
    register: async (username, email, password) => {
        return apiCall('/api/auth/register', 'POST', {
            username,
            email,
            password
        });
    },

    /**
     * Login user
     * @param {string} username - Username
     * @param {string} password - Password
     * @returns {Promise} - Contains token on success
     */
    login: async (username, password) => {
        return apiCall('/api/auth/login', 'POST', {
            username,
            password
        });
    },

    /**
     * Validate JWT token
     * @param {string} token - JWT token
     * @returns {Promise} - Contains validation result
     */
    validateToken: async (token) => {
        return apiCall('/api/auth/validate', 'POST', { token });
    }
};

/**
 * Sweets API calls
 */
const SweetsAPI = {
    /**
     * Get all sweets
     * @returns {Promise} - Array of sweet objects
     */
    getAll: async () => {
        return apiCall('/api/sweets', 'GET');
    },

    /**
     * Get a single sweet by ID
     * @param {number} sweetId - Sweet ID
     * @returns {Promise} - Sweet object
     */
    getById: async (sweetId) => {
        return apiCall(`/api/sweets/${sweetId}`, 'GET');
    },

    /**
     * Search sweets
     * @param {string} query - Search query
     * @param {string} category - Category filter
     * @param {number} minPrice - Minimum price
     * @param {number} maxPrice - Maximum price
     * @returns {Promise} - Array of matching sweets
     */
    search: async (query, category = '', minPrice = 0, maxPrice = Infinity) => {
        const params = new URLSearchParams({
            q: query,
            category: category,
            minPrice: minPrice,
            maxPrice: maxPrice
        });
        return apiCall(`/api/sweets/search?${params.toString()}`, 'GET');
    },

    /**
     * Create a new sweet (admin only)
     * @param {object} sweetData - Sweet object with name, description, category, price, quantity
     * @param {string} token - JWT token
     * @returns {Promise} - Created sweet object
     */
    create: async (sweetData, token) => {
        return apiCall('/api/sweets', 'POST', sweetData, token);
    },

    /**
     * Update a sweet (admin only)
     * @param {number} sweetId - Sweet ID
     * @param {object} sweetData - Updated sweet data
     * @param {string} token - JWT token
     * @returns {Promise} - Updated sweet object
     */
    update: async (sweetId, sweetData, token) => {
        return apiCall(`/api/sweets/${sweetId}`, 'PUT', sweetData, token);
    },

    /**
     * Delete a sweet (admin only)
     * @param {number} sweetId - Sweet ID
     * @param {string} token - JWT token
     * @returns {Promise} - Deletion confirmation
     */
    delete: async (sweetId, token) => {
        return apiCall(`/api/sweets/${sweetId}`, 'DELETE', null, token);
    },

    /**
     * Purchase a sweet
     * @param {number} sweetId - Sweet ID
     * @param {number} quantity - Quantity to purchase
     * @param {string} token - JWT token
     * @returns {Promise} - Purchase confirmation with total price
     */
    purchase: async (sweetId, quantity, token) => {
        return apiCall(`/api/sweets/${sweetId}/purchase`, 'POST', {
            quantity: quantity
        }, token);
    },

    /**
     * Restock a sweet (admin only)
     * @param {number} sweetId - Sweet ID
     * @param {number} quantity - Quantity to add
     * @param {string} token - JWT token
     * @returns {Promise} - Restock confirmation
     */
    restock: async (sweetId, quantity, token) => {
        return apiCall(`/api/sweets/${sweetId}/restock`, 'POST', {
            quantity: quantity
        }, token);
    }
};

/**
 * User purchases history API
 */
const PurchasesAPI = {
    /**
     * Get user's purchase history
     * @param {string} token - JWT token
     * @returns {Promise} - Array of purchase objects
     */
    getUserHistory: async (token) => {
        return apiCall('/api/purchases/history', 'GET', null, token);
    },

    /**
     * Get purchase details by ID
     * @param {number} purchaseId - Purchase ID
     * @param {string} token - JWT token
     * @returns {Promise} - Purchase object
     */
    getById: async (purchaseId, token) => {
        return apiCall(`/api/purchases/${purchaseId}`, 'GET', null, token);
    }
};

/**
 * Admin statistics API
 */
const StatsAPI = {
    /**
     * Get dashboard statistics (admin only)
     * @param {string} token - JWT token
     * @returns {Promise} - Statistics object
     */
    getDashboard: async (token) => {
        return apiCall('/api/admin/stats', 'GET', null, token);
    },

    /**
     * Get sales report (admin only)
     * @param {string} startDate - Start date (YYYY-MM-DD)
     * @param {string} endDate - End date (YYYY-MM-DD)
     * @param {string} token - JWT token
     * @returns {Promise} - Sales report data
     */
    getSalesReport: async (startDate, endDate, token) => {
        const params = new URLSearchParams({
            startDate: startDate,
            endDate: endDate
        });
        return apiCall(`/api/admin/sales?${params.toString()}`, 'GET', null, token);
    }
};

/**
 * Utility function to check if a token is still valid
 * @param {string} token - JWT token
 * @returns {boolean} - True if token is valid and not expired
 */
function isTokenValid(token) {
    if (!token) return false;
    
    try {
        const payload = JSON.parse(atob(token.split('.')[1]));
        if (payload.exp) {
            const expiryTime = payload.exp * 1000; // Convert to milliseconds
            return Date.now() < expiryTime;
        }
        return true;
    } catch (err) {
        console.error('Token validation error:', err);
        return false;
    }
}

/**
 * Extract user info from JWT token
 * @param {string} token - JWT token
 * @returns {object} - User object with username, email, isAdmin, exp
 */
function getUserFromToken(token) {
    if (!token) return null;
    
    try {
        const payload = JSON.parse(atob(token.split('.')[1]));
        return {
            username: payload.username || '',
            email: payload.email || '',
            isAdmin: payload.is_admin === 'true' || payload.is_admin === true,
            userId: payload.user_id || 0,
            expiresAt: payload.exp ? new Date(payload.exp * 1000) : null
        };
    } catch (err) {
        console.error('Token decode error:', err);
        return null;
    }
}

/**
 * Format price to currency string
 * @param {number} price - Price value
 * @returns {string} - Formatted price (e.g., "$12.99")
 */
function formatPrice(price) {
    return '$' + parseFloat(price).toFixed(2);
}

/**
 * Format date to readable format
 * @param {string|Date} date - Date value
 * @returns {string} - Formatted date
 */
function formatDate(date) {
    const d = new Date(date);
    return d.toLocaleDateString('en-US', {
        year: 'numeric',
        month: 'short',
        day: 'numeric',
        hour: '2-digit',
        minute: '2-digit'
    });
}

/**
 * Debounce function for search input
 * @param {function} func - Function to debounce
 * @param {number} delay - Delay in milliseconds
 * @returns {function} - Debounced function
 */
function debounce(func, delay = 300) {
    let timeoutId;
    return function(...args) {
        clearTimeout(timeoutId);
        timeoutId = setTimeout(() => func.apply(this, args), delay);
    };
}