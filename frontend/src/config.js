// config.js
// Configuration for the frontend application
//
// Backend API configuration
// 
// Configuration methods (in order of precedence):
// 1. Environment variables (Vite): Create a .env file with:
//    VITE_BACKEND_HOST=localhost
//    VITE_BACKEND_PORT=3000
// 2. Directly modify the default values below
//
// Note: Environment variables in Vite must be prefixed with VITE_
//       and require restarting the dev server to take effect.

export const config = {
  // Backend server address (default: localhost)
  // Override with VITE_BACKEND_HOST environment variable
  backendHost: import.meta.env.VITE_BACKEND_HOST || 'localhost',
  
  // Backend server port (default: 3000)
  // Override with VITE_BACKEND_PORT environment variable
  // Should match the port in backend/config.json
  backendPort: import.meta.env.VITE_BACKEND_PORT || '3000',
  
  // Build the full backend URL
  get backendUrl() {
    return `http://${this.backendHost}:${this.backendPort}`
  },
  
  // Backend API endpoints
  endpoints: {
    chat: '/chat',
    health: '/health'
  },
  
  // Get full API endpoint URL
  getApiUrl(endpoint) {
    return `${this.backendUrl}${endpoint}`
  }
}

// Export default for convenience
export default config

