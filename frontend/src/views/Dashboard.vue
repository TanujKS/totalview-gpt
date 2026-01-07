<template>
  <div class="dashboard">
    <h2>Network Dashboard</h2>
    <div class="dashboard-grid">
      <div class="card">
        <h3>Total Devices</h3>
        <p class="stat-value">42</p>
        <p class="stat-label">Active network devices</p>
      </div>
      <div class="card">
        <h3>Network Health</h3>
        <p class="stat-value">98.5%</p>
        <p class="stat-label">Overall system uptime</p>
      </div>
      <div class="card">
        <h3>Active Alerts</h3>
        <p class="stat-value">3</p>
        <p class="stat-label">Requires attention</p>
      </div>
      <div class="card">
        <h3>Bandwidth Usage</h3>
        <p class="stat-value">67%</p>
        <p class="stat-label">Average utilization</p>
      </div>
    </div>

    <div class="section">
      <h3>Recent Activity</h3>
      <div class="activity-list">
        <div class="activity-item">
          <span class="activity-text">Device WS-C3650-24PS-E reported interface errors</span>
          <span class="activity-time">2 minutes ago</span>
        </div>
        <div class="activity-item">
          <span class="activity-text">Device ASR-1001-X status updated</span>
          <span class="activity-time">15 minutes ago</span>
        </div>
        <div class="activity-item">
          <span class="activity-text">Bandwidth threshold exceeded on interface Gi0/1</span>
          <span class="activity-time">1 hour ago</span>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
</script>

<style scoped>
.dashboard {
  max-width: 100%;
}

.dashboard h2 {
  margin-top: 0;
  margin-bottom: 24px;
  color: #333;
  font-size: 28px;
}

.dashboard-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
  gap: 20px;
  margin-bottom: 32px;
}

.card {
  background: white;
  border-radius: 8px;
  padding: 24px;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.05);
  border: 1px solid #e0e0e0;
}

.card h3 {
  margin: 0 0 12px 0;
  font-size: 16px;
  font-weight: 500;
  color: #666;
}

.stat-value {
  margin: 0 0 8px 0;
  font-size: 36px;
  font-weight: 600;
  color: #333;
}

.stat-label {
  margin: 0;
  font-size: 14px;
  color: #999;
}

.section {
  background: white;
  border-radius: 8px;
  padding: 24px;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.05);
  border: 1px solid #e0e0e0;
}

.section h3 {
  margin-top: 0;
  margin-bottom: 16px;
  color: #333;
  font-size: 20px;
}

.activity-list {
  display: flex;
  flex-direction: column;
  gap: 12px;
}

.activity-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 14px 16px;
  background: #f8f9fa;
  border-radius: 6px;
  border: 1px solid #e9ecef;
  transition: background-color 0.2s;
}

.activity-item:hover {
  background: #f1f3f5;
}

.activity-text {
  flex: 1;
  color: #333;
  font-size: 14px;
  line-height: 1.5;
}

.activity-time {
  font-size: 12px;
  color: #999;
  white-space: nowrap;
  margin-left: 16px;
  font-weight: 400;
}
</style>

