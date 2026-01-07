# TotalView GPT

A network monitoring system with ChatGPT integration for network administrators. This application monitors network health and performance, allowing administrators to analyze network interface errors and get AI-powered recommendations through a ChatGPT-powered assistant.

## Overview

TotalView GPT consists of:
- **Backend**: A C++ HTTP server that proxies requests to OpenAI's ChatGPT API
- **Frontend**: A Vue.js web application with **example pages** demonstrating how a persistent ChatGPT assistant button would be integrated into the real TotalView network monitoring frontend

**Note**: The frontend pages (Dashboard, Network Status, Device Details) are **example/demo pages** that showcase how the ChatGPT integration would work in the actual TotalView application. They demonstrate how the persistent Chat button appears across different monitoring screens and how network data would be collected and sent to ChatGPT.

## Requirements

This project meets the following technical requirements:

### Backend Requirements ✓
- ✅ **32-bit Windows compatible** - Built with 32-bit architecture support
- ✅ **UTF-8 encoded strings** - All string handling uses UTF-8 encoding
- ✅ **ISO C++17 compliant** - Uses C++17 standard features
- ✅ **Exception-free main routine** - Main logic handles errors without throwing exceptions
- ✅ **Log callback system** - All logging done through `void(int log_level, const std::string& message)` callback
  - Log levels: 0 (errors only), 1 (communication steps), 2 (detailed request/response)
- ✅ **No direct I/O** - All output goes through the log callback, no stdout/file writes in main code
- ✅ **API retry logic** - Handles rate limiting (HTTP 429) with exponential backoff retries

### Frontend Requirements ✓
- ✅ **Light-themed, minimal UI** - Clean, modern interface matching TotalView's aesthetic
- ✅ **Example pages** - Dashboard, Network Status, and Device Details pages demonstrating how the Chat button would persist across different screens in the real TotalView frontend
- ✅ **Persistent Chat button** - Floating button appears on all pages, demonstrating the integration pattern for TotalView
- ✅ **Network data input** - Form to input device information (brand, model, OS, interface details, error statistics) - would be auto-populated from TotalView's real network data
- ✅ **ChatGPT integration** - Communicates with backend to send prompts to ChatGPT API

## Architecture

```
┌─────────────────┐
│   Vue.js App    │
│   (Frontend)    │
│                 │
│  ┌───────────┐  │
│  │Chat Button│  │─── Always visible
│  └───────────┘  │
└────────┬────────┘
         │ HTTP POST /chat
         ▼
┌─────────────────┐
│  C++ Backend    │
│  (Port 8080)    │
│                 │
│  ┌───────────┐  │
│  │Retry Logic│  │
│  └─────┬─────┘  │
└────────┼────────┘
         │ HTTPS
         ▼
┌─────────────────┐
│  OpenAI API     │
│  (ChatGPT)      │
└─────────────────┘
```

## Project Structure

```
totalview-gpt/
├── backend/           # C++ HTTP server
│   ├── main.cpp      # Main server implementation
│   ├── main.h        # Header for embedded use
│   ├── config.json   # Configuration (API key)
│   ├── build.sh      # Linux build script
│   └── build_windows.bat  # Windows 32-bit build script
├── frontend/         # Vue.js web application (example pages)
│   ├── src/
│   │   ├── components/
│   │   │   ├── Layout.vue      # Main layout with persistent chat button
│   │   │   └── ChatWindow.vue  # ChatGPT interface
│   │   ├── views/
│   │   │   ├── Dashboard.vue      # Example page
│   │   │   ├── NetworkStatus.vue  # Example page
│   │   │   └── DeviceDetails.vue  # Example page
│   │   └── router/
│   └── package.json
└── README.md         # This file
```

## Building and Testing

### Prerequisites

- **Backend**:
  - C++17 compatible compiler (g++ or MSVC)
  - libcurl development libraries
  - OpenSSL (for HTTPS)

- **Frontend**:
  - Node.js 20.19.0+ or 22.12.0+
  - npm

- **Configuration**:
  - OpenAI API key (get one at https://platform.openai.com/)

### Backend Setup

1. **Configure API key**:
   ```bash
   cd backend
   # Edit config.json and add your OpenAI API key
   {
     "openai_api_key": "sk-your-api-key-here"
   }
   ```

2. **Build for Linux**:
   ```bash
   cd backend
   chmod +x build.sh
   ./build.sh
   ```

3. **Build for Windows (32-bit)**:
   ```cmd
   cd backend
   build_windows.bat
   ```

4. **Run the server**:
   ```bash
   ./llm_poc  # or llm_poc.exe on Windows
   ```
   
   Server will start on `http://localhost:8080`

### Frontend Setup

1. **Install dependencies**:
   ```bash
   cd frontend
   npm install
   ```

2. **Run development server**:
   ```bash
   npm run dev
   ```
   
   Frontend will be available at `http://localhost:5173` (or similar Vite port)

3. **Build for production**:
   ```bash
   npm run build
   ```

### Testing the Full Pipeline

1. **Start the backend**:
   ```bash
   cd backend
   ./llm_poc
   ```
   You should see: `Server starting on http://0.0.0.0:8080`

2. **Start the frontend**:
   ```bash
   cd frontend
   npm run dev
   ```

3. **Test the integration**:
   - Open the frontend URL in your browser
   - Navigate to any example page (Dashboard, Network Status, or Device Details)
   - **Notice**: The "ChatGPT" button (bottom-right corner) persists across all pages
   - Click the "ChatGPT" button to open the chat interface
   - Fill in the network device information form, or click "Prefill Test Data"
   - Click "Start Chat Session"
   - Verify the request is sent to the backend and ChatGPT responds
   - Navigate to different pages and verify the Chat button remains visible

4. **Verify backend logs**:
   - Backend logs will show communication steps (log level 1) if enabled
   - Errors will be logged at level 0

## Usage Example

1. Navigate to any example page (e.g., **Device Details**)
2. **Notice**: The **ChatGPT** button (floating button, bottom-right) is visible on all pages
3. Click the **ChatGPT** button to open the chat interface
4. Fill in network device information:
   - Network Brand: Cisco
   - Model: WS-C3650-24PS-E
   - OS Version: Denali 16.3.5b
   - Interface details and error statistics
   - Or use "Prefill Test Data" for quick testing
5. Click **"Start Chat Session"**
6. The system sends a formatted prompt to ChatGPT:
   ```
   You are a network administrator
   
   On a Cisco model WS-C3650-24PS-E device, it is running OS Denali 16.3.5b...
   What is causing these errors, and what is the configuration that will fix this?
   ```
7. ChatGPT responds with analysis and recommendations
8. Continue the conversation with follow-up questions
9. Navigate to other example pages - the Chat button remains accessible and the chat session persists

## API Endpoints

### `POST /chat`
Sends a chat message to ChatGPT API.

**Request Body**:
```json
{
  "user_message": "What is causing these errors?",
  "system_message": "You are a network administrator",
  "context": {
    "messages": [...]
  },
  "conversation_id": "conv_123456"
}
```

**Response**:
```json
{
  "conversation_id": "conv_123456",
  "assistant_message": "ChatGPT's response..."
}
```

### `GET /health`
Health check endpoint.

**Response**:
```json
{
  "ok": true
}
```

### `OPTIONS /*`
CORS preflight handler.

## Configuration

Backend configuration is stored in `backend/config.json`:
```json
{
  "openai_api_key": "sk-your-api-key-here"
}
```

## Development Notes

- The backend uses a log callback system for all output - no direct stdout/stderr writes
- Rate limiting is handled automatically with retry logic (HTTP 429 responses)
- The frontend Chat button persists across all pages using Vue Router and a Layout component
- **Frontend pages are examples**: The Dashboard, Network Status, and Device Details pages are example/demo pages that demonstrate how the ChatGPT integration would work in the real TotalView frontend. The Layout component and Chat button implementation can be directly integrated into the actual TotalView application.
- All strings are UTF-8 encoded throughout the system


