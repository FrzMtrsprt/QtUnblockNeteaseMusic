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
- Compile `CMakeLists.txt` & Run
- If you want to generate a complete package with libraries,  
  copy `QtUnblockNeteaseMusic.exe` to a new directory and run `windeployqt.exe .\QtUnblockNeteaseMusic.exe`

## Thanks
This little tool wouldn't have been made possible without these incredible projects:
- [Qt](https://github.com/qt): Providing a powerful but easy-to-use development framework
- [nondanee/UnblockNeteaseMusic](https://github.com/nondanee/UnblockNeteaseMusic): The project that started everything
- [UnblockNeteaseMusic/server](https://github.com/UnblockNeteaseMusic/server): Maintaining and enhancing the UNM server
- [barry-ran/QtScrcpy](https://github.com/barry-ran/QtScrcpy): I learnt how to use Qt by shamelessly copying his codes

## To be solved
- ~~The log output from the enhanced server is messy ([#791](https://github.com/UnblockNeteaseMusic/server/issues/791))~~ Fixed in upstream ([#798](https://github.com/UnblockNeteaseMusic/server/pull/798))
- Build everything with cmake ~~Done~~ There's a bug in Qt 6.3.1 preventing lupdate from working with CMake
- Untested/unsupported on macOS, Linux, and Windows 7 or lower
- I'm a newbie
