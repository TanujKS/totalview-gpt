import { createRouter, createWebHistory } from 'vue-router'
import Dashboard from '../views/Dashboard.vue'
import NetworkStatus from '../views/NetworkStatus.vue'
import DeviceDetails from '../views/DeviceDetails.vue'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/',
      name: 'dashboard',
      component: Dashboard
    },
    {
      path: '/network',
      name: 'network-status',
      component: NetworkStatus
    },
    {
      path: '/device',
      name: 'device-details',
      component: DeviceDetails
    }
  ],
})

export default router
