<template>
  <div class="input-audio">
    <span>输入方式 </span>
    <el-select v-model="equipmentType" placeholder="Select" size="large">
      <el-option v-for="item in equipmentTypeList" :key="item" :label="item" :value="item" />
    </el-select>
    <span>选择{{ equipmentType }} </span>
    <el-select
      v-if="equipmentType === '麦克风'"
      v-model="microphoneValue"
      placeholder="Select"
      size="large"
    >
      <el-option
        v-for="item in microphoneList"
        :key="item.deviceId"
        :label="item.label"
        :value="item.deviceId"
      />
    </el-select>
    <el-select
      v-else-if="equipmentType === '扬声器'"
      v-model="speakerValue"
      placeholder="Select"
      size="large"
    >
      <el-option
        v-for="item in speakerList"
        :key="item.deviceId"
        :label="item.label"
        :value="item.deviceId"
      />
    </el-select>
    <div>
      <el-button type="primary" @click="startIntercom()">开始采集</el-button>
      <el-button type="danger" @click="websocketSendStop(true)">暂停</el-button>
      <el-button type="success" @click="isStop = false">继续</el-button>
      <el-button type="danger" @click="stopRecord()">停止录音</el-button>
      <el-button type="info" @click="clearResult()">清空结果</el-button>
      <el-button type="info" @click="down()">下载空语音数据</el-button>
      <el-button type="success" v-if="audioContext">ok</el-button>
    </div>
    <div class="audio2textResult_box">
      <div class="microphonetranscript" v-for="(item, index) in audio2textResult" :key="index">
        <div class="text_6">{{ item }}</div>
        <el-icon class="edit-box" :size="16" :color="'#409eff'">
          <Edit @click="sendText(item)" />
          <ChatDotSquare @click="sendText(item, true)" />
        </el-icon>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, defineEmits } from 'vue'
import { generate32BitRandomCode } from '../../../utils/common'

const emits = defineEmits(['sendText'])

const ali_token = import.meta.env.RENDERER_VITE_ALI_TOKEN
const ali_appkey = import.meta.env.RENDERER_VITE_ALI_APPKEY
const microphoneList = ref([]) // 录音设备列表
const speakerList = ref([]) // 扬声器设备列表
const microphoneValue = ref('default')
const speakerValue = ref('default')
const audioContext = ref(null) // 录音上下文
let ws = null
let websocket_task_id = ''
let message_id = ''
const audio2textResult = ref(['vue2和vue3的区别'])
const equipmentType = ref('麦克风')
const equipmentTypeList = ['麦克风', '扬声器']
let isStop = false

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

function generateBlankWavFile(sampleRate, durationInSeconds) {
  const numChannels = 1 // 单声道
  const bitsPerSample = 16 // 16 bit 采样位数
  const bytesPerSample = bitsPerSample / 8
  const blockAlign = numChannels * bytesPerSample
  const byteRate = sampleRate * blockAlign
  const numSamples = sampleRate * durationInSeconds
  const dataSize = numSamples * blockAlign
  const fileSize = 44 + dataSize // WAV 文件头大小为 44 字节

  const buffer = new ArrayBuffer(fileSize)
  const view = new DataView(buffer)

  // 写入 WAV 文件头
  writeString(view, 0, 'RIFF') // ChunkID
  view.setUint32(4, fileSize - 8, true) // ChunkSize
  writeString(view, 8, 'WAVE') // Format
  writeString(view, 12, 'fmt ') // Subchunk1ID
  view.setUint32(16, 16, true) // Subchunk1Size
  view.setUint16(20, 1, true) // AudioFormat (PCM)
  view.setUint16(22, numChannels, true) // NumChannels
  view.setUint32(24, sampleRate, true) // SampleRate
  view.setUint32(28, byteRate, true) // ByteRate
  view.setUint16(32, blockAlign, true) // BlockAlign
  view.setUint16(34, bitsPerSample, true) // BitsPerSample
  writeString(view, 36, 'data') // Subchunk2ID
  view.setUint32(40, dataSize, true) // Subchunk2Size

  // 写入空白音频数据
  for (let i = 0; i < numSamples; i++) {
    const offset = 44 + i * blockAlign
    view.setInt16(offset, 0, true) // 写入 16 位 PCM 数据，值为 0
  }

  return buffer
}

function writeString(view, offset, string) {
  for (let i = 0; i < string.length; i++) {
    view.setUint8(offset + i, string.charCodeAt(i))
  }
}

// 生成一个 1 秒的空白 WAV 文件，采样率为 16000 Hz
const blankWavFile = generateBlankWavFile(16000, 1)

const down = () => {
  // 将生成的音频数据转换为 Blob 对象
  const blob = new Blob([blankWavFile], { type: 'audio/wav' })

  // 创建下载链接
  const url = URL.createObjectURL(blob)
  const a = document.createElement('a')
  a.href = url
  a.download = 'blank.wav'
  a.style.display = 'none'
  document.body.appendChild(a)

  // 触发下载
  a.click()

  // 清理
  URL.revokeObjectURL(url)
  document.body.removeChild(a)
}

// 开始采集
const startIntercom = () => {
  if (audioContext.value) {
    return
  }
  initWebSocket()
}

// 开始录音
const startRecord = () => {
  const equipmentTypeValue = {
    麦克风: microphoneValue.value,
    扬声器: speakerValue.value
  }
  const constraints = {
    audio: {
      deviceId: {
        exact: equipmentTypeValue[equipmentType.value]
      }
    }
  }
  navigator.mediaDevices
    .getUserMedia(constraints)
    .then((stream) => {
      audioContext.value = new AudioContext()
      const source = audioContext.value.createMediaStreamSource(stream)
      const processor = audioContext.value.createScriptProcessor(4096, 1, 1)
      processor.onaudioprocess = (event) => {
        // console.log('event', event)

        const inputBuffer = event.inputBuffer.getChannelData(0)
        const downsampledBuffer = downsampleBuffer(inputBuffer, 48000, 16000)
        const pcmData = float32ToPCM(downsampledBuffer)
        // console.log('pcmData', pcmData)

        if (isStop) {
          websocketSend(blankWavFile)
        } else {
          websocketSend(pcmData)
        }
        // console.log('Sent downsampled PCM data:', pcmData.byteLength)

        // websocketSendStop() //发送结束
      }
      source.connect(processor)
      processor.connect(audioContext.value.destination)
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
  audioContext.value.close()
  audioContext.value = null
}
const initWebSocket = () => {
  //关闭websocket
  if (ws != null) {
    ws.close()
    ws == null
  }
  const wsUrl = `wss://nls-gateway-cn-shanghai.aliyuncs.com/ws/v1?token=${ali_token}`
  //连接服务端
  ws = new WebSocket(wsUrl)
  //指定事件回调
  ws.onmessage = websocketOnMessage
  ws.onopen = websocketOnOpen
  ws.onerror = websocketOnError
  ws.onclose = websocketClose
}

const websocketOnOpen = () => {
  isStop = false
  //连接建立之后执行send方法发送数据
  console.log('向 websocket 发送 链接请求')
  websocket_task_id = generate32BitRandomCode() //生成新的任务id
  message_id = message_id || generate32BitRandomCode()
  //actions 是首次连接需要的参数,可自行看阿里云文档
  const actions = {
    header: {
      message_id: message_id,
      task_id: websocket_task_id,
      namespace: 'SpeechTranscriber',
      name: 'StartTranscription',
      appkey: ali_appkey
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
const websocketSendStop = (suspend) => {
  isStop = true
  if (suspend) {
    //暂停
    console.log('暂停')
    return
  }
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
      appkey: ali_appkey
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

const sendText = (text, immediately = false) => {
  //插入文本
  console.log('插入文本', text)
  emits('sendText', text, immediately)
}

const clearResult = () => {
  //清空结果
  audio2textResult.value = []
}
</script>

<style lang="scss" scoped>
.input-audio {
  max-width: 600px;
  min-width: 300px;
  height: 100%;
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
  max-height: calc(100vh - 400px);
  position: relative;
  overflow-y: scroll;
  padding: 20px 0;
}

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
  padding-right: 22px;
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
</style>
