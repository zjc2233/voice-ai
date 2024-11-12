<template>
  <div class="home">
    <div class="header">语音AI工具</div>
    <div class="content">
      <el-row :gutter="20" class="w-full h-full flex justify-center">
        <el-col :xs="11" :sm="10" :md="9" :lg="8" :xl="7">
          <InputAudio @sendText="sendText" />
        </el-col>
        <el-col :xs="11" :sm="10" :md="9" :lg="8" :xl="7">
          <InputAI ref="inputAI" />
        </el-col>
      </el-row>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref } from 'vue'
import InputAI from './components/inputAI.vue'
import InputAudio from './components/inputAudio.vue'

const inputAI = ref<InstanceType<typeof InputAI>>()

const sendText = (text: string, immediately: boolean = false) => {
  inputAI.value?.insertText(text, immediately)
}
</script>

<style scoped lang="scss">
.home {
  width: 100vw;
  height: 100vh;
  min-width: 70cap;
  min-height: 400px;
  position: relative;
  .header {
    display: flex;
    justify-content: center;
    align-items: center;
    font-size: 32px;
    font-family: Roboto;
    font-weight: 700;
    line-height: 40px;
    color: #f3f4f6;
    height: 100px;
    width: 100%;
  }
  .content {
    height: calc(100vh - 140px);
    width: 100%;
    position: relative;
  }
}
</style>
