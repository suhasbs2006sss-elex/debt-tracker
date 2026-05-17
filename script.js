// Debt Tracker Application
class DebtTracker {
    constructor() {
        this.debts = JSON.parse(localStorage.getItem('debts')) || [];
        this.currentTab = 'owed-to-me';
        this.editingDebtId = null;
        this.init();
    }

    init() {
        this.cacheElements();
        this.bindEvents();
        this.renderDebts();
        this.updateStats();
        this.requestNotificationPermission();
        this.setupServiceWorker();
    }

    cacheElements() {
        this.debtList = document.getElementById('debtList');
        this.totalOwedEl = document.getElementById('totalOwed');
        this.totalOweEl = document.getElementById('totalOwe');
        this.netBalanceEl = document.getElementById('netBalance');
        this.addDebtBtn = document.getElementById('addDebtBtn');
        this.debtModal = document.getElementById('debtModal');
        this.closeModalBtn = document.querySelector('.close-modal');
        this.debtForm = document.getElementById('debtForm');
        this.modalTitle = document.getElementById('modalTitle');
        this.toast = document.getElementById('toast');
        this.toastMessage = document.getElementById('toastMessage');
        this.tabBtns = document.querySelectorAll('.tab-btn');
    }

    bindEvents() {
        this.addDebtBtn.addEventListener('click', () => this.openModal());
        this.closeModalBtn.addEventListener('click', () => this.closeModal());
        this.debtForm.addEventListener('submit', (e) => this.handleFormSubmit(e));
        this.tabBtns.forEach(btn => {
            btn.addEventListener('click', () => this.switchTab(btn.dataset.tab));
        });
        document.addEventListener('click', (e) => {
            if (e.target === this.debtModal) {
                this.closeModal();
            }
        });
    }

    openModal(debt = null) {
        if (debt) {
            this.editingDebtId = debt.id;
            this.modalTitle.textContent = 'Edit Debt';
            document.getElementById('name').value = debt.name;
            document.getElementById('amount').value = debt.amount;
            document.getElementById('type').value = debt.type;
            document.getElementById('dueDate').value = debt.dueDate || '';
            document.getElementById('notes').value = debt.notes || '';
        } else {
            this.editingDebtId = null;
            this.modalTitle.textContent = 'Add Debt';
            this.debtForm.reset();
        }
        this.debtModal.classList.add('show');
    }

    closeModal() {
        this.debtModal.classList.remove('show');
        this.editingDebtId = null;
    }

    handleFormSubmit(e) {
        e.preventDefault();

        const name = document.getElementById('name').value;
        const amount = parseFloat(document.getElementById('amount').value);
        const type = document.getElementById('type').value;
        const dueDate = document.getElementById('dueDate').value;
        const notes = document.getElementById('notes').value;

        if (!name || isNaN(amount) || amount <= 0) {
            this.showToast('Please fill in all required fields correctly', 'error');
            return;
        }

        const debtData = {
            name,
            amount,
            type,
            dueDate,
            notes,
            createdAt: new Date().toISOString()
        };

        if (this.editingDebtId) {
            // Update existing debt
            const index = this.debts.findIndex(d => d.id === this.editingDebtId);
            if (index !== -1) {
                this.debts[index] = { ...this.debts[index], ...debtData };
                this.showToast('Debt updated successfully', 'success');
            }
        } else {
            // Create new debt
            debtData.id = Date.now().toString();
            this.debts.push(debtData);
            this.showToast('Debt added successfully', 'success');

            // Schedule notification if due date is set
            if (dueDate) {
                this.scheduleNotification(debtData);
            }
        }

        this.saveDebts();
        this.renderDebts();
        this.updateStats();
        this.closeModal();
    }

    deleteDebt(id) {
        if (confirm('Are you sure you want to delete this debt?')) {
            this.debts = this.debts.filter(debt => debt.id !== id);
            this.saveDebts();
            this.renderDebts();
            this.updateStats();
            this.showToast('Debt deleted', 'success');
        }
    }

    editDebt(id) {
        const debt = this.debts.find(d => d.id === id);
        if (debt) {
            this.openModal(debt);
        }
    }

    switchTab(tab) {
        this.currentTab = tab;
        this.tabBtns.forEach(btn => {
            btn.classList.toggle('active', btn.dataset.tab === tab);
        });
        this.renderDebts();
    }

    renderDebts() {
        const filteredDebts = this.debts.filter(debt => debt.type === this.currentTab);

        if (filteredDebts.length === 0) {
            this.debtList.innerHTML = `
                <div class="empty-state">
                    <i class="fas fa-hand-holding-usd"></i>
                    <h3>No debts</h3>
                    <p>Click the + button to add your first debt</p>
                </div>
            `;
            return;
        }

        this.debtList.innerHTML = filteredDebts.map(debt => {
            const isOverdue = debt.dueDate && new Date(debt.dueDate) < new Date() && !debt.paid;
            const dueDateHtml = debt.dueDate
                ? `<span class="due-date ${isOverdue ? 'overdue' : ''}">
                    <i class="far fa-calendar"></i> ${debt.dueDate}
                  </span>`
                : '';

            return `
                <div class="debt-item ${debt.type}">
                    <div class="debt-header">
                        <div class="debt-name">
                            <i class="fas fa-user"></i>
                            <h3>${debt.name}</h3>
                        </div>
                        <div class="debt-amount">$${debt.amount.toFixed(2)}</div>
                    </div>
                    <div class="debt-details">
                        ${dueDateHtml}
                        <span class="debt-actions">
                            <button class="action-btn edit-btn" onclick="tracker.editDebt('${debt.id}')">
                                <i class="fas fa-edit"></i>
                            </button>
                            <button class="action-btn delete-btn" onclick="tracker.deleteDebt('${debt.id}')">
                                <i class="fas fa-trash"></i>
                            </button>
                        </span>
                    </div>
                </div>
            `;
        }).join('');
    }

    updateStats() {
        const totalOwed = this.debts
            .filter(d => d.type === 'owed-to-me' && !d.paid)
            .reduce((sum, d) => sum + d.amount, 0);

        const totalOwe = this.debts
            .filter(d => d.type === 'i-owe' && !d.paid)
            .reduce((sum, d) => sum + d.amount, 0);

        const netBalance = totalOwed - totalOwe;

        this.totalOwedEl.textContent = `$${totalOwed.toFixed(2)}`;
        this.totalOweEl.textContent = `$${totalOwe.toFixed(2)}`;
        this.netBalanceEl.textContent = `$${netBalance.toFixed(2)}`;
    }

    saveDebts() {
        localStorage.setItem('debts', JSON.stringify(this.debts));
    }

    showToast(message, type = 'success') {
        this.toastMessage.textContent = message;
        this.toast.classList.add('show');

        if (type === 'error') {
            this.toastMessage.style.color = 'var(--accent-danger)';
        } else {
            this.toastMessage.style.color = 'var(--text-primary)';
        }

        setTimeout(() => {
            this.toast.classList.remove('show');
        }, 3000);
    }

    scheduleNotification(debt) {
        const dueDate = new Date(debt.dueDate);
        const now = new Date();
        const timeUntil = dueDate - now;

        // Schedule notification 24 hours before due date
        const notificationTime = dueDate.getTime() - 24 * 60 * 60 * 1000;

        if (notificationTime > now.getTime()) {
            setTimeout(() => {
                this.sendNotification(debt);
            }, notificationTime - now.getTime());
        }
    }

    sendNotification(debt) {
        if (!('Notification' in window)) {
            return;
        }

        if (Notification.permission === 'granted') {
            new Notification('Debt Reminder', {
                body: `Reminder: ${debt.name} owes you $${debt.amount.toFixed(2)}`,
                icon: 'https://cdn-icons-png.flaticon.com/512/3135/3135715.png',
                requireInteraction: true
            });
        }
    }

    requestNotificationPermission() {
        if ('Notification' in window && Notification.permission === 'default') {
            Notification.requestPermission();
        }
    }

    setupServiceWorker() {
        if ('serviceWorker' in navigator) {
            // Create a simple service worker for PWA functionality
            const swCode = `
                const CACHE_NAME = 'debttracker-v1';
                const urlsToCache = ['/index.html', '/style.css', '/script.js'];

                self.addEventListener('install', event => {
                    event.waitUntil(
                        caches.open(CACHE_NAME)
                            .then(cache => cache.addAll(urlsToCache))
                    );
                });

                self.addEventListener('fetch', event => {
                    event.respondWith(
                        caches.match(event.request)
                            .then(response => response || fetch(event.request))
                    );
                });
            `;

            const blob = new Blob([swCode], { type: 'application/javascript' });
            const swUrl = URL.createObjectURL(blob);

            navigator.serviceWorker.register(swUrl)
                .then(registration => {
                    console.log('SW registered: ', registration);
                })
                .catch(registrationError => {
                    console.log('SW registration failed: ', registrationError);
                });
        }
    }
}

// Initialize the app
const tracker = new DebtTracker();