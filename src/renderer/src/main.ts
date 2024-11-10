import './assets/main.css'
import ElementPlus from 'element-plus'
import { createPinia } from 'pinia'
// import 'element-plus/dist/index.css'
import 'element-plus/theme-chalk/dark/css-vars.css'
import * as ElementPlusIconsVue from '@element-plus/icons-vue'
import 'virtual:uno.css'
import './style/index.scss'

import { createApp } from 'vue'
import App from './App.vue'

const app = createApp(App)
const pinia = createPinia()

for (const [key, component] of Object.entries(ElementPlusIconsVue)) {
  app.component(key, component)
}
app.use(pinia)
app.use(ElementPlus)
app.mount('#app')
