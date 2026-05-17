// Test script for DebtTracker application
describe('DebtTracker', () => {
    let tracker;

    beforeEach(() => {
        // Clear localStorage before each test
        localStorage.clear();
        tracker = new DebtTracker();
    });

    test('should initialize with empty debts', () => {
        expect(tracker.debts.length).toBe(0);
    });

    test('should add a new debt', () => {
        const initialCount = tracker.debts.length;
        tracker.handleFormSubmit({
            preventDefault: () => {}
        });
        expect(tracker.debts.length).toBeGreaterThan(initialCount);
    });

    test('should calculate totals correctly', () => {
        // Add test debts
        tracker.debts = [
            { id: '1', name: 'Alice', amount: 100, type: 'owed-to-me', paid: false },
            { id: '2', name: 'Bob', amount: 50, type: 'i-owe', paid: false }
        ];
        tracker.updateStats();
        expect(document.getElementById('totalOwed').textContent).toBe('$100.00');
        expect(document.getElementById('totalOwe').textContent).toBe('$50.00');
        expect(document.getElementById('netBalance').textContent).toBe('$50.00');
    });

    test('should switch tabs correctly', () => {
        tracker.switchTab('i-owe');
        expect(tracker.currentTab).toBe('i-owe');
    });

    test('should delete a debt', () => {
        const debt = { id: '1', name: 'Test', amount: 100, type: 'owed-to-me', paid: false };
        tracker.debts = [debt];
        tracker.deleteDebt('1');
        expect(tracker.debts.length).toBe(0);
    });
});