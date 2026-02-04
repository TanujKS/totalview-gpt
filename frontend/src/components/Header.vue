<template>
  <header class="header">
    <div class="header-content">
      <h1 class="logo">TotalView</h1>
      <nav class="nav">
        <router-link to="/" class="nav-link">Dashboard</router-link>
        <router-link to="/network" class="nav-link">Network Status</router-link>
        <router-link to="/device" class="nav-link">Device Details</router-link>
      </nav>

      <div v-if="config.vmManagerUrl" class="vm-controls">
        <span class="vm-label">Windows Server VM (GPT Server)</span>
        <div class="status-pill-wrapper">
          <span
            :class="['status-pill', statusClass]"
            :title="statusTooltip"
          >
            {{ displayStatus }}
          </span>
          <button
            class="refresh-btn"
            @click="fetchStatus"
            :disabled="statusLoading"
            title="Refresh GPT Server VM status"
          >
            <svg
              width="14"
              height="14"
              viewBox="0 0 24 24"
              fill="none"
              stroke="currentColor"
              stroke-width="2"
              :class="{ spin: statusLoading }"
            >
              <path d="M23 4v6h-6M1 20v-6h6" />
              <path d="M3.51 9a9 9 0 0 1 14.85-3.36L23 10M1 14l4.64 4.36A9 9 0 0 0 20.49 15" />
            </svg>
          </button>
        </div>
        <button
          class="vm-btn start-btn"
          :disabled="startDisabled"
          @click="startVm"
          title="Start Windows Server VM (GPT Server)"
        >
          Start
        </button>
        <button
          class="vm-btn stop-btn"
          :disabled="stopDisabled"
          @click="stopVm"
          title="Stop Windows Server VM (GPT Server)"
        >
          Stop
        </button>
        <button class="vm-btn logout-btn" @click="logout" title="Sign out">Sign out</button>
      </div>
    </div>
  </header>
</template>

<script setup>
import { ref, computed, onMounted } from 'vue'
import config from '../config.js'
import { getAuthPassword, clearAuth } from '../auth.js'

const vmStatus = ref(null)
const statusError = ref(null)
const statusLoading = ref(false)
const actionLoading = ref(false)

const displayStatus = computed(() => {
  if (statusError.value) return 'Error'
  if (statusLoading.value && !vmStatus.value) return '…'
  return vmStatus.value?.status ?? '—'
})

const statusClass = computed(() => {
  const s = (vmStatus.value?.status ?? '').toUpperCase()
  if (statusError.value) return 'status-error'
  if (['RUNNING'].includes(s)) return 'status-running'
  if (['TERMINATED', 'STOPPED'].includes(s)) return 'status-stopped'
  if (['STOPPING', 'STAGING', 'PROVISIONING'].includes(s)) return 'status-transitioning'
  return 'status-unknown'
})

const statusTooltip = computed(() => {
  if (statusError.value) return statusError.value
  const s = vmStatus.value
  return s ? `${s.status}${s.name ? ` — ${s.name}` : ''} — Windows Server VM (GPT Server)` : 'Click refresh to fetch Windows Server VM (GPT Server) status'
})

const isTransitioning = computed(() => statusClass.value === 'status-transitioning')

const startDisabled = computed(
  () => actionLoading.value || statusLoading.value || isTransitioning.value || vmStatus.value?.status === 'RUNNING'
)

const stopDisabled = computed(
  () => actionLoading.value || statusLoading.value || isTransitioning.value || vmStatus.value?.status !== 'RUNNING'
)

function vmApiKey() {
  return getAuthPassword() || config.vmManagerApiKey || ''
}

function logout() {
  clearAuth()
  window.location.href = '/login'
}

async function fetchStatus() {
  if (!config.vmManagerUrl) return
  const key = vmApiKey()
  if (!key) return
  statusLoading.value = true
  statusError.value = null
  try {
    const res = await fetch(config.getVmManagerUrl('/status'), {
      headers: { 'x-api-key': key }
    })
    const data = await res.json()
    if (!data.ok) throw new Error(data.error ?? 'Request failed')
    vmStatus.value = data
  } catch (e) {
    statusError.value = e.message
    vmStatus.value = null
  } finally {
    statusLoading.value = false
  }
}

async function startVm() {
  if (startDisabled.value) return
  actionLoading.value = true
  statusError.value = null
  try {
    const res = await fetch(config.getVmManagerUrl('/start'), {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'x-api-key': vmApiKey()
      }
    })
    const data = await res.json()
    if (!data.ok) throw new Error(data.error ?? 'Request failed')
    await fetchStatus()
  } catch (e) {
    statusError.value = e.message
  } finally {
    actionLoading.value = false
  }
}

async function stopVm() {
  if (stopDisabled.value) return
  actionLoading.value = true
  statusError.value = null
  try {
    const res = await fetch(config.getVmManagerUrl('/stop'), {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'x-api-key': vmApiKey()
      }
    })
    const data = await res.json()
    if (!data.ok) throw new Error(data.error ?? 'Request failed')
    await fetchStatus()
  } catch (e) {
    statusError.value = e.message
  } finally {
    actionLoading.value = false
  }
}

onMounted(() => {
  if (config.vmManagerUrl) fetchStatus()
})
</script>

<style scoped>
.header {
  background: white;
  border-bottom: 1px solid #e0e0e0;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.05);
}

.header-content {
  max-width: 1200px;
  margin: 0 auto;
  padding: 16px 24px;
  display: flex;
  justify-content: space-between;
  align-items: center;
  flex-wrap: wrap;
  gap: 16px;
}

.logo {
  margin: 0;
  font-size: 24px;
  font-weight: 600;
  color: #333;
}

.nav {
  display: flex;
  gap: 24px;
}

.nav-link {
  text-decoration: none;
  color: #666;
  font-weight: 500;
  padding: 8px 0;
  transition: color 0.2s;
  border-bottom: 2px solid transparent;
}

.nav-link:hover {
  color: #4a90e2;
}

.nav-link.router-link-active {
  color: #4a90e2;
  border-bottom-color: #4a90e2;
}

.vm-controls {
  display: flex;
  align-items: center;
  gap: 12px;
}

.vm-label {
  font-size: 12px;
  color: #666;
  font-weight: 500;
  white-space: nowrap;
}

.status-pill-wrapper {
  display: flex;
  align-items: center;
  gap: 6px;
}

.status-pill {
  font-size: 12px;
  font-weight: 600;
  padding: 6px 12px;
  border-radius: 20px;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.status-running {
  background: #d4edda;
  color: #155724;
}

.status-stopped {
  background: #e9ecef;
  color: #6c757d;
}

.status-transitioning {
  background: #fff3cd;
  color: #856404;
}

.status-error {
  background: #f8d7da;
  color: #721c24;
}

.status-unknown {
  background: #e2e3e5;
  color: #383d41;
}

.refresh-btn {
  background: none;
  border: none;
  padding: 4px;
  cursor: pointer;
  color: #666;
  border-radius: 4px;
  display: flex;
  align-items: center;
  justify-content: center;
}

.refresh-btn:hover:not(:disabled) {
  background: #f0f0f0;
  color: #333;
}

.refresh-btn:disabled {
  cursor: not-allowed;
  opacity: 0.6;
}

.refresh-btn .spin {
  animation: spin 0.8s linear infinite;
}

@keyframes spin {
  from { transform: rotate(0deg); }
  to { transform: rotate(360deg); }
}

.vm-btn {
  font-size: 13px;
  font-weight: 500;
  padding: 6px 14px;
  border-radius: 6px;
  cursor: pointer;
  transition: all 0.2s;
  border: 1px solid transparent;
}

.vm-btn:disabled {
  cursor: not-allowed;
  opacity: 0.5;
}

.start-btn {
  background: #28a745;
  color: white;
  border-color: #28a745;
}

.start-btn:hover:not(:disabled) {
  background: #218838;
  border-color: #1e7e34;
}

.stop-btn {
  background: #dc3545;
  color: white;
  border-color: #dc3545;
}

.stop-btn:hover:not(:disabled) {
  background: #c82333;
  border-color: #bd2130;
}

.logout-btn {
  background: #6c757d;
  color: white;
  border-color: #6c757d;
  margin-left: 8px;
}

.logout-btn:hover:not(:disabled) {
  background: #5a6268;
  border-color: #545b62;
}

@media (max-width: 768px) {
  .header-content {
    flex-direction: column;
    align-items: stretch;
    gap: 16px;
    padding: 12px 16px;
  }

  .nav {
    flex-direction: column;
    gap: 8px;
    align-items: stretch;
  }

  .nav-link {
    padding: 10px 0;
  }

  .vm-controls {
    flex-direction: column;
    align-items: stretch;
    gap: 12px;
  }

  .vm-label {
    white-space: normal;
  }

  .status-pill-wrapper {
    justify-content: flex-start;
  }

  .vm-controls .vm-btn {
    margin-left: 0;
  }

  .logout-btn {
    margin-left: 0;
  }
}
</style>
