{
  "targets": [
    {
      "target_name": "qt_audio_recorder",
      "sources": [ "qt_audio_recorder.cpp" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        # "/opt/homebrew/Cellar/qt@5/5.15.16/include",
        # "/opt/homebrew/Cellar/qt@5/5.15.16/include/QtMultimedia",
        # "/opt/homebrew/Cellar/qt@5/5.15.16/include/QtCore",
        "/Users/zhaojuchang/.nvm/versions/node/v18.19.0/bin/node"
        "<!(pkg-config --cflags Qt5Core Qt5Multimedia Qt5Gui Qt5Widgets)",
      ],
      "libraries": [
        "-L/opt/homebrew/Cellar/qt@5/5.15.16/lib",
        # "-lQtCore",
        # "-lQtMultimedia"
        # "-L/opt/homebrew/Cellar/qt@5/5.15.16/include/QtMultimedia",
        # "-L/opt/homebrew/Cellar/qt@5/5.15.16/include/QtCore"
        "<!(pkg-config --libs Qt5Core Qt5Multimedia Qt5Gui Qt5Widgets)"
      ],
      "cflags": [ "-std=c++11" ]
    }
  ]
}