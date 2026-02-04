import { createRouter, createWebHistory } from 'vue-router'
import { isAuthenticated } from '../auth.js'
import Login from '../views/Login.vue'
import Dashboard from '../views/Dashboard.vue'
import NetworkStatus from '../views/NetworkStatus.vue'
import DeviceDetails from '../views/DeviceDetails.vue'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/login',
      name: 'login',
      component: Login,
      meta: { public: true }
    },
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

router.beforeEach((to) => {
  if (to.meta.public) return true
  if (!isAuthenticated()) {
    return { name: 'login', query: { redirect: to.fullPath } }
  }
  return true
})

export default router
