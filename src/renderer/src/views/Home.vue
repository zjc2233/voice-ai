<script setup lang="ts">
import { ref } from 'vue'
import { generate32BitRandomCode } from '../utils/common'

const zjc_token = 'b51b3f6453274e7b8709e0c10f41a335'
const zjc_appkey = 'Z7zm3rL6QnmMbRb0'
const microphoneList = ref([]) // 录音设备列表
const speakerList = ref([]) // 扬声器设备列表
const microphoneValue = ref('default')
const speakerValue = ref('default')
let audioContext = null
let ws = null
let websocket_task_id = ''
let message_id = ''
const audio2textResult = ref([
  '你好，你能听到我说话吗？我感觉你也是可以听到的，但是你这边都有什么意思呢？你能说什么呢？'
])
const inputText = ref('')

// 获取设备信息
const getMicro = () => {
  navigator.mediaDevices
    .enumerateDevices()
    .then((devices) => {
      const microphoneListArr = [],
        speakerListArr = []
      console.log('devices---------------------------', devices)
      devices.forEach((device) => {
        if (device.kind == 'audioinput') {
          microphoneListArr.push(device)
        } else if (device.kind == 'audiooutput') {
          speakerListArr.push(device)
        }
      })
      microphoneList.value = microphoneListArr
      speakerList.value = speakerListArr
    })
    .catch((err) => {
      console.error(`错误发生 ${err.name}: ${err.message}`)
    })
}
getMicro()

// 开始采集
const startIntercom = () => {
  initWebSocket()
}

// 开始录音
const startRecord = () => {
  const constraints = {
    audio: {
      deviceId: {
        exact: microphoneValue.value
      }
    }
  }
  navigator.mediaDevices
    .getUserMedia(constraints)
    .then((stream) => {
      audioContext = new AudioContext()
      const source = audioContext.createMediaStreamSource(stream)
      const processor = audioContext.createScriptProcessor(4096, 1, 1)
      processor.onaudioprocess = (event) => {
        console.log('event', event)

        const inputBuffer = event.inputBuffer.getChannelData(0)
        const downsampledBuffer = downsampleBuffer(inputBuffer, 48000, 16000)
        const pcmData = float32ToPCM(downsampledBuffer)
        console.log('pcmData', pcmData)

        websocketSend(pcmData)
        console.log('Sent downsampled PCM data:', pcmData.byteLength)

        // websocketSendStop() //发送结束
      }
      source.connect(processor)
      processor.connect(audioContext.destination)
    })
    .catch((err) => {
      console.error(`错误发生 ${err.name}: ${err.message}`)
    })
}

const float32ToPCM = (input) => {
  const buffer = new ArrayBuffer(input.length * 2)
  const output = new DataView(buffer)

  for (let i = 0; i < input.length; i++) {
    const s = Math.max(-1, Math.min(1, input[i]))
    output.setInt16(i * 2, s < 0 ? s * 0x8000 : s * 0x7fff, true) // little-endian
  }
  return buffer
}

const downsampleBuffer = (buffer, inputSampleRate, outputSampleRate) => {
  if (inputSampleRate === outputSampleRate) {
    return buffer
  }
  const sampleRateRatio = inputSampleRate / outputSampleRate
  const newLength = Math.round(buffer.length / sampleRateRatio)
  const result = new Float32Array(newLength)
  let offsetResult = 0
  let offsetBuffer = 0

  while (offsetResult < result.length) {
    const nextOffsetBuffer = Math.round((offsetResult + 1) * sampleRateRatio)
    let accum = 0,
      count = 0
    for (let i = offsetBuffer; i < nextOffsetBuffer && i < buffer.length; i++) {
      accum += buffer[i]
      count++
    }
    result[offsetResult] = accum / count
    offsetResult++
    offsetBuffer = nextOffsetBuffer
  }
  return result
}

// 停止录音
const stopRecord = () => {
  audioContext.close()
}
const initWebSocket = () => {
  //关闭websocket
  if (ws != null) {
    ws.close()
    ws == null
  }
  const wsUrl = `wss://nls-gateway-cn-shanghai.aliyuncs.com/ws/v1?token=${zjc_token}`
  //连接服务端
  ws = new WebSocket(wsUrl)
  //指定事件回调
  ws.onmessage = websocketOnMessage
  ws.onopen = websocketOnOpen
  ws.onerror = websocketOnError
  ws.onclose = websocketClose
  // 每隔 30 秒发送心跳包保持连接
  const timer = setInterval(() => {
    if (ws && ws.readyState === ws.OPEN) {
      ws.send('ping')
    } else {
      clearInterval(timer)
    }
  }, 30000)
}

const websocketOnOpen = () => {
  //连接建立之后执行send方法发送数据
  console.log('向 websocket 发送 链接请求')
  websocket_task_id = generate32BitRandomCode() //生成新的任务id
  message_id = generate32BitRandomCode()
  //actions 是首次连接需要的参数,可自行看阿里云文档
  const actions = {
    header: {
      message_id: message_id,
      task_id: websocket_task_id,
      namespace: 'SpeechTranscriber',
      name: 'StartTranscription',
      appkey: zjc_appkey
    },
    payload: {
      format: 'PCM', //音频编码格式，默认是PCM（无压缩的PCM文件或WAV文件），16bit采样位数的单声道。

      sample_rate: 16000,
      enable_intermediate_result: true, //是否返回中间识别结果，默认是false。
      enable_punctuation_prediction: true, //是否在后处理中添加标点，默认是false。
      enable_inverse_text_normalization: true,
      enable_ignore_sentence_timeout: true, //是否忽略实时识别中的单句识别超时，默认是false。
      enable_words: true, //是否开启返回词信息，默认是false。
      max_sentence_silence: 2000 //	语音断句检测阈值，静音时长超过该阈值会被认为断句，参数范围200ms～2000ms，默认值800ms。
    }
  }
  websocketSend(JSON.stringify(actions))
}
const websocketSendStop = () => {
  //连接建立之后发送 StopTranscription指令
  console.log('向  websocket 发送 Stop指令')
  message_id = generate32BitRandomCode()
  //actions 是首次连接需要的参数,可自行看阿里云文档
  const actions = {
    header: {
      message_id: message_id,
      task_id: websocket_task_id,
      namespace: 'SpeechTranscriber',
      name: 'StopTranscription',
      appkey: zjc_appkey
    }
  }
  websocketSend(JSON.stringify(actions))
}

const websocketOnError = () => {
  //连接建立失败重连
  console.log('连接建立失败重连')
}

const websocketOnMessage = (e) => {
  //数据接收
  const redata = JSON.parse(e.data)
  if (redata.header.name === 'TranscriptionResultChanged') {
    console.log('数据接收-返回的数据', redata.payload)

    //websocket 语音转文本 一句话是否收集完毕  true:完毕
    websocket_audio2txt_complete_b = false //数据在收集中
  } else if (redata.header.name === 'SentenceBegin') {
    //一句话开始后，就可以启动录音了
  } else if (redata.header.name === 'TranscriptionStarted') {
    console.log('数据接收,开始：', redata)
    startRecord()
  } else if (redata.header.name === 'SentenceEnd') {
    //结束
    console.log('数据接收-一句话结束---返回的数据', redata.payload)
    audio2textResult.value.push(redata.payload.result) //将结果添加到数组中
  }
}

const websocketSend = (Data) => {
  //数据发送 判断是否连接成功,连接成功再发送数据过去
  if (ws.readyState === 1) {
    ws.send(Data)
  } else {
    console.log('websock未连接-------------------')
  }
}

// eslint-disable-next-line
const websocketClose = (e) => {
  //关闭
  console.log('websocketClose断开连接')
}

//插入文本
const insertText = (text) => {
  inputText.value = inputText.value + text
}
</script>

<template>
  <div class="home">
    <div class="con">
      <div class="center">
        <div class="section">
          <span class="section_title"
            >麦克风输入
            <span class="section_label"> (选择麦克风) </span>
          </span>
          <el-select v-model="microphoneValue" placeholder="Select" size="large">
            <el-option
              v-for="item in microphoneList"
              :key="item.deviceId"
              :label="item.label"
              :value="item.deviceId"
            />
          </el-select>
          <div>
            <el-button type="primary" @click="startIntercom()">开始采集</el-button>
            <el-button type="danger" @click="stopRecord()">停止录音</el-button>
          </div>
          <div class="audio2textResult_box">
            <div
              class="microphonetranscript"
              v-for="(item, index) in audio2textResult"
              :key="index"
            >
              <div class="text_6">{{ item }}</div>
              <el-icon class="edit-box" :size="16" :color="'#409eff'" @click="insertText(item)">
                <Edit />
              </el-icon>
            </div>
          </div>
        </div>
        <div class="section">
          <span class="section_title">
            扬声器输入
            <span class="section_label"> (选择扬声器) </span>
          </span>
          <el-select v-model="speakerValue" placeholder="Select" size="large">
            <el-option
              v-for="item in speakerList"
              :key="item.deviceId"
              :label="item.label"
              :value="item.deviceId"
            />
          </el-select>
          <div>
            <el-button type="success" @click="startIntercom()">开始播放</el-button>
            <el-button type="warning" @click="stopRecord()">停止播放</el-button>
          </div>
          <div class="speakertranscript">
            <span class="text_17"> 这是一段来自科技播客的音频。 </span>
          </div>
        </div>
        <div class="div_8">
          <span class="section_title">
            AI交互
            <span class="section_label"> (输入给AI) </span></span
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
      </div>
    </div>
  </div>
</template>

<style lang="scss">
.no-shrink {
  flex-shrink: 0;
}
</style>
<style scoped lang="scss">
.home {
  width: 100%;
  height: auto;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  align-items: flex-start;
  padding-top: 0px;
  padding-right: 80px;
  padding-bottom: 0px;
  padding-left: 80px;
  overflow: hidden;
  background-color: #111827;
  position: relative;
  min-height: 100%;
}

.con {
  width: 1280px;
  height: 637px;
  position: relative;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  align-items: flex-start;
  padding-top: 32px;
  padding-right: 16px;
  padding-bottom: 32px;
  padding-left: 16px;
  gap: 10px;
  flex-shrink: 0;
}

.center {
  width: auto;
  height: auto;
  top: 32px;
  left: 16px;
  display: flex;
  flex-direction: row;
  justify-content: flex-start;
  align-items: flex-start;
  justify-content: space-between;
  gap: 24px;
  flex-shrink: 0;
  right: 16px;
  bottom: 32px;
}

.section {
  width: 400px;
  height: auto;
  top: 0;
  position: relative;
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
  bottom: 0;
}

.audio2textResult_box {
  width: 100%;
  height: 100%;
  position: relative;
  max-height: calc(100vh - 460px);
  overflow-y: scroll;
  padding: 20px 0;
}

.microphonetranscript {
  width: 100%;
  height: 100%;
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
  padding-right: 22px;
  /* P */
  .text_6 {
    width: auto;
    min-height: 32px;
    top: 16px;
    left: 16px;
    padding: 4px;
    gap: 10px;
    border-radius: 4px;
    right: 16px;
    font-size: 14px;
    font-family: Roboto;
    font-weight: 400;
    line-height: 24px;
    color: #d1d5db;
  }
  .edit-box {
    display: none;
  }
  &:hover {
    .edit-box {
      display: inline-block;
      position: absolute;
      top: 10px;
      right: 10px;
      font-size: 20px;
      color: #f3f4f6;
      cursor: pointer;
    }
  }
}

/* speakerTranscript */
.speakertranscript {
  width: auto;
  height: 256px;
  position: absolute;
  top: 200px;
  left: 24px;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  align-items: flex-start;
  padding: 16px;
  gap: 10px;
  flex-shrink: 0;
  overflow: hidden;
  border-radius: 8px;
  background-color: #374151;
  right: 24px;
}

/* P */
.text_17 {
  width: auto;
  height: 32px;
  position: absolute;
  top: 16px;
  left: 16px;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  align-items: flex-start;
  padding-top: 4px;
  padding-right: 4px;
  padding-bottom: 4px;
  padding-left: 4px;
  gap: 10px;
  flex-shrink: 0;
  border-radius: 4px;
  right: 16px;
  font-size: 16px;
  font-family: Roboto;
  font-weight: 400;
  line-height: 24px;
  color: #d1d5db;
  white-space: pre;
}

/* DIV */
.div_8 {
  width: 400px;
  height: auto;
  position: absolute;
  top: 0;
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
  right: 0;
  bottom: 0;
}

/* H2 */
.section_title {
  width: 352px;
  display: flex;
  justify-content: start;
  align-items: flex-end;
  font-size: 24px;
  font-family: Roboto;
  font-weight: 700;
  line-height: 32px;
  color: #f3f4f6;
}

.section_label {
  font-size: 14px;
  font-family: Roboto;
  font-weight: 500;
  line-height: 20px;
  color: #d1d5db;
}

/* aiResponse */
.airesponse {
  width: auto;
  height: 256px;
  width: 100%;
  top: auto;
  left: 24px;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  align-items: flex-start;
  padding-top: 16px;
  padding-right: 16px;
  padding-bottom: 16px;
  padding-left: 16px;
  gap: 10px;
  flex-shrink: 0;
  overflow: hidden;
  border-radius: 8px;
  background-color: #374151;
  right: 24px;
  bottom: 24px;
}
</style>
