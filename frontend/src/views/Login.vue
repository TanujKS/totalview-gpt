<template>
  <div class="login-page">
    <div class="login-card">
      <h1 class="login-title">TotalView</h1>
      <p class="login-subtitle">Enter the demo password to continue</p>
      <form @submit.prevent="submit" class="login-form">
        <input
          v-model="password"
          type="password"
          class="login-input"
          placeholder="Password"
          autocomplete="current-password"
          autofocus
        />
        <p v-if="error" class="login-error">{{ error }}</p>
        <button type="submit" class="login-btn" :disabled="loading">
          {{ loading ? 'Checking…' : 'Sign in' }}
        </button>
      </form>
    </div>
  </div>
</template>

<script setup>
import { ref } from 'vue'
import { useRouter, useRoute } from 'vue-router'
import config from '../config.js'
import { setAuthPassword } from '../auth.js'

const router = useRouter()
const route = useRoute()
const password = ref('')
const error = ref('')
const loading = ref(false)

async function submit() {
  error.value = ''
  if (!password.value.trim()) {
    error.value = 'Please enter the password.'
    return
  }
  loading.value = true
  try {
    const url = config.getVmManagerUrl('/auth')
    const res = await fetch(url, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ password: password.value })
    })
    const data = await res.json().catch(() => ({}))
    if (res.ok && data.ok) {
      setAuthPassword(password.value)
      const redirect = route.query.redirect || '/'
      const path = typeof redirect === 'string' ? redirect : router.resolve({ name: 'dashboard' }).href
      window.location.href = path
    } else {
      error.value = 'Invalid password.'
    }
  } catch (e) {
    error.value = 'Could not reach server. Try again.'
  } finally {
    loading.value = false
  }
}
</script>

<style scoped>
.login-page {
  min-height: 100vh;
  display: flex;
  align-items: center;
  justify-content: center;
  background: #f5f5f5;
}

.login-card {
  background: white;
  border-radius: 12px;
  padding: 32px 40px;
  box-shadow: 0 4px 20px rgba(0, 0, 0, 0.08);
  border: 1px solid #e0e0e0;
  width: 100%;
  max-width: 360px;
}

.login-title {
  margin: 0 0 8px 0;
  font-size: 24px;
  font-weight: 600;
  color: #333;
}

.login-subtitle {
  margin: 0 0 24px 0;
  font-size: 14px;
  color: #666;
}

.login-form {
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.login-input {
  padding: 12px 14px;
  font-size: 16px;
  border: 1px solid #ddd;
  border-radius: 8px;
  outline: none;
}

.login-input:focus {
  border-color: #4a90e2;
  box-shadow: 0 0 0 2px rgba(74, 144, 226, 0.2);
}

.login-error {
  margin: 0;
  font-size: 14px;
  color: #c53030;
}

.login-btn {
  padding: 12px 20px;
  font-size: 16px;
  font-weight: 500;
  color: white;
  background: #4a90e2;
  border: none;
  border-radius: 8px;
  cursor: pointer;
}

.login-btn:hover:not(:disabled) {
  background: #357abd;
}

.login-btn:disabled {
  opacity: 0.7;
  cursor: not-allowed;
}
</style>
