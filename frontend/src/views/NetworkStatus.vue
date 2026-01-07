<template>
  <div class="network-status">
    <h2>Network Status</h2>
    
    <div class="section">
      <h3>Interface Statistics</h3>
      <div class="table-container">
        <table class="data-table">
          <thead>
            <tr>
              <th>Device</th>
              <th>Interface</th>
              <th>Type</th>
              <th>Status</th>
              <th>Speed</th>
              <th>TX Util</th>
              <th>RX Util</th>
              <th>Errors</th>
            </tr>
          </thead>
          <tbody>
            <tr>
              <td>WS-C3650-24PS-E</td>
              <td>Gi0/5</td>
              <td>ethernetCsmacd</td>
              <td><span class="status-badge warning">Warning</span></td>
              <td>1 Gbps</td>
              <td>1.32%</td>
              <td>5.32%</td>
              <td>239,113</td>
            </tr>
            <tr>
              <td>WS-C3650-24PS-E</td>
              <td>Gi0/3</td>
              <td>ethernetCsmacd</td>
              <td><span class="status-badge ok">OK</span></td>
              <td>1 Gbps</td>
              <td>0.45%</td>
              <td>0.23%</td>
              <td>0</td>
            </tr>
            <tr>
              <td>ASR-1001-X</td>
              <td>Gi0/0/1</td>
              <td>ethernetCsmacd</td>
              <td><span class="status-badge ok">OK</span></td>
              <td>10 Gbps</td>
              <td>12.5%</td>
              <td>8.3%</td>
              <td>0</td>
            </tr>
            <tr>
              <td>N9K-C93180YC-EX</td>
              <td>Eth1/5</td>
              <td>ethernetCsmacd</td>
              <td><span class="status-badge error">Error</span></td>
              <td>10 Gbps</td>
              <td>0.0%</td>
              <td>0.0%</td>
              <td>1,542</td>
            </tr>
          </tbody>
        </table>
      </div>
    </div>

    <div class="section">
      <h3>Device Overview</h3>
      <div class="device-grid">
        <div class="device-card">
          <h4>Cisco WS-C3650-24PS-E</h4>
          <p><strong>OS:</strong> Denali 16.3.5b</p>
          <p><strong>Uptime:</strong> 45 days</p>
          <p><strong>Interfaces:</strong> 24 active</p>
          <p><strong>Status:</strong> <span class="status-badge warning">Warning</span></p>
        </div>
        <div class="device-card">
          <h4>Cisco ASR-1001-X</h4>
          <p><strong>OS:</strong> IOS XE 17.03.04a</p>
          <p><strong>Uptime:</strong> 120 days</p>
          <p><strong>Interfaces:</strong> 8 active</p>
          <p><strong>Status:</strong> <span class="status-badge ok">OK</span></p>
        </div>
        <div class="device-card">
          <h4>Cisco N9K-C93180YC-EX</h4>
          <p><strong>OS:</strong> NX-OS 9.3(5)</p>
          <p><strong>Uptime:</strong> 30 days</p>
          <p><strong>Interfaces:</strong> 48 active</p>
          <p><strong>Status:</strong> <span class="status-badge error">Error</span></p>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
</script>

<style scoped>
.network-status {
  max-width: 100%;
}

.network-status h2 {
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

.section h3 {
  margin-top: 0;
  margin-bottom: 16px;
  color: #333;
  font-size: 20px;
}

.table-container {
  overflow-x: auto;
}

.data-table {
  width: 100%;
  border-collapse: collapse;
}

.data-table th {
  background: #f8f9fa;
  padding: 12px;
  text-align: left;
  font-weight: 600;
  color: #333;
  border-bottom: 2px solid #e0e0e0;
  font-size: 14px;
}

.data-table td {
  padding: 12px;
  border-bottom: 1px solid #e0e0e0;
  color: #666;
  font-size: 14px;
}

.data-table tr:hover {
  background: #f8f9fa;
}

.status-badge {
  display: inline-block;
  padding: 4px 12px;
  border-radius: 12px;
  font-size: 12px;
  font-weight: 500;
}

.status-badge.ok {
  background: #d4edda;
  color: #155724;
}

.status-badge.warning {
  background: #fff3cd;
  color: #856404;
}

.status-badge.error {
  background: #f8d7da;
  color: #721c24;
}

.device-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
  gap: 20px;
}

.device-card {
  background: #f8f9fa;
  border-radius: 6px;
  padding: 20px;
  border: 1px solid #e0e0e0;
}

.device-card h4 {
  margin-top: 0;
  margin-bottom: 12px;
  color: #333;
  font-size: 18px;
}

.device-card p {
  margin: 8px 0;
  color: #666;
  font-size: 14px;
}

.device-card strong {
  color: #333;
}
</style>

