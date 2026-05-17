# DebtTracker - Notion Reminder Application

A beautiful, dark-themed debt tracking application with mobile-first design. Track who owes you money and who you owe, with reminder notifications.

## Features

- **Dark Aesthetic UI**: Modern dark theme with gradient accents
- **Mobile-First Design**: Optimized for smartphone use
- **Debt Tracking**: Add, edit, and delete debts
- **Two Categories**: Owed to Me and I Owe
- **Statistics**: Real-time totals and net balance
- **Reminders**: Notification alerts for due dates
- **Offline Support**: Works offline with localStorage
- **PWA Ready**: Installable on mobile devices

## Installation

### As a Web App (Mobile)
1. Open the application in your mobile browser
2. Tap the Share button
3. Select "Add to Home Screen"
4. The app will be installed and work offline

### Desktop
Simply open the HTML file in any modern browser.

## Usage

### Adding a Debt
1. Click the + button in the top right
2. Fill in the details:
   - Name of the person
   - Amount
   - Type (Owed to Me / I Owe)
   - Due Date (optional)
   - Notes (optional)
3. Click Save

### Editing or Deleting
- Click the edit or trash icon on any debt card
- Confirm deletion when prompted

### Viewing Statistics
- The top cards show:
  - Total Owed to You
  - Total You Owe
  - Net Balance

### Tabs
Switch between "Owed to Me" and "I Owe" tabs to view different categories.

## Data Storage

All data is stored locally in your browser's localStorage. No data is sent to any server.

## Notifications

- The app will request notification permission on first load
- Reminders are scheduled 24 hours before the due date
- Notifications will appear even when the app is closed (if permission granted)

## Keyboard Shortcuts

- `+` or `+` button: Open add debt modal
- `Escape`: Close modal
- `Enter`: Submit form in modal

## Browser Compatibility

Works on all modern browsers including:
- Chrome/Edge (Android, iOS, Desktop)
- Firefox
- Safari (iOS, macOS)

## Customization

The app uses CSS variables for easy theming. You can modify the colors in `style.css` under `:root`.

## License

MIT License - Feel free to use and modify as needed.