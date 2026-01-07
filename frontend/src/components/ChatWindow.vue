<template>
  <div v-if="isOpen" class="chat-overlay" @click.self="closeChat">
    <div class="chat-window">
      <div class="chat-header">
        <h3>Network Administrator Assistant</h3>
        <button @click="closeChat" class="close-btn">&times;</button>
      </div>

      <div class="chat-content">
        <div class="network-data-form" v-if="showForm">
          <h4>Network Device Information</h4>
          <div class="form-grid">
            <div class="form-group">
              <label>Network Brand:</label>
              <input v-model="networkData.brand" type="text" placeholder="e.g., Cisco" />
            </div>
            <div class="form-group">
              <label>Network Model:</label>
              <input v-model="networkData.model" type="text" placeholder="e.g., WS-C3650-24PS-E" />
            </div>
            <div class="form-group">
              <label>OS Version:</label>
              <input v-model="networkData.osVersion" type="text" placeholder="e.g., Denali 16.3.5b" />
            </div>
            <div class="form-group">
              <label>Interface Number:</label>
              <input v-model="networkData.interfaceNumber" type="text" placeholder="e.g., 5" />
            </div>
            <div class="form-group">
              <label>Interface Type:</label>
              <input v-model="networkData.interfaceType" type="text" placeholder="e.g., ethernetCsmacd" />
            </div>
            <div class="form-group">
              <label>Speed (bits/sec):</label>
              <input v-model="networkData.speed" type="text" placeholder="e.g., 1000000000" />
            </div>
            <div class="form-group">
              <label>Transmit Utilization (%):</label>
              <input v-model="networkData.txUtilization" type="text" placeholder="e.g., 1.32" />
            </div>
            <div class="form-group">
              <label>Receive Utilization (%):</label>
              <input v-model="networkData.rxUtilization" type="text" placeholder="e.g., 5.32" />
            </div>
            <div class="form-group">
              <label>Inbound Errors:</label>
              <input v-model="networkData.inboundErrors" type="text" placeholder="e.g., 1" />
            </div>
            <div class="form-group">
              <label>Outbound Discards:</label>
              <input v-model="networkData.outboundDiscards" type="text" placeholder="e.g., 167" />
            </div>
            <div class="form-group">
              <label>Deferred Transmissions:</label>
              <input v-model="networkData.deferredTransmissions" type="text" placeholder="e.g., 167" />
            </div>
            <div class="form-group">
              <label>FCS Errors:</label>
              <input v-model="networkData.fcsErrors" type="text" placeholder="e.g., 239113" />
            </div>
          </div>
          <div class="button-group">
            <button @click="prefillData" class="prefill-btn">Prefill Test Data</button>
            <button @click="startChat" class="start-chat-btn">Start Chat Session</button>
          </div>
        </div>

        <div class="messages-container" v-else ref="messagesContainer">
          <div v-for="(msg, index) in messages" :key="index" :class="['message', msg.role]">
            <div class="message-content">{{ msg.content }}</div>
          </div>
          <div v-if="loading" class="message assistant">
            <div class="message-content">Thinking...</div>
          </div>
        </div>
      </div>

      <div class="chat-input-container" v-if="!showForm">
        <input
          v-model="currentMessage"
          @keyup.enter="sendMessage"
          placeholder="Type your message..."
          class="chat-input"
          :disabled="loading"
        />
        <button @click="sendMessage" :disabled="loading || !currentMessage.trim()" class="send-btn">
          Send
        </button>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, watch, nextTick } from 'vue'

const props = defineProps({
  isOpen: Boolean
})

const emit = defineEmits(['close'])

const showForm = ref(true)
const loading = ref(false)
const currentMessage = ref('')
const messagesContainer = ref(null)
const conversationId = ref('')

const networkData = ref({
  brand: '',
  model: '',
  osVersion: '',
  interfaceNumber: '',
  interfaceType: '',
  speed: '',
  txUtilization: '',
  rxUtilization: '',
  inboundErrors: '',
  outboundDiscards: '',
  deferredTransmissions: '',
  fcsErrors: ''
})

const messages = ref([])

const closeChat = () => {
  emit('close')
}

const prefillData = () => {
  networkData.value = {
    brand: 'Cisco',
    model: 'WS-C3650-24PS-E',
    osVersion: 'Denali 16.3.5b',
    interfaceNumber: '5',
    interfaceType: 'ethernetCsmacd',
    speed: '1000000000',
    txUtilization: '1.32',
    rxUtilization: '5.32',
    inboundErrors: '1',
    outboundDiscards: '167',
    deferredTransmissions: '167',
    fcsErrors: '239113'
  }
}

const scrollToBottom = async () => {
  await nextTick()
  if (messagesContainer.value) {
    messagesContainer.value.scrollTop = messagesContainer.value.scrollHeight
  }
}

const buildSystemMessage = () => {
  return 'You are a network administrator'
}

const buildUserMessage = (userQuestion) => {
  const { brand, model, osVersion, interfaceNumber, interfaceType, speed, 
          txUtilization, rxUtilization, inboundErrors, outboundDiscards, 
          deferredTransmissions, fcsErrors } = networkData.value
  
  let prompt = `On a ${brand} model ${model} device, it is running OS ${osVersion}. `
  prompt += `Interface #${interfaceNumber} is a ${interfaceType} type of interface `
  prompt += `and is running at ${speed} bits per second. `
  prompt += `It's utilization is ${txUtilization}% transmit and ${rxUtilization}% receive. `
  prompt += `It is showing ${inboundErrors} Inbound Error, ${outboundDiscards} Outbound Discards, `
  prompt += `${deferredTransmissions} Deferred Transmissions, and ${fcsErrors} FCS Errors.`
  
  if (userQuestion) {
    prompt += `\n\n${userQuestion}`
  } else {
    prompt += `\n\nWhat is causing these errors, and what is the configuration that will fix this?`
  }
  
  return prompt
}

const startChat = async () => {
  // Validate required fields
  const required = ['brand', 'model', 'osVersion', 'interfaceNumber', 'interfaceType', 
                    'speed', 'txUtilization', 'rxUtilization']
  const missing = required.filter(field => !networkData.value[field])
  
  if (missing.length > 0) {
    alert(`Please fill in all required fields: ${missing.join(', ')}`)
    return
  }

  showForm.value = false
  conversationId.value = `conv_${Date.now()}`
  
  // Add initial system and user messages
  const systemMsg = buildSystemMessage()
  const userMsg = buildUserMessage()
  
  messages.value = [
    { role: 'assistant', content: 'Hello! I\'m your network administrator assistant. I see you have some network interface errors. Let me analyze the information...' }
  ]

  // Send initial message
  await sendChatRequest(userMsg, systemMsg)
}

const sendMessage = async () => {
  if (!currentMessage.value.trim() || loading.value) return

  const userMsg = currentMessage.value.trim()
  messages.value.push({ role: 'user', content: userMsg })
  currentMessage.value = ''
  await scrollToBottom()

  await sendChatRequest(userMsg, buildSystemMessage())
}

const sendChatRequest = async (userMessage, systemMessage) => {
  loading.value = true

  try {
    // Build context with previous messages (excluding system message)
    const contextMessages = messages.value
      .filter(msg => msg.role !== 'system')
      .map(msg => ({
        role: msg.role,
        content: msg.content
      }))

    const response = await fetch('http://localhost:8080/chat', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({
        user_message: userMessage,
        system_message: systemMessage,
        context: {
          messages: contextMessages
        },
        conversation_id: conversationId.value
      })
    })

    if (!response.ok) {
      throw new Error(`HTTP error! status: ${response.status}`)
    }

    const data = await response.json()
    
    if (data.error) {
      messages.value.push({ 
        role: 'assistant', 
        content: `Error: ${data.error}` 
      })
    } else {
      messages.value.push({ 
        role: 'assistant', 
        content: data.assistant_message 
      })
    }
  } catch (error) {
    console.error('Error sending chat request:', error)
    messages.value.push({ 
      role: 'assistant', 
      content: `Sorry, I encountered an error: ${error.message}. Please make sure the backend server is running.` 
    })
  } finally {
    loading.value = false
    await scrollToBottom()
  }
}

watch(() => props.isOpen, (newVal) => {
  if (newVal) {
    // Reset form when opening
    showForm.value = true
    messages.value = []
    currentMessage.value = ''
    conversationId.value = ''
  }
})
</script>

<style scoped>
.chat-overlay {
  position: fixed;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background: rgba(0, 0, 0, 0.5);
  display: flex;
  justify-content: center;
  align-items: center;
  z-index: 1000;
}

.chat-window {
  background: white;
  border-radius: 8px;
  width: 90%;
  max-width: 800px;
  max-height: 90vh;
  display: flex;
  flex-direction: column;
  box-shadow: 0 4px 20px rgba(0, 0, 0, 0.15);
}

.chat-header {
  padding: 16px 20px;
  border-bottom: 1px solid #e0e0e0;
  display: flex;
  justify-content: space-between;
  align-items: center;
  background: #f8f9fa;
  border-radius: 8px 8px 0 0;
}

.chat-header h3 {
  margin: 0;
  font-size: 18px;
  color: #333;
}

.close-btn {
  background: none;
  border: none;
  font-size: 28px;
  cursor: pointer;
  color: #666;
  line-height: 1;
  padding: 0;
  width: 30px;
  height: 30px;
  display: flex;
  align-items: center;
  justify-content: center;
}

.close-btn:hover {
  color: #333;
}

.chat-content {
  flex: 1;
  overflow-y: auto;
  padding: 20px;
}

.network-data-form h4 {
  margin-top: 0;
  margin-bottom: 20px;
  color: #333;
}

.form-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
  gap: 16px;
  margin-bottom: 20px;
}

.form-group {
  display: flex;
  flex-direction: column;
}

.form-group label {
  margin-bottom: 6px;
  font-weight: 500;
  color: #555;
  font-size: 14px;
}

.form-group input {
  padding: 8px 12px;
  border: 1px solid #ddd;
  border-radius: 4px;
  font-size: 14px;
}

.form-group input:focus {
  outline: none;
  border-color: #4a90e2;
}

.button-group {
  display: flex;
  gap: 12px;
  margin-top: 10px;
}

.prefill-btn {
  flex: 1;
  padding: 12px;
  background: #f8f9fa;
  color: #333;
  border: 1px solid #ddd;
  border-radius: 4px;
  font-size: 16px;
  font-weight: 500;
  cursor: pointer;
  transition: all 0.2s;
}

.prefill-btn:hover {
  background: #e9ecef;
  border-color: #adb5bd;
}

.start-chat-btn {
  flex: 2;
  padding: 12px;
  background: #4a90e2;
  color: white;
  border: none;
  border-radius: 4px;
  font-size: 16px;
  font-weight: 500;
  cursor: pointer;
}

.start-chat-btn:hover {
  background: #357abd;
}

.messages-container {
  max-height: 500px;
  overflow-y: auto;
  padding: 10px 0;
}

.message {
  margin-bottom: 16px;
  display: flex;
}

.message.user {
  justify-content: flex-end;
}

.message-content {
  max-width: 70%;
  padding: 10px 14px;
  border-radius: 8px;
  white-space: pre-wrap;
  word-wrap: break-word;
}

.message.user .message-content {
  background: #4a90e2;
  color: white;
}

.message.assistant .message-content {
  background: #f0f0f0;
  color: #333;
}

.chat-input-container {
  padding: 16px 20px;
  border-top: 1px solid #e0e0e0;
  display: flex;
  gap: 10px;
  background: #f8f9fa;
  border-radius: 0 0 8px 8px;
}

.chat-input {
  flex: 1;
  padding: 10px 14px;
  border: 1px solid #ddd;
  border-radius: 4px;
  font-size: 14px;
}

.chat-input:focus {
  outline: none;
  border-color: #4a90e2;
}

.chat-input:disabled {
  background: #f5f5f5;
  cursor: not-allowed;
}

.send-btn {
  padding: 10px 20px;
  background: #4a90e2;
  color: white;
  border: none;
  border-radius: 4px;
  font-size: 14px;
  font-weight: 500;
  cursor: pointer;
}

.send-btn:hover:not(:disabled) {
  background: #357abd;
}

.send-btn:disabled {
  background: #ccc;
  cursor: not-allowed;
}
</style>

