<template>
  <div class="device-details">
    <h2>Device Details</h2>
    
    <div class="section">
      <div class="device-header">
        <h3>Cisco WS-C3650-24PS-E</h3>
        <span class="status-badge warning">Warning</span>
      </div>
      
      <div class="info-grid">
        <div class="info-item">
          <label>Model:</label>
          <span>WS-C3650-24PS-E</span>
        </div>
        <div class="info-item">
          <label>Operating System:</label>
          <span>Denali 16.3.5b</span>
        </div>
        <div class="info-item">
          <label>Serial Number:</label>
          <span>FCW2142L0A1</span>
        </div>
        <div class="info-item">
          <label>Uptime:</label>
          <span>45 days, 12 hours</span>
        </div>
        <div class="info-item">
          <label>IP Address:</label>
          <span>192.168.1.10</span>
        </div>
        <div class="info-item">
          <label>Location:</label>
          <span>Data Center - Rack A12</span>
        </div>
      </div>
    </div>

    <div class="section">
      <h3>Interface #5 Details</h3>
      <div class="interface-details">
        <div class="detail-row">
          <div class="detail-item">
            <label>Interface Type:</label>
            <span>ethernetCsmacd</span>
          </div>
          <div class="detail-item">
            <label>Speed:</label>
            <span>1,000,000,000 bits per second (1 Gbps)</span>
          </div>
        </div>
        <div class="detail-row">
          <div class="detail-item">
            <label>Transmit Utilization:</label>
            <span class="stat">1.32%</span>
          </div>
          <div class="detail-item">
            <label>Receive Utilization:</label>
            <span class="stat">5.32%</span>
          </div>
        </div>
        
        <div class="error-section">
          <h4>Error Statistics</h4>
          <div class="error-grid">
            <div class="error-item">
              <label>Inbound Errors:</label>
              <span class="error-value">1</span>
            </div>
            <div class="error-item">
              <label>Outbound Discards:</label>
              <span class="error-value">167</span>
            </div>
            <div class="error-item">
              <label>Deferred Transmissions:</label>
              <span class="error-value">167</span>
            </div>
            <div class="error-item">
              <label>FCS Errors:</label>
              <span class="error-value high">239,113</span>
            </div>
          </div>
        </div>
      </div>
    </div>

    <div class="section">
      <h3>Configuration</h3>
      <pre class="config-block"><code>interface GigabitEthernet0/5
 description Uplink to Core Switch
 speed 1000
 duplex full
 no shutdown
</code></pre>
    </div>
  </div>
</template>

<script setup>
</script>

<style scoped>
.device-details {
  max-width: 100%;
}

.device-details h2 {
  margin-top: 0;
  margin-bottom: 24px;
  color: #333;
  font-size: 28px;
}

.section {
  background: white;
  border-radius: 8px;
  padding: 24px;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.05);
  border: 1px solid #e0e0e0;
  margin-bottom: 24px;
}

.device-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 24px;
  padding-bottom: 16px;
  border-bottom: 1px solid #e0e0e0;
}

.device-header h3 {
  margin: 0;
  color: #333;
  font-size: 24px;
}

.status-badge {
  display: inline-block;
  padding: 6px 16px;
  border-radius: 12px;
  font-size: 14px;
  font-weight: 500;
}

.status-badge.warning {
  background: #fff3cd;
  color: #856404;
}

.info-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
  gap: 20px;
}

.info-item {
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.info-item label {
  font-size: 12px;
  color: #999;
  text-transform: uppercase;
  letter-spacing: 0.5px;
  font-weight: 600;
}

.info-item span {
  font-size: 16px;
  color: #333;
}

.interface-details {
  display: flex;
  flex-direction: column;
  gap: 24px;
}

.detail-row {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
  gap: 24px;
}

.detail-item {
  display: flex;
  flex-direction: column;
  gap: 8px;
}

.detail-item label {
  font-size: 14px;
  color: #666;
  font-weight: 500;
}

.detail-item span {
  font-size: 16px;
  color: #333;
}

.detail-item .stat {
  font-size: 20px;
  font-weight: 600;
  color: #4a90e2;
}

.error-section {
  margin-top: 16px;
  padding-top: 24px;
  border-top: 2px solid #e0e0e0;
}

.error-section h4 {
  margin-top: 0;
  margin-bottom: 16px;
  color: #333;
  font-size: 18px;
}

.error-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: 16px;
}

.error-item {
  display: flex;
  flex-direction: column;
  gap: 8px;
  padding: 12px;
  background: #f8f9fa;
  border-radius: 4px;
}

.error-item label {
  font-size: 12px;
  color: #666;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.error-value {
  font-size: 24px;
  font-weight: 600;
  color: #dc3545;
}

.error-value.high {
  color: #721c24;
}

.config-block {
  background: #f8f9fa;
  border: 1px solid #e0e0e0;
  border-radius: 4px;
  padding: 16px;
  overflow-x: auto;
  margin: 0;
}

.config-block code {
  font-family: 'Courier New', monospace;
  font-size: 14px;
  color: #333;
  line-height: 1.6;
}
</style>

