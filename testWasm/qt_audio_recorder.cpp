#include "/Users/zhaojuchang/Documents/myProject/electron/voice-ai/node_modules/nan/nan.h"
#include <QAudioRecorder>
#include <QAudioEncoderSettings>
#include <QUrl>
#include <QTimer>
#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QCoreApplication> 

void StartRecording(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  // 使用绝对路径
  QString absolutePath = QDir::toNativeSeparators(QDir::current().filePath("output.wav"));
  qDebug() << "Output file path:" << absolutePath;

  QAudioRecorder recorder;
  QAudioEncoderSettings settings;
  settings.setCodec("audio/pcm");
  settings.setQuality(QMultimedia::HighQuality);
  recorder.setAudioSettings(settings);
  recorder.setOutputLocation(QUrl::fromLocalFile(absolutePath));

  // 启动录制
  recorder.record();
  qDebug() << "Recording started.";

  // 创建 QCoreApplication 实例
  int argc = 0;
  char *argv[] = { nullptr };
  QCoreApplication app(argc, argv);

  // 设置定时器，录制 5 秒后停止
  QTimer *timer = new QTimer();
  timer->setSingleShot(true);
  QObject::connect(timer, &QTimer::timeout, [&recorder]() {
    recorder.stop();
    qDebug() << "Recording stopped.";
  });
  timer->start(5000);

  // 运行 QCoreApplication 事件循环
  app.exec();

  // 释放定时器
  delete timer;
}

void Init(v8::Local<v8::Object> exports) {
  Nan::SetMethod(exports, "startRecording", StartRecording);
}

NODE_MODULE(qt_audio_recorder, Init)