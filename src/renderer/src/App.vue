<script setup lang="ts">
import Home from './views/Home.vue'

const ipcHandle = () => window.electron.ipcRenderer.send('ping')
</script>

<template>
  <el-button type="primary" @click="ipcHandle">Send IPC</el-button>
  <Home />
</template>
