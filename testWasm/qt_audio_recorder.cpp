#include "/Users/zhaojuchang/Documents/myProject/electron/voice-ai/node_modules/nan/nan.h"
#include <QAudioRecorder>
#include <QAudioInput>
#include <QAudioDeviceInfo>
#include <QAudioEncoderSettings>
#include <QUrl>
#include <QTimer>
#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QCoreApplication>
#include <QApplication>
#include <QByteArray>
#include <QBuffer>
#include <QThread>


QAudioRecorder *audioRecorder = nullptr;  // 全局变量
QAudioInput *audioInput = nullptr; 
QBuffer *buffer = nullptr;
QThread* audioThread = nullptr;

void StopRecording();  // 声明 StopRecording 函数

void GetMicrophoneInputs(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  // 检查是否已经有一个 audioRecorder 实例
  if (audioRecorder != nullptr) {
    delete audioRecorder;
    audioRecorder = nullptr;
  }

  // 创建新的 QAudioRecorder 实例
  audioRecorder = new QAudioRecorder;

  // 获取系统可用的麦克风输入设备列表
  QStringList inputs = audioRecorder->audioInputs();
  qDebug() << "Available microphone inputs:" << inputs;

  // 将 QStringList 转换为 v8::Array
  v8::Local<v8::Array> result = Nan::New<v8::Array>(inputs.size());
  for (int i = 0; i < inputs.size(); i++) {
    Nan::Set(result, i, Nan::New(inputs[i].toStdString()).ToLocalChecked());
  }

  // 调用回调函数
  if (info.Length() > 0 && info[0]->IsFunction()) {
    Nan::Callback callback(info[0].As<v8::Function>());
    const unsigned argc = 1;
    v8::Local<v8::Value> argv[argc] = { result };
    callback.Call(argc, argv);
  }
}

void GetSpeakerOutputs(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  // 获取系统可用的扬声器输出设备列表
  QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
  QStringList outputs;
  for (const QAudioDeviceInfo &device : devices) {
    outputs.append(device.deviceName());
  }
  qDebug() << "Available speaker outputs:" << outputs;

  // 将 QStringList 转换为 v8::Array
  v8::Local<v8::Array> result = Nan::New<v8::Array>(outputs.size());
  for (int i = 0; i < outputs.size(); i++) {
    Nan::Set(result, i, Nan::New(outputs[i].toStdString()).ToLocalChecked());
  }

  // 调用回调函数
  if (info.Length() > 0 && info[0]->IsFunction()) {
    Nan::Callback callback(info[0].As<v8::Function>());
    const unsigned argc = 1;
    v8::Local<v8::Value> argv[argc] = { result };
    callback.Call(argc, argv);
  }
}


void ReadAndPrintPCMData() {
  if (buffer != nullptr && buffer->bytesAvailable() > 0) {
    QByteArray data = buffer->readAll();
    // 打印 PCM 数据
    qDebug() << "PCM Data:" << data;
  }
}

void StartRecording(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  // 检查是否已经有一个 audioInput 实例
  if (audioInput != nullptr) {
    delete audioInput;
    audioInput = nullptr;
  }

  // 获取扬声器设备名称
  if (info.Length() < 1 || !info[0]->IsString() || info[0]->IsNullOrUndefined()) {
    Nan::ThrowTypeError("Expected a non-null string argument for device name");
    return;
  }
  QString deviceName = QString::fromUtf8(*Nan::Utf8String(info[0]));

  // 获取系统可用的扬声器输出设备列表
  QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
  QAudioDeviceInfo selectedDevice;
  for (const QAudioDeviceInfo &device : devices) {
  qDebug() << "可用设备名称:" << device.deviceName();
    if (device.deviceName() == deviceName) {
      selectedDevice = device;
      break;
    }
  }

  if (selectedDevice.isNull()) {
    Nan::ThrowError(Nan::New("Device not found: " + deviceName.toStdString()).ToLocalChecked());
    return;
  }

  // 设置音频格式
  QAudioFormat format;
  format.setSampleRate(44100);
  format.setChannelCount(1);
  format.setSampleSize(16);
  format.setCodec("audio/pcm");
  format.setByteOrder(QAudioFormat::LittleEndian);
  format.setSampleType(QAudioFormat::SignedInt);

  if (!selectedDevice.isFormatSupported(format)) {
    Nan::ThrowError(Nan::New("Format not supported for device: " + deviceName.toStdString()).ToLocalChecked());
    return;
  }

  // 创建 QAudioInput 实例
  audioInput = new QAudioInput(selectedDevice, format);

  // 创建 QBuffer 实例
  buffer = new QBuffer();
  buffer->open(QIODevice::ReadWrite);

  // 开始录制
  audioInput->start(buffer);

  // 设置定时器，录制 3 秒后停止
  // QTimer::singleShot(3000, [=]() {
  //   qDebug() << "Recording timed out.";
  //   StopRecording();
  // });

  qDebug() << "Recording started.";

  // // 读取并打印 PCM 数据
  // while (true) {
  //   qDebug() << "PCM Data:" << buffer;
  //   if (buffer->bytesAvailable() > 0) {
  //     QByteArray data = buffer->readAll();
  //     // 打印 PCM 数据
  //     qDebug() << "PCM Data:" << data;
  //   }

  //   // 处理事件循环
  //   QCoreApplication::processEvents();
  // }

    // 创建并启动 QThread
  audioThread = new QThread();
  QObject::connect(audioThread, &QThread::started, [=]() {
    // 设置定时器，定期读取并打印 PCM 数据
    QTimer* timer = new QTimer();
    QObject::connect(timer, &QTimer::timeout, ReadAndPrintPCMData);
    timer->start(100); // 每 100 毫秒读取一次数据
  });
  audioThread->start();

}

void StopRecording() {
  if (audioInput != nullptr) {
    audioInput->stop();
    delete audioInput;
    audioInput = nullptr;
  }

  if (buffer != nullptr) {
    buffer->close();
    QByteArray data = buffer->data();
    qDebug() << "Recorded data:" << data;
    delete buffer;
    buffer = nullptr;
  }

  qDebug() << "Recording stopped.";
  
}


void Init(v8::Local<v8::Object> exports) {
  // 启动 Qt 事件循环
  int argc = 0;
  char** argv = nullptr;
  QApplication app(argc, argv);

  Nan::SetMethod(exports, "getMicrophoneInputs", GetMicrophoneInputs);
  Nan::SetMethod(exports, "getSpeakerOutputs", GetSpeakerOutputs);
  Nan::SetMethod(exports, "startRecording", StartRecording);
  // Nan::SetMethod(exports, "stopRecording", StopRecording);
}

NODE_MODULE(qt_audio_recorder, Init)
