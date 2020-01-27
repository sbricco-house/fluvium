import Vue from 'vue'
import VueRouter from 'vue-router'
import Home from '../views/Home.vue'
import RiverPage from "@/views/RiverPage.vue"
Vue.use(VueRouter)

const routes = [
  {
    path: '/',
    name: 'home',
    component: Home
  },
  {
    path: "/river/:riverName",
    component : RiverPage,
    props: true
  }
]

const router = new VueRouter({
  routes
})

export default router
