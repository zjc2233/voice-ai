<template>
  <div class="input_ai">
    <span>
      AI交互
      <span> (输入给AI) </span></span
    >
    <el-input
      type="textarea"
      :rows="5"
      v-model="inputText"
      placeholder="分析以上两段文字的主要内容。"
      clearable
    ></el-input>
    <el-button type="success">提交给AI</el-button>
    <div class="airesponse"></div>
  </div>
</template>
<script setup lang="ts">
import { ref, defineExpose } from 'vue'
const inputText = ref('')
//插入文本
const insertText = (text) => {
  inputText.value = inputText.value + text
}

defineExpose({
  insertText
})
</script>
<style scoped lang="scss">
.input_ai {
  max-width: 400px;
  min-width: 300px;
  height: auto;
  height: 100%;
  left: auto;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  align-items: flex-start;
  padding-top: 24px;
  padding-right: 24px;
  padding-bottom: 24px;
  padding-left: 24px;
  gap: 10px;
  flex-shrink: 0;
  overflow: hidden;
  border-radius: 8px;
  background: linear-gradient(0, rgba(0, 0, 0, 0), rgba(0, 0, 0, 0)),
    linear-gradient(0, #1f2937, #1f2937);
  box-shadow:
    0px 4px 6px -1px rgba(0, 0, 0, 0.1),
    0px 2px 4px -2px rgba(0, 0, 0, 0.1);
}
</style>
