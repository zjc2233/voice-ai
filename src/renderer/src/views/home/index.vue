<template>
  <div class="home">
    <div class="header">语音AI工具</div>
    <div class="content">
      <el-row :gutter="20" class="w-full h-full">
        <el-col :span="7" :offset="5">
          <InputAudio @sendText="sendText" />
        </el-col>
        <el-col :span="7" :offset="0">
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

const sendText = (text: string) => {
  console.log(text)
  inputAI.value?.insertText(text)
}
</script>

<style scoped lang="scss">
.home {
  width: 100vw;
  height: 100vh;
  min-width: 600px;
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
