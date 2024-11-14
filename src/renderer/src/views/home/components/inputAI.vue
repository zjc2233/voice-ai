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
      placeholder="请输入您要交互的文本"
      @keyup.enter.native="submitAi()"
      clearable
    ></el-input>
    <div>
      <el-button type="success" @click="submitAi()">提交给AI</el-button>
      <el-button type="info" @click="clearResult()">清空结果</el-button>
    </div>
    <div class="airesponse">
      <div class="microphonetranscript" v-for="(item, index) in aiResponseList" :key="index">
        <div class="text_6">{{ item }}</div>
      </div>
    </div>
  </div>
</template>
<script setup lang="ts">
import { ref, defineExpose, reactive } from 'vue'

const inputText = ref('')
const aiResponseList = reactive([])
//插入文本
const insertText = (text, immediately = false) => {
  if (immediately) {
    inputText.value = text
    submitAi()
  } else {
    inputText.value = inputText.value + text
  }
}

const deepseeekConfig = {
  messages: [
    {
      content: ``,
      role: 'user'
    }
  ],
  model: 'deepseek-chat',
  frequency_penalty: 0,
  max_tokens: 2048,
  presence_penalty: 0,
  response_format: {
    type: 'text'
  },
  stop: null,
  stream: true,
  stream_options: null,
  temperature: 1,
  top_p: 1,
  tools: null,
  tool_choice: 'none',
  logprobs: false,
  top_logprobs: null
}

// 创建 AbortController 实例
let controller, signal

const submitAi = () => {
  controller = new AbortController()
  signal = controller.signal
  deepseeekConfig.messages[0].content = inputText.value
  const body = JSON.stringify(deepseeekConfig)
  fetch(`http://localhost:5173/deepseekApi/chat/completions`, {
    method: 'post',
    headers: {
      'Content-Type': 'application/json',
      Authorization: `Bearer ${import.meta.env.RENDERER_VITE_DEEPSEEK_APIKEY}` // 自定义请求头
    },
    body,
    signal,
    responseType: 'stream' // 响应类型为流
  }).then((response) => {
    const reader = response.body.getReader()
    const decoder = new TextDecoder('utf-8')
    aiResponseList.push('')
    let received_text = ''

    function read() {
      reader.read().then(({ done, value }) => {
        if (done) {
          console.log('Stream ended')
          return
        }

        // 拿到的value就是后端分段返回的数据，大多是以data:开头的字符串
        // 需要通过decode方法处理数据块，例如转换为文本或进行其他操作
        // eslint-disable-next-line @typescript-eslint/no-unused-vars
        const chunkText = decoder
          .decode(value)
          .split('\n')
          .forEach((val) => {
            if (!val) return
            console.log('DONE', val)
            if (val === 'data: [DONE]') {
              return
            }
            try {
              // 后端返回的流式数据一般都是以data:开头的字符，排除掉data:后就是需要的数据
              // 具体返回结构可以跟后端约定
              const text = val?.replace('data:', '') || ''
              console.log(val, text, '输出分段返回的数据')
              const textObj = JSON.parse(text)
              if (textObj.choices[0].delta.content) {
                received_text += textObj.choices[0].delta.content
                aiResponseList[aiResponseList.length - 1] = received_text
              }
              console.log('received_text', received_text)
            } catch (err) {
              console.log(err)
            }
          })
        read()
      })
    }

    read()
  })
}

const clearResult = () => {
  controller.abort()
  aiResponseList.length = 0
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
  .airesponse {
    width: 100%;
    max-height: calc(100vh - 400px);
    position: relative;
    overflow-y: scroll;
    padding: 20px 0;
    .microphonetranscript {
      width: 100%;
      position: relative;
      top: 0px;
      left: 0px;
      gap: 10px;
      flex-shrink: 0;
      overflow: hidden;
      border-radius: 8px;
      background-color: #374151;
      right: 24px;
      margin-bottom: 12px;
      /* P */
      .text_6 {
        width: auto;
        min-height: 32px;
        top: 16px;
        left: 16px;
        padding: 12px;
        gap: 10px;
        border-radius: 4px;
        right: 16px;
        font-size: 14px;
        font-family: Roboto;
        font-weight: 400;
        line-height: 24px;
        color: #d1d5db;
      }
    }
  }
}
</style>
