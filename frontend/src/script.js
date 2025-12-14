class SweetShopApp {
    constructor() {
        this.token = localStorage.getItem('authToken') || '';
        this.currentUser = null;
        this.allSweets = [];
        this.filteredSweets = [];
        this.init();
    }

    init() {
        if (this.token) {
            this.loadUserFromToken();
        } else {
            this.showAuthSection();
        }
    }

    loadUserFromToken() {
        try {
            const payload = JSON.parse(atob(this.token.split('.')[1]));
            this.currentUser = {
                username: payload.username,
                email: payload.email || '',
                isAdmin: payload.is_admin === 'true' || payload.is_admin === true,
                userId: payload.user_id || 0
            };
            this.showDashboard();
            this.loadAllSweets();
        } catch (err) {
            console.error('Token decode failed:', err);
            this.logout();
        }
    }

    showAuthSection() {
        document.getElementById('authFormsContainer').style.display = 'flex';
        document.getElementById('dashboardContainer').style.display = 'none';
        document.getElementById('adminPanel').style.display = 'none';
        document.getElementById('authSection').style.display = 'flex';
        document.getElementById('userSection').style.display = 'none';
    }

    showDashboard() {
        document.getElementById('authFormsContainer').style.display = 'none';
        document.getElementById('dashboardContainer').style.display = 'block';
        document.getElementById('authSection').style.display = 'none';
        document.getElementById('userSection').style.display = 'flex';
        document.getElementById('userGreeting').textContent = `Welcome, ${this.currentUser.username}!`;

        if (this.currentUser.isAdmin) {
            document.getElementById('adminBtn').style.display = 'block';
        }
    }

    async loadAllSweets() {
        try {
            const sweets = await apiCall('/api/sweets', 'GET');
            if (Array.isArray(sweets)) {
                this.allSweets = sweets;
                this.filteredSweets = sweets;
                this.renderSweets(sweets);
            }
        } catch (err) {
            console.error('Failed to load sweets:', err);
            this.showToast('Failed to load sweets', 'error');
        }
    }

    renderSweets(sweets) {
        const grid = document.getElementById('sweetsGrid');
        grid.innerHTML = '';

        if (sweets.length === 0) {
            grid.innerHTML = '<p style="grid-column: 1/-1; text-align: center; padding: 2rem; color: #999;">No sweets found</p>';
            return;
        }

        sweets.forEach(sweet => {
            const card = this.createSweetCard(sweet);
            grid.appendChild(card);
        });
    }

    createSweetCard(sweet) {
        const card = document.createElement('div');
        card.className = 'sweet-card';

        const qty = parseInt(sweet.quantity) || 0;
        const quantityClass = qty === 0 ? 'out-of-stock' : qty < 10 ? 'low' : '';
        const isOutOfStock = qty === 0;

        let adminActions = '';
        if (this.currentUser && this.currentUser.isAdmin) {
            adminActions = `
                <div style="margin-top: 0.5rem;">
                    <button class="edit-btn" onclick="app.editSweet(${sweet.id})">Edit</button>
                    <button class="delete-btn" onclick="app.deleteSweet(${sweet.id})">Delete</button>
                </div>
            `;
        }

        card.innerHTML = `
            <div class="sweet-card-header">
                <div class="sweet-card-title">${this.escapeHtml(sweet.name)}</div>
                <div class="sweet-card-category">${this.escapeHtml(sweet.category)}</div>
            </div>
            <div class="sweet-card-body">
                <p class="sweet-card-description">${this.escapeHtml(sweet.description || 'No description available')}</p>
                <div class="sweet-card-price">$${parseFloat(sweet.price).toFixed(2)}</div>
                <div class="sweet-card-quantity">
                    <span class="quantity-label">Stock Available:</span>
                    <span class="quantity-value ${quantityClass}">${qty}</span>
                </div>
            </div>
            <div class="sweet-card-footer">
                <button class="purchase-btn" 
                        onclick="app.purchaseSweet(${sweet.id})" 
                        ${isOutOfStock ? 'disabled' : ''}>
                    ${isOutOfStock ? 'Out of Stock' : 'Purchase'}
                </button>
                ${adminActions}
            </div>
        `;

        return card;
    }

    escapeHtml(text) {
        const map = {
            '&': '&amp;',
            '<': '&lt;',
            '>': '&gt;',
            '"': '&quot;',
            "'": '&#039;'
        };
        return text.replace(/[&<>"']/g, m => map[m]);
    }

    async purchaseSweet(sweetId) {
        const sweet = this.allSweets.find(s => s.id == sweetId);
        if (!sweet) {
            this.showToast('Sweet not found', 'error');
            return;
        }

        const quantity = prompt(`Enter quantity to purchase (Available: ${sweet.quantity}):`, '1');
        if (!quantity || isNaN(quantity) || parseInt(quantity) <= 0) {
            return;
        }

        try {
            const result = await apiCall(`/api/sweets/${sweetId}/purchase`, 'POST', {
                quantity: parseInt(quantity)
            }, this.token);

            this.showToast(`Purchase successful! Total: $${result.total_price || 0}`, 'success');
            await this.loadAllSweets();
        } catch (err) {
            this.showToast('Purchase failed: ' + err.message, 'error');
        }
    }

    editSweet(sweetId) {
        this.showToast('Edit functionality coming soon', 'info');
    }

    deleteSweet(sweetId) {
        if (!confirm('Are you sure you want to delete this sweet?')) {
            return;
        }

        apiCall(`/api/sweets/${sweetId}`, 'DELETE', {}, this.token)
            .then(() => {
                this.showToast('Sweet deleted successfully', 'success');
                this.loadAllSweets();
            })
            .catch(err => {
                this.showToast('Failed to delete sweet: ' + err.message, 'error');
            });
    }

    toggleAdminPanel() {
        const panel = document.getElementById('adminPanel');
        if (panel.style.display === 'none') {
            panel.style.display = 'block';
        } else {
            panel.style.display = 'none';
        }
    }

    showToast(message, type = 'info') {
        const toast = document.getElementById('toastNotification');
        toast.textContent = message;
        toast.className = `toast-notification show ${type}`;
        setTimeout(() => {
            toast.classList.remove('show');
        }, 3000);
    }

    logout() {
        this.token = '';
        this.currentUser = null;
        localStorage.removeItem('authToken');
        this.showAuthSection();
        document.getElementById('loginFormDiv').style.display = 'block';
        document.getElementById('registerFormDiv').style.display = 'none';
        this.clearForms();
    }

    clearForms() {
        document.getElementById('loginUsername').value = '';
        document.getElementById('loginPassword').value = '';
        document.getElementById('registerUsername').value = '';
        document.getElementById('registerEmail').value = '';
        document.getElementById('registerPassword').value = '';
        document.getElementById('loginError').textContent = '';
        document.getElementById('registerError').textContent = '';
    }
}

// Global app instance
let app;

// Initialize app on DOM load
document.addEventListener('DOMContentLoaded', () => {
    app = new SweetShopApp();
});

// UI Event Handlers
function showLoginForm() {
    document.getElementById('loginFormDiv').style.display = 'block';
    document.getElementById('registerFormDiv').style.display = 'none';
}

function showRegisterForm() {
    document.getElementById('registerFormDiv').style.display = 'block';
    document.getElementById('loginFormDiv').style.display = 'none';
}

async function performLogin(event) {
    event.preventDefault();
    const username = document.getElementById('loginUsername').value.trim();
    const password = document.getElementById('loginPassword').value.trim();
    const errorDiv = document.getElementById('loginError');

    if (!username || !password) {
        errorDiv.textContent = 'Please fill in all fields';
        errorDiv.classList.add('show');
        return;
    }

    try {
        const result = await apiCall('/api/auth/login', 'POST', {
            username,
            password
        });

        if (result.token) {
            localStorage.setItem('authToken', result.token);
            app.token = result.token;
            app.loadUserFromToken();
            app.clearForms();
            app.showToast('Login successful!', 'success');
        } else {
            throw new Error('No token received');
        }
    } catch (err) {
        errorDiv.textContent = 'Invalid username or password';
        errorDiv.classList.add('show');
    }
}

async function performRegister(event) {
    event.preventDefault();
    const username = document.getElementById('registerUsername').value.trim();
    const email = document.getElementById('registerEmail').value.trim();
    const password = document.getElementById('registerPassword').value.trim();
    const errorDiv = document.getElementById('registerError');

    if (!username || !email || !password) {
        errorDiv.textContent = 'Please fill in all fields';
        errorDiv.classList.add('show');
        return;
    }

    if (password.length < 6) {
        errorDiv.textContent = 'Password must be at least 6 characters';
        errorDiv.classList.add('show');
        return;
    }

    try {
        const result = await apiCall('/api/auth/register', 'POST', {
            username,
            email,
            password
        });

        if (result.token) {
            localStorage.setItem('authToken', result.token);
            app.token = result.token;
            app.loadUserFromToken();
            app.clearForms();
            app.showToast('Registration successful!', 'success');
        } else {
            throw new Error('No token received');
        }
    } catch (err) {
        errorDiv.textContent = 'Registration failed: ' + err.message;
        errorDiv.classList.add('show');
    }
}

function performLogout() {
    if (confirm('Are you sure you want to logout?')) {
        app.logout();
    }
}

function filterSweets() {
    const searchTerm = document.getElementById('searchInput').value.toLowerCase();
    const categoryFilter = document.getElementById('categorySelect').value;

    app.filteredSweets = app.allSweets.filter(sweet => {
        const matchesSearch = sweet.name.toLowerCase().includes(searchTerm) ||
                              sweet.description.toLowerCase().includes(searchTerm);
        const matchesCategory = !categoryFilter || sweet.category === categoryFilter;
        return matchesSearch && matchesCategory;
    });

    app.renderSweets(app.filteredSweets);
}

function resetFilters() {
    document.getElementById('searchInput').value = '';
    document.getElementById('categorySelect').value = '';
    app.renderSweets(app.allSweets);
}

async function addNewSweet(event) {
    event.preventDefault();
    const name = document.getElementById('sweetName').value.trim();
    const description = document.getElementById('sweetDescription').value.trim();
    const category = document.getElementById('sweetCategory').value;
    const price = parseFloat(document.getElementById('sweetPrice').value);
    const quantity = parseInt(document.getElementById('sweetQuantity').value);

    if (!name || !category || isNaN(price) || isNaN(quantity)) {
        app.showToast('Please fill in all required fields', 'error');
        return;
    }

    try {
        await apiCall('/api/sweets', 'POST', {
            name,
            description,
            category,
            price,
            quantity
        }, app.token);

        app.showToast('Sweet added successfully!', 'success');
        event.target.reset();
        app.loadAllSweets();
    } catch (err) {
        app.showToast('Failed to add sweet: ' + err.message, 'error');
    }
}