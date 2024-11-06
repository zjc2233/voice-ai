<script setup lang="ts">
import { ref } from 'vue'

const microphoneList = ref([])

const getMicro = () => {
  // 获取麦克风设备
  navigator.mediaDevices
    .enumerateDevices()
    .then((devices) => {
      console.log('devices---------------------------', devices)
      devices.forEach((device) => {
        if (device.kind == 'audioinput') {
          console.log(`麦克风: ${JSON.stringify(device)}`);
        }
      })
    })
    .catch((err) => {
      console.error(`错误发生 ${err.name}: ${err.message}`)
    })
}
</script>

<template>
  <div class="body_111495">
    <el-button type="danger" @click="getMicro()">按钮</el-button>
    {{ microphoneList }}
    <div class="div">
      <div class="div_1">
        <div class="div_2">
          <span class="text"> 麦克风输入 </span>
          <div class="div_3">
            <span class="text_1"> 选择麦克风 </span>
            <div class="microphone">
              <img
                class="img"
                src="https://seal-img.nos-jd.163yun.com/obj/w5rCgMKVw6DCmGzCmsK-/56293312575/0579/e266/ef05/ef0d5f9b0dd3ddc582782e4f4b44dd00.png"
              />
              <div class="option"></div>
              <div class="option_1"></div>
              <div class="option_2"></div>
              <span class="text_2"> 内置麦克风 </span>
            </div>
          </div>
          <div class="div_4">
            <span class="text_3"> 开始录音 </span><span class="text_4"> 停止录音 </span
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
            <div class="speaker">
              <img
                class="img_1"
                src="https://seal-img.nos-jd.163yun.com/obj/w5rCgMKVw6DCmGzCmsK-/56293312575/0579/e266/ef05/ef0d5f9b0dd3ddc582782e4f4b44dd00.png"
              />
              <div class="option_3"></div>
              <div class="option_4"></div>
              <div class="option_5"></div>
              <span class="text_13"> 内置扬声器 </span>
            </div>
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

<style>
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

/* microphone */
.microphone {
  width: 352px;
  height: 42px;
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

/* speaker */
.speaker {
  width: 352px;
  height: 42px;
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
