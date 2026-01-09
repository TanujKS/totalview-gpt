# TotalView GPT Frontend

Vue.js web application demonstrating ChatGPT integration for network administrators. **This frontend contains example pages** that showcase how the persistent ChatGPT assistant button would work in the real TotalView network monitoring application.

## Overview

This frontend is a **proof-of-concept demonstration** of how ChatGPT integration would be integrated into the actual TotalView network monitoring frontend. The pages (Dashboard, Network Status, Device Details) are **example pages** that demonstrate how the persistent ChatGPT assistant button would appear and function across different screens in the real TotalView application.

The implementation demonstrates:
- How the Chat button persists across all pages
- How network device data is collected and formatted for ChatGPT
- How the chat interface integrates seamlessly with the monitoring UI

**Note**: These are example/demo pages, not the actual TotalView frontend. The Layout component and Chat button implementation can be integrated into the real TotalView application to provide the same persistent ChatGPT functionality.

## Key Features

- **Light-themed, minimal UI**: Clean, modern design with light color scheme
- **Persistent ChatGPT button**: Floating action button visible on all pages
- **Network data input form**: Comprehensive form for entering device information
- **Multi-turn conversations**: Maintains conversation history during chat sessions
- **Test data prefilling**: Quick "Prefill" button for testing with sample data


## How the Persistent Chat Button Works

The ChatGPT button persists across all pages using a **Layout Component Pattern**:

### Architecture

```
App.vue
  └── Layout.vue (persistent wrapper)
      ├── Header (with navigation)
      ├── <router-view /> (page content changes)
      └── ChatButton (always visible)
          └── ChatWindow.vue (modal overlay)
```

### Implementation Details

1. **Layout.vue Component** (`src/components/Layout.vue`):
   - Wraps all page content
   - Contains the persistent chat button as a fixed position element
   - Manages chat window state (`chatOpen`)
   - Never unmounts when navigating between pages

2. **Router Configuration** (`src/router/index.js`):
   - All routes render within the Layout component
   - Navigation changes only the `<router-view />` content
   - Layout remains mounted throughout

3. **Chat Button**:
   - **Fixed positioning**: `position: fixed; bottom: 24px; right: 24px;`
   - **High z-index**: Appears above all page content (`z-index: 999`)
   - **Always rendered**: Part of Layout, not individual pages
   - **State management**: Uses Vue's `ref()` for reactive chat window state

4. **ChatWindow Component**:
   - Modal overlay that appears when button is clicked
   - Full-screen overlay (`position: fixed` covering entire viewport)
   - Controlled by `isOpen` prop from Layout component
   - Emits `close` event to hide itself

### Code Flow

```javascript
// Layout.vue
const chatOpen = ref(false)  // State lives in Layout

const openChat = () => {
  chatOpen.value = true      // Opens chat window
}

// ChatWindow receives prop and emits events
<ChatWindow :isOpen="chatOpen" @close="closeChat" />
```

**Why this works**:
- Layout component is parent to all routes
- Vue Router only replaces `<router-view />` content
- Layout (and chat button) persist across route changes
- State is managed at Layout level, not page level


## ChatGPT Integration

### Data Collection

The chat interface collects:
- Network Brand (e.g., Cisco)
- Network Model (e.g., WS-C3650-24PS-E)
- OS Version (e.g., Denali 16.3.5b)
- Interface Number
- Interface Type (e.g., ethernetCsmacd)
- Speed (bits per second)
- Utilization percentages (TX/RX)
- Error statistics (Inbound Errors, Outbound Discards, Deferred Transmissions, FCS Errors)

### Prompt Formatting

The frontend formats the data into a structured prompt:
```
You are a network administrator

On a {brand} model {model} device, it is running OS {osVersion}. 
Interface #{interfaceNumber} is a {interfaceType} type of interface 
and is running at {speed} bits per second. 
It's utilization is {txUtilization}% transmit and {rxUtilization}% receive. 
It is showing {inboundErrors} Inbound Error, {outboundDiscards} Outbound Discards, 
{deferredTransmissions} Deferred Transmissions, and {fcsErrors} FCS Errors.

What is causing these errors, and what is the configuration that will fix this?
```

### API Communication

- **Endpoint**: `http://localhost:8080/chat`
- **Method**: POST
- **Request Format**:
  ```json
  {
    "user_message": "...",
    "system_message": "You are a network administrator",
    "context": {
      "messages": [...conversation history...]
    },
    "conversation_id": "conv_123456"
  }
  ```
- **Response Handling**: Displays assistant responses and maintains conversation history

## Development

### Setup

```bash
npm install
```

### Development Server

```bash
npm run dev
```

Starts Vite dev server with hot-reload (typically at `http://localhost:5173`)

### Production Build

```bash
npm run build
```

Builds optimized production bundle to `dist/` directory.

### Dependencies

- **Vue 3**: Reactive frontend framework
- **Vue Router**: Client-side routing
- **Vite**: Build tool and dev server

## Styling

- **Color Scheme**: Light theme with whites, light grays, and blue accents
- **Typography**: System font stack for native look
- **Layout**: Flexbox and CSS Grid for responsive design
- **Components**: Scoped styles per component

## Browser Support

Modern browsers that support:
- ES6+ JavaScript
- CSS Grid and Flexbox
- Fetch API

Recommended: Chrome, Firefox, Edge (latest versions)

## Testing the Chat Feature

1. Ensure backend is running on `http://localhost:8080`
2. Navigate to any page
3. Click the floating "ChatGPT" button (bottom-right)
4. Use "Prefill Test Data" button to quickly populate form
5. Click "Start Chat Session"
6. Verify backend receives request and ChatGPT responds
7. Continue conversation with follow-up questions
