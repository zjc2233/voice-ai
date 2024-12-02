const qtAudioRecorder = require('./build/Release/qt_audio_recorder');

// 调用本地模块的函数
// 获取麦克风输入设备列表
qtAudioRecorder.getMicrophoneInputs((inputs) => {
    console.log('Available microphone inputs:', inputs);
    if (inputs.length > 0) {

        // 开始录制
        qtAudioRecorder.startRecording(inputs[1], (err) => {
            if (err) {
                console.error(err);
                return;
            }
            console.log('Recording started.');
        });
    } else {
        console.error('No speaker outputs available.');
    }
});

// 获取扬声器输出设备列表
qtAudioRecorder.getSpeakerOutputs((outputs) => {
    console.log('Available speaker outputs:', outputs);
    // 选择第一个设备名称
    // if (outputs.length > 0) {
    //     const deviceName = outputs[0];
    //     console.log('Selected device:', deviceName);

    //     // 开始录制
    //     qtAudioRecorder.startRecording(outputs[0], (err) => {
    //         if (err) {
    //             console.error(err);
    //             return;
    //         }
    //         console.log('Recording started.');
    //     });
    // } else {
    //     console.error('No speaker outputs available.');
    // }
});

// setTimeout(() => {
//     // 停止录音
//     console.log('停止录音');
//     // qtAudioRecorder.stopRecording({});
// }, 5000);