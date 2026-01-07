<template>
  <div class="layout">
    <header class="header">
      <div class="header-content">
        <h1 class="logo">TotalView</h1>
        <nav class="nav">
          <router-link to="/" class="nav-link">Dashboard</router-link>
          <router-link to="/network" class="nav-link">Network Status</router-link>
          <router-link to="/device" class="nav-link">Device Details</router-link>
        </nav>
      </div>
    </header>

    <main class="main-content">
      <router-view />
    </main>

    <button @click="openChat" class="chat-button" title="Open ChatGPT Assistant">
      <svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
        <path d="M20 2H4C2.9 2 2 2.9 2 4V22L6 18H20C21.1 18 22 17.1 22 16V4C22 2.9 21.1 2 20 2ZM20 16H6L4 18V4H20V16Z" fill="currentColor"/>
        <path d="M7 9H17V11H7V9ZM7 12H14V14H7V12Z" fill="currentColor"/>
      </svg>
      Chat
    </button>

    <ChatWindow :isOpen="chatOpen" @close="closeChat" />
  </div>
</template>

<script setup>
import { ref } from 'vue'
import ChatWindow from './ChatWindow.vue'

const chatOpen = ref(false)

const openChat = () => {
  chatOpen.value = true
}

const closeChat = () => {
  chatOpen.value = false
}
</script>

<style scoped>
.layout {
  min-height: 100vh;
  display: flex;
  flex-direction: column;
  background: #f5f5f5;
}

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

.main-content {
  flex: 1;
  max-width: 1200px;
  width: 100%;
  margin: 0 auto;
  padding: 24px;
}

.chat-button {
  position: fixed;
  bottom: 24px;
  right: 24px;
  background: #4a90e2;
  color: white;
  border: none;
  border-radius: 50px;
  padding: 14px 24px;
  font-size: 16px;
  font-weight: 500;
  cursor: pointer;
  box-shadow: 0 4px 12px rgba(74, 144, 226, 0.4);
  display: flex;
  align-items: center;
  gap: 8px;
  transition: all 0.3s;
  z-index: 999;
}

.chat-button:hover {
  background: #357abd;
  box-shadow: 0 6px 16px rgba(74, 144, 226, 0.5);
  transform: translateY(-2px);
}

.chat-button:active {
  transform: translateY(0);
}
</style>

