// PoC auth: sessionStorage key for demo password (used as x-api-key for VM manager)
const AUTH_KEY = 'tv_demo_password'

export function getAuthPassword() {
  return sessionStorage.getItem(AUTH_KEY) || null
}

export function setAuthPassword(password) {
  if (password) sessionStorage.setItem(AUTH_KEY, password)
  else sessionStorage.removeItem(AUTH_KEY)
}

export function clearAuth() {
  sessionStorage.removeItem(AUTH_KEY)
}

export function isAuthenticated() {
  return !!getAuthPassword()
}

export function getVmManagerAuthHeader() {
  const p = getAuthPassword()
  if (p) return { 'x-api-key': p }
  return {}
}
