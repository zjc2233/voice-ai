<script setup lang="ts">
import { ref } from 'vue'
import { generate32BitRandomCode } from '../utils/common'

const zjc_token = '954a7a40b3e843b5b48d38eebda49f7f'
const zjc_appkey = 'Z7zm3rL6QnmMbRb0'
const microphoneList = ref([])
const speakerList = ref([])
const microphoneValue = ref('default')
const speakerValue = ref('default')
let audioContext = null
let recorder = null
//初始化weosocket
let ws = null
// eslint-disable-next-line @typescript-eslint/no-unused-vars
let websocket_audio2txt_complete_b = false //websocket 语音转文本 一句话是否收集完毕  true:完毕
// let websocket_audio2txt_time = 0
let websocket_task_id = ''
let message_id = ''
const websocket_audio2txt_result_msg = ref('')

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

// 停止录音后调用此函数，将 audioData 转换为 Blob
// const createAudioBlob = (audioData) => {
//   // 将 Float32Array 转换为 Int16Array 以 PCM 格式保存
//   const pcmData = float32ToPCM(audioData)

//   // 创建音频 Blob
//   const audioBlob = new Blob([pcmData], { type: 'audio/wav' })
//   console.log('audioBlob', audioBlob)

//   // 分割音频 Blob，每块大小为 3200 字节
//   const chunkSize = 3200
//   const audioChunks = []

//   for (let start = 0; start < audioBlob.size; start += chunkSize) {
//     const chunk = audioBlob.slice(start, start + chunkSize)
//     // audioChunks.push(chunk) // 存储每个块
//     console.log('chunk', chunk)
//     const newChunk = new Blob([chunk], { type: 'audio/pcm' })
//     console.log('newChunk', newChunk)
//     websocketSend(newChunk)
//   }

//   return audioChunks // 返回切割后的音频块
// }

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
  // initWebSocket();
}

const websocketOnMessage = (e) => {
  //数据接收
  const redata = JSON.parse(e.data)
  if (redata.header.name === 'TranscriptionResultChanged') {
    console.log(
      '数据接收-返回的数据' + dateFtt(new Date(), 'yyyy-MM-dd hh:mm:ss.S'),
      redata.payload
    )

    //websocket 语音转文本 一句话是否收集完毕  true:完毕
    websocket_audio2txt_complete_b = false //数据在收集中
  } else if (redata.header.name === 'SentenceBegin') {
    //一句话开始后，就可以启动录音了
  } else if (redata.header.name === 'TranscriptionStarted') {
    console.log('数据接收,开始：', redata)

    //获取音频信息，并发送
    // getWavAndSend()
    startRecord()
  } else if (redata.header.name === 'SentenceEnd') {
    //结束
    console.log(
      '数据接收-一句话结束---返回的数据' + dateFtt(new Date(), 'yyyy-MM-dd hh:mm:ss.S'),
      redata.payload
    )

    websocket_audio2txt_result_msg.value =
      websocket_audio2txt_result_msg.value + redata.payload.result

    //websocket 语音转文本 一句话是否收集完毕  true:完毕
    websocket_audio2txt_complete_b = true
    //websocket 语音转文本 一句话收集完毕的时间，用于判断间隔
    // websocket_audio2txt_time = new Date().getTime()

    //设置定时器-websocket
    // setTimer_websocket();
  }
}

// 开始录音
const handleStart = () => {
  console.log('handleStart...')
  //this.initWebSocket(); //初始化weosocket

  websocket_audio2txt_result_msg.value = '' //置空

  //启动录音的控件
  recorder = new Recorder({
    sampleBits: 16, // 采样位数，支持 8 或 16，默认是16
    sampleRate: 16000, // 采样率，支持 11025、16000、22050、24000、44100、48000，根据浏览器默认值，我的chrome是48000
    numChannels: 1 //声道数
    // compiling: true,//(0.x版本中生效,1.x增加中)  // 是否边录边转换，默认是false
  })
  recorder.start()
}

//获取音频信息，并发送
const getWavAndSend = () => {
  // const blob = this.recorder.getWAVBlob();
  const blob = recorder.getPCMBlob()
  const blob_size = blob.size

  console.log(
    '获取音频信息，并发送,blob_size:' +
      blob_size +
      ' ' +
      dateFtt(new Date(), 'yyyy-MM-dd hh:mm:ss.S'),
    blob
  )

  const max_blob_size = 3200 //支持1600 或3200
  let my_num = blob_size / max_blob_size

  my_num = my_num + 1
  for (let i = 0; i < my_num; i++) {
    let end_index_blob = max_blob_size * (i + 1)
    if (end_index_blob > blob_size) {
      end_index_blob = blob_size
    }
    const blob2 = blob.slice(i * max_blob_size, end_index_blob)

    const newbolb = new Blob([blob2], { type: 'audio/pcm' })
    //发送
    websocketSend(newbolb)
  }
  websocketSendStop() //发生结束
}

const websocketSend = (Data) => {
  //数据发送
  // console.log('websocket 数据发送',Data);
  //判断是否连接成功,连接成功再发送数据过去
  if (ws.readyState === 1) {
    ws.send(Data)
  } else {
    console.log('websock未连接-------------------')
  }
}

// eslint-disable-next-line
const websocketClose = (e) => {
  //关闭
  // eslint-disable-next-line
  console.log('websocketClose断开连接')
  // this.initWebSocket();
}
</script>

<template>
  <div class="body_111495">
    <el-button type="danger" @click="getMicro()">按钮</el-button>
    <span class="text_6" @click="handleStart()"> 开始录音 </span>
    <div class="div">
      <div class="div_1">
        <div class="div_2">
          <span class="text"> 麦克风输入 </span>
          <div class="div_3">
            <span class="text_1"> 选择麦克风 </span>
            <el-select v-model="microphoneValue" placeholder="Select" size="large">
              <el-option
                v-for="item in microphoneList"
                :key="item.deviceId"
                :label="item.label"
                :value="item.deviceId"
              />
            </el-select>
          </div>
          <div class="div_4">
            <span class="text_3" @click="startIntercom()"> 开始采集 </span
            ><span class="text_4" @click="stopRecord()"> 停止录音 </span
            ><span class="text_5"> 就绪 </span>
          </div>
          <div class="microphonetranscript">
            <span class="text_6"> 今天天气真好，阳光明媚。 </span
            ><span class="text_7"> 我们打算去公园野餐，享受户外时光。 </span
            ><span class="text_8"> 希望能遇到有趣的人，交到新朋友。 </span
            ><span class="text_9"> 昨天我们去了新开的咖啡馆。 </span
            ><span class="text_10"> 那里的拿铁咖啡非常美味。 </span>
          </div>
        </div>
        <div class="div_5">
          <span class="text_11"> 扬声器输入 </span>
          <div class="div_6">
            <span class="text_12"> 选择扬声器 </span>
            <el-select v-model="speakerValue" placeholder="Select" size="large">
              <el-option
                v-for="item in speakerList"
                :key="item.deviceId"
                :label="item.label"
                :value="item.deviceId"
              />
            </el-select>
          </div>
          <div class="div_7">
            <span class="text_14"> 开始播放 </span><span class="text_15"> 停止播放 </span
            ><span class="text_16"> 就绪 </span>
          </div>
          <div class="speakertranscript">
            <span class="text_17"> 这是一段来自科技播客的音频。 </span
            ><span class="text_18"> 主持人正在讨论最新科技发展趋势。 </span
            ><span class="text_19"> 人工智能和机器学习是当前热门话题。 </span
            ><span class="text_20"> 专家认为这些技术将彻底改变我们的生活。 </span
            ><span class="text_21"> 上周的节目讨论了可再生能源的未来。 </span
            ><span class="text_22"> 太阳能和风能技术正在飞速发展。 </span>
          </div>
        </div>
        <div class="div_8">
          <span class="text_23"> AI交互 </span>
          <div class="div_9">
            <span class="text_24"> 输入给AI </span
            ><span class="text_25"> 分析以上两段文字的主要内容。 </span>
          </div>
          <span class="text_26"> 提交给AI </span>
          <div class="airesponse">
            <span class="text_27"> 两段文字的主要内容如下： </span>
            <div class="ol">
              <div class="li">
                <div class="flexcontainer">
                  <span class="text_28"> 1. </span
                  ><span class="text_29"> 描述了一个阳光明媚的日子，计划去公园 </span>
                </div>
                <span class="text_30">
                  野餐并希望结识新朋友。反映了对户外活动和<br />社交的期待。
                </span>
              </div>
              <div class="li_1">
                <div class="flexcontainer_1">
                  <span class="text_31"> 2. </span
                  ><span class="text_32"> 讨论了一个科技相关的播客内容，主题包 </span>
                </div>
                <span class="text_33">
                  括人工智能、机器学习以及可再生能源，强调<br />了这些技术对未来生活的潜在影响。
                </span>
              </div>
            </div>
            <span class="text_34">
              这两段文字涵盖了日常生活与科技发展两个截<br />然不同的主题。
            </span>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<style lang="scss">
* {
  box-sizing: border-box;
}
.flex-row {
  display: flex;
  flex-direction: row;
}
.flex-col {
  display: flex;
  flex-direction: column;
}
.justify-start {
  display: flex;
  justify-content: flex-start;
}
.justify-center {
  display: flex;
  justify-content: center;
}
.justify-end {
  display: flex;
  justify-content: flex-end;
}
.justify-between {
  display: flex;
  justify-content: space-between;
}
.items-start {
  display: flex;
  align-items: flex-start;
}
.items-end {
  display: flex;
  align-items: flex-end;
}
.items-center {
  display: flex;
  align-items: center;
}
.no-shrink {
  flex-shrink: 0;
}
</style>
<style scoped>
/* BODY-111495 */
.body_111495 {
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

/* DIV */
.div {
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

/* DIV */
.div_1 {
  width: auto;
  height: auto;
  position: absolute;
  top: 32px;
  left: 16px;
  display: flex;
  flex-direction: row;
  justify-content: flex-start;
  align-items: flex-start;
  gap: 24px;
  flex-shrink: 0;
  right: 16px;
  bottom: 32px;
}

/* DIV */
.div_2 {
  width: 400px;
  height: auto;
  position: absolute;
  top: 0;
  left: 0;
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

/* H2 */
.text {
  width: 352px;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: flex-start;
  flex-shrink: 0;
  font-size: 24px;
  font-family: Roboto;
  font-weight: 700;
  line-height: 32px;
  color: #f3f4f6;
  white-space: pre;
  padding-top: 0;
  padding-right: 0;
  padding-bottom: 0;
  padding-left: 0;
}

/* DIV */
.div_3 {
  width: 352px;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: flex-start;
  gap: 8px;
  flex-shrink: 0;
  position: relative;
}

/* LABEL */
.text_1 {
  width: 352px;
  height: 20px;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: flex-start;
  flex-shrink: 0;
  font-size: 14px;
  font-family: Roboto;
  font-weight: 500;
  line-height: 20px;
  color: #d1d5db;
  white-space: pre;
  padding-top: 0;
  padding-right: 0;
  padding-bottom: 0;
  padding-left: 0;
}

/* IMG */
.img {
  width: 24px;
  height: auto;
  position: absolute;
  top: 8px;
  left: auto;
  flex-shrink: 0;
  object-fit: cover;
  right: 9px;
  bottom: 10px;
}

/* OPTION */
.option {
  width: 0;
  height: 0;
  position: absolute;
  top: -133px;
  left: -121px;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  align-items: flex-start;
  padding-right: 2px;
  padding-bottom: 1px;
  padding-left: 2px;
  gap: 10px;
  flex-shrink: 0;
}

/* OPTION */
.option_1 {
  width: 0;
  height: 0;
  position: absolute;
  top: -133px;
  left: -121px;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  align-items: flex-start;
  padding-right: 2px;
  padding-bottom: 1px;
  padding-left: 2px;
  gap: 10px;
  flex-shrink: 0;
}

/* OPTION */
.option_2 {
  width: 0;
  height: 0;
  position: absolute;
  top: -133px;
  left: -121px;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  align-items: flex-start;
  padding-right: 2px;
  padding-bottom: 1px;
  padding-left: 2px;
  gap: 10px;
  flex-shrink: 0;
}

/* 内置麦克风 */
.text_2 {
  position: absolute;
  top: 8px;
  left: 11px;
  flex-shrink: 0;
  font-size: 16px;
  font-family: Roboto;
  font-weight: 400;
  line-height: 24px;
  color: #d1d5db;
  white-space: pre;
  bottom: 10px;
  height: auto;
}

/* DIV */
.div_4 {
  width: auto;
  height: 40px;
  position: absolute;
  top: 158px;
  left: 24px;
  display: flex;
  flex-direction: row;
  justify-content: flex-start;
  align-items: center;
  flex-shrink: 0;
  right: 24px;
}

/* startRecording */
.text_3 {
  width: 96px;
  height: auto;
  position: absolute;
  top: 0;
  left: 0;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  align-items: flex-start;
  padding-top: 8px;
  padding-right: 16px;
  padding-bottom: 8px;
  padding-left: 16px;
  gap: 10px;
  flex-shrink: 0;
  border-radius: 4px;
  background-color: #2563eb;
  bottom: 0;
  text-align: center;
  font-size: 16px;
  font-family: Roboto;
  font-weight: 700;
  line-height: 24px;
  color: #ffffff;
  white-space: pre;
}

/* stopRecording */
.text_4 {
  width: 96px;
  height: auto;
  position: absolute;
  top: 0;
  left: 96px;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  align-items: flex-start;
  padding-top: 8px;
  padding-right: 16px;
  padding-bottom: 8px;
  padding-left: 16px;
  gap: 10px;
  flex-shrink: 0;
  border-radius: 4px;
  background-color: #dc2626;
  bottom: 0;
  text-align: center;
  font-size: 16px;
  font-family: Roboto;
  font-weight: 700;
  line-height: 24px;
  color: #ffffff;
  white-space: pre;
}

/* recordingStatus */
.text_5 {
  width: 44px;
  height: auto;
  position: absolute;
  top: 6px;
  left: 208px;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  align-items: flex-start;
  padding-top: 4px;
  padding-right: 8px;
  padding-bottom: 4px;
  padding-left: 8px;
  gap: 10px;
  flex-shrink: 0;
  border-radius: 9999px;
  background-color: #374151;
  bottom: 6px;
  font-size: 14px;
  font-family: Roboto;
  font-weight: 600;
  line-height: 20px;
  color: #d1d5db;
  white-space: pre;
}

/* microphoneTranscript */
.microphonetranscript {
  width: auto;
  height: 256px;
  position: absolute;
  top: 214px;
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
}

/* P */
.text_6 {
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

/* P */
.text_7 {
  width: auto;
  height: 32px;
  position: absolute;
  top: 56px;
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

/* P */
.text_8 {
  width: auto;
  height: 32px;
  position: absolute;
  top: 96px;
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

/* P */
.text_9 {
  width: auto;
  height: 32px;
  position: absolute;
  top: 136px;
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

/* P */
.text_10 {
  width: auto;
  height: 32px;
  position: absolute;
  top: 176px;
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
.div_5 {
  width: 400px;
  height: auto;
  position: absolute;
  top: 0;
  left: 0;
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
  margin-left: auto;
  margin-right: auto;
  bottom: 0;
}

/* H2 */
.text_11 {
  width: 352px;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: flex-start;
  flex-shrink: 0;
  font-size: 24px;
  font-family: Roboto;
  font-weight: 700;
  line-height: 32px;
  color: #f3f4f6;
  white-space: pre;
  padding-top: 0;
  padding-right: 0;
  padding-bottom: 0;
  padding-left: 0;
}

/* DIV */
.div_6 {
  width: 352px;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: flex-start;
  gap: 8px;
  flex-shrink: 0;
  position: relative;
}

/* LABEL */
.text_12 {
  width: 352px;
  height: 20px;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: flex-start;
  flex-shrink: 0;
  font-size: 14px;
  font-family: Roboto;
  font-weight: 500;
  line-height: 20px;
  color: #d1d5db;
  white-space: pre;
  padding-top: 0;
  padding-right: 0;
  padding-bottom: 0;
  padding-left: 0;
}

/* IMG */
.img_1 {
  width: 24px;
  height: auto;
  position: absolute;
  top: 8px;
  left: auto;
  flex-shrink: 0;
  object-fit: cover;
  right: 9px;
  bottom: 10px;
}

/* OPTION */
.option_3 {
  width: 0;
  height: 0;
  position: absolute;
  top: -133px;
  left: -545px;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  align-items: flex-start;
  padding-right: 2px;
  padding-bottom: 1px;
  padding-left: 2px;
  gap: 10px;
  flex-shrink: 0;
}

/* OPTION */
.option_4 {
  width: 0;
  height: 0;
  position: absolute;
  top: -133px;
  left: -545px;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  align-items: flex-start;
  padding-right: 2px;
  padding-bottom: 1px;
  padding-left: 2px;
  gap: 10px;
  flex-shrink: 0;
}

/* OPTION */
.option_5 {
  width: 0;
  height: 0;
  position: absolute;
  top: -133px;
  left: -545px;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  align-items: flex-start;
  padding-right: 2px;
  padding-bottom: 1px;
  padding-left: 2px;
  gap: 10px;
  flex-shrink: 0;
}

/* 内置扬声器 */
.text_13 {
  position: absolute;
  top: 8px;
  left: 11px;
  flex-shrink: 0;
  font-size: 16px;
  font-family: Roboto;
  font-weight: 400;
  line-height: 24px;
  color: #d1d5db;
  white-space: pre;
  bottom: 10px;
  height: auto;
}

/* DIV */
.div_7 {
  width: auto;
  height: 40px;
  position: absolute;
  top: 158px;
  left: 24px;
  display: flex;
  flex-direction: row;
  justify-content: flex-start;
  align-items: center;
  flex-shrink: 0;
  right: 24px;
}

/* startPlayback */
.text_14 {
  width: 96px;
  height: auto;
  position: absolute;
  top: 0;
  left: 0;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  align-items: flex-start;
  padding-top: 8px;
  padding-right: 16px;
  padding-bottom: 8px;
  padding-left: 16px;
  gap: 10px;
  flex-shrink: 0;
  border-radius: 4px;
  background-color: #16a34a;
  bottom: 0;
  text-align: center;
  font-size: 16px;
  font-family: Roboto;
  font-weight: 700;
  line-height: 24px;
  color: #ffffff;
  white-space: pre;
}

/* stopPlayback */
.text_15 {
  width: 96px;
  height: auto;
  position: absolute;
  top: 0;
  left: 96px;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  align-items: flex-start;
  padding-top: 8px;
  padding-right: 16px;
  padding-bottom: 8px;
  padding-left: 16px;
  gap: 10px;
  flex-shrink: 0;
  border-radius: 4px;
  background-color: #ca8a04;
  bottom: 0;
  text-align: center;
  font-size: 16px;
  font-family: Roboto;
  font-weight: 700;
  line-height: 24px;
  color: #ffffff;
  white-space: pre;
}

/* playbackStatus */
.text_16 {
  width: 44px;
  height: auto;
  position: absolute;
  top: 6px;
  left: 208px;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  align-items: flex-start;
  padding-top: 4px;
  padding-right: 8px;
  padding-bottom: 4px;
  padding-left: 8px;
  gap: 10px;
  flex-shrink: 0;
  border-radius: 9999px;
  background-color: #374151;
  bottom: 6px;
  font-size: 14px;
  font-family: Roboto;
  font-weight: 600;
  line-height: 20px;
  color: #d1d5db;
  white-space: pre;
}

/* speakerTranscript */
.speakertranscript {
  width: auto;
  height: 256px;
  position: absolute;
  top: 214px;
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

/* P */
.text_18 {
  width: auto;
  height: 32px;
  position: absolute;
  top: 56px;
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

/* P */
.text_19 {
  width: auto;
  height: 32px;
  position: absolute;
  top: 96px;
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

/* P */
.text_20 {
  width: auto;
  height: 32px;
  position: absolute;
  top: 136px;
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

/* P */
.text_21 {
  width: auto;
  height: 32px;
  position: absolute;
  top: 176px;
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

/* P */
.text_22 {
  width: auto;
  height: 32px;
  position: absolute;
  top: auto;
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
  bottom: 8px;
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
.text_23 {
  width: 352px;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: flex-start;
  flex-shrink: 0;
  font-size: 24px;
  font-family: Roboto;
  font-weight: 700;
  line-height: 32px;
  color: #f3f4f6;
  white-space: pre;
  padding-top: 0;
  padding-right: 0;
  padding-bottom: 0;
  padding-left: 0;
}

/* DIV */
.div_9 {
  width: 352px;
  height: 149px;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  align-items: flex-start;
  gap: 8px;
  flex-shrink: 0;
  position: relative;
}

/* LABEL */
.text_24 {
  width: 352px;
  height: 20px;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: flex-start;
  flex-shrink: 0;
  font-size: 14px;
  font-family: Roboto;
  font-weight: 500;
  line-height: 20px;
  color: #d1d5db;
  white-space: pre;
  padding-top: 0;
  padding-right: 0;
  padding-bottom: 0;
  padding-left: 0;
}

/* aiInput */
.text_25 {
  width: 352px;
  height: 114px;
  position: relative;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  align-items: flex-start;
  padding-top: 7px;
  padding-right: 11px;
  padding-bottom: 7px;
  padding-left: 11px;
  gap: 10px;
  flex-shrink: 0;
  overflow: hidden;
  border-radius: 6px;
  border-width: 1px 1px 1px 1px;
  border-style: solid;
  border-color: #4b5563;
  background-color: #374151;
  font-size: 16px;
  font-family: Roboto;
  font-weight: 400;
  line-height: 24px;
  color: #d1d5db;
  white-space: pre;
}

/* submitToAI */
.text_26 {
  width: 95px;
  height: 40px;
  position: absolute;
  top: 237px;
  left: 24px;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  align-items: flex-start;
  padding-top: 8px;
  padding-right: 16px;
  padding-bottom: 8px;
  padding-left: 16px;
  gap: 10px;
  flex-shrink: 0;
  border-radius: 4px;
  background-color: #9333ea;
  text-align: center;
  font-size: 16px;
  font-family: Roboto;
  font-weight: 700;
  line-height: 24px;
  color: #ffffff;
  white-space: pre;
}

/* aiResponse */
.airesponse {
  width: auto;
  height: 256px;
  position: absolute;
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

/* P */
.text_27 {
  width: 320px;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: flex-start;
  flex-shrink: 0;
  font-size: 16px;
  font-family: Roboto;
  font-weight: 400;
  line-height: 24px;
  color: #d1d5db;
  white-space: pre;
  padding-top: 0;
  padding-right: 0;
  padding-bottom: 0;
  padding-left: 0;
}

/* OL */
.ol {
  width: 320px;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: flex-start;
  gap: 8px;
  flex-shrink: 0;
  position: relative;
}

/* LI */
.li {
  position: relative;
  width: 320px;
  height: 72px;
  flex-shrink: 0;
  display: flex;
  flex-direction: column;
  align-items: flex-start;
}

/* flexContainer */
.flexcontainer {
  position: relative;
  display: flex;
  width: 100%;
  height: 24px;
  flex-direction: row;
  align-items: flex-start;
  isolation: isolate;
  margin-top: 0;
  margin-left: 0;
}

/* 1. */
.text_28 {
  font-size: 16px;
  font-family: Roboto;
  font-weight: 400;
  line-height: 24px;
  color: #d1d5db;
  white-space: pre;
  z-index: 1;
  position: relative;
  margin-left: 0;
  margin-top: 0;
}

/* 描述了一个阳光明媚的日子，计划去公园 */
.text_29 {
  position: absolute;
  top: 0;
  left: 17px;
  font-size: 16px;
  font-family: Roboto;
  font-weight: 400;
  line-height: 24px;
  color: #d1d5db;
  white-space: pre;
  right: 15px;
  width: auto;
  z-index: 0;
}

/* 野餐并希望结识新朋友。反映了对户外活动和
社交的期待。 */
.text_30 {
  position: absolute;
  top: auto;
  left: 0;
  font-size: 16px;
  font-family: Roboto;
  font-weight: 400;
  line-height: 24px;
  color: #d1d5db;
  right: 0;
  width: auto;
  bottom: 0;
}

/* LI */
.li_1 {
  position: relative;
  width: 320px;
  height: 72px;
  flex-shrink: 0;
  display: flex;
  flex-direction: column;
  align-items: flex-start;
}

/* flexContainer */
.flexcontainer_1 {
  position: relative;
  display: flex;
  width: 100%;
  height: 24px;
  flex-direction: row;
  align-items: flex-start;
  isolation: isolate;
  margin-top: 0;
  margin-left: 0;
}

/* 2. */
.text_31 {
  font-size: 16px;
  font-family: Roboto;
  font-weight: 400;
  line-height: 24px;
  color: #d1d5db;
  white-space: pre;
  z-index: 1;
  position: relative;
  margin-left: 0;
  margin-top: 0;
}

/* 讨论了一个科技相关的播客内容，主题包 */
.text_32 {
  position: absolute;
  top: 0;
  left: 17px;
  font-size: 16px;
  font-family: Roboto;
  font-weight: 400;
  line-height: 24px;
  color: #d1d5db;
  white-space: pre;
  right: 15px;
  width: auto;
  z-index: 0;
}

/* 括人工智能、机器学习以及可再生能源，强调
了这些技术对未来生活的潜在影响。 */
.text_33 {
  position: absolute;
  top: auto;
  left: 0;
  font-size: 16px;
  font-family: Roboto;
  font-weight: 400;
  line-height: 24px;
  color: #d1d5db;
  right: 0;
  width: auto;
  bottom: 0;
}

/* P */
.text_34 {
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: flex-start;
  flex-shrink: 0;
  font-size: 16px;
  font-family: Roboto;
  font-weight: 400;
  line-height: 24px;
  color: #d1d5db;
  padding-top: 0;
  padding-right: 0;
  padding-bottom: 0;
  padding-left: 0;
}
</style>
