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
  backendHost: 'https://totalview.tanuj.xyz',
  
  // Build the full backend URL
  get backendUrl() {
    return `${this.backendHost}`
  },
  
  // Backend API endpoints
  endpoints: {
    chat: '/chat',
    health: '/health'
  },
  
  // Get full API endpoint URL
  getApiUrl(endpoint) {
    return `${this.backendUrl}${endpoint}`
  },

  // VM Manager (Cloud Run) - manages GCE instance for the chat backend
  // Override with VITE_VM_MANAGER_URL and VITE_VM_MANAGER_API_KEY
  vmManagerUrl: 'https://totalview-manager-535805016190.us-central1.run.app',
  vmManagerApiKey: import.meta.env.VITE_VM_MANAGER_API_KEY || '',
  getVmManagerUrl(path) {
    const base = this.vmManagerUrl.replace(/\/$/, '')
    return `${base}${path?.startsWith('/') ? path : `/${path || ''}`}`
  }
}

// Export default for convenience
export default config

