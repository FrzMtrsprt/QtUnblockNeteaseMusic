# QtUnblockNeteaseMusic
 A desktop client for UnblockNeteaseMusic, written in Qt.

## Features
- Choose the UNM server's starting arguments
- Remembers the options from the last run in a config file
- View real time log output from the server
- Auto detects the server's file, node or packaged executable

## Supports
The original [nondanee/UnblockNeteaseMusic](https://github.com/nondanee/UnblockNeteaseMusic)

And the enhanced [UnblockNeteaseMusic/server](https://github.com/UnblockNeteaseMusic/server)

## Screenshot
![image](https://github.com/FrzMtrsprt/QtUnblockNeteaseMusic/blob/main/screenshot.png)

## Build
- Set up the Qt development environment (Qt 6.3.1)
- Clone the repository
- Open the project in VSCode / Qt Creator
- Compile `CMakeLists.txt` with CMake
- Compile the `qt_*.ts` localization files with Linguist
- Create a `locale` folder next to the compiled binaries
- Place the compiled `qt_*.qm` files and the `qtbase_*.qm` files from Qt installation in the `locale` folder
- The final product should have the following directory structure:  
```
  ├─ res  
  │  └─ icon.png  
  ├─ locale  
  │  ├─ qt_en.qm  
  │  ├─ qt_zh_CN.qm  
  │  ├─ qtbase_en.qm  
  │  └─ qtbase_zh_CN.qm  
  └─ QtUnblockNeteaseMusic.exe
```
## To be solved
- ~~The log output from the enhanced server is messy ([#791](https://github.com/UnblockNeteaseMusic/server/issues/791))~~ Fixed in upstream ([#798](https://github.com/UnblockNeteaseMusic/server/pull/798))
- Build everything with cmake
- Untested/unsupported on macOS, Linux, and Windows 7 or lower
- I'm a newbie
