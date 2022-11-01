# QtUnblockNeteaseMusic
 A desktop client for UnblockNeteaseMusic, written in Qt.

[![CodeFactor](https://www.codefactor.io/repository/github/frzmtrsprt/qtunblockneteasemusic/badge)](https://www.codefactor.io/repository/github/frzmtrsprt/qtunblockneteasemusic)
![GitHub commit activity](https://img.shields.io/github/commit-activity/m/FrzMtrsprt/QtUnblockNeteaseMusic)
![GitHub all releases](https://img.shields.io/github/downloads/FrzMtrsprt/QtUnblockNeteaseMusic/total)


## Features
- Choose the UNM server's starting arguments
- Remembers the options from the last run in a config file
- View real time log output from the server
- Auto detects the server's file, node or packaged executable
- Dark theme support

## Supports
The original [nondanee/UnblockNeteaseMusic](https://github.com/nondanee/UnblockNeteaseMusic)

And the enhanced [UnblockNeteaseMusic/server](https://github.com/UnblockNeteaseMusic/server)

## Screenshot
![image](https://github.com/FrzMtrsprt/QtUnblockNeteaseMusic/blob/main/screenshot.png)

## Usage
1. Download the latest version from [releases](https://github.com/FrzMtrsprt/QtUnblockNeteaseMusic/releases)
2. Install [Node.js Runtime](https://nodejs.org/en/download/current/) if you wish to use node server instead of packaged executable
3. Download the server from [nondanee/UnblockNeteaseMusic](https://github.com/nondanee/UnblockNeteaseMusic) or [UnblockNeteaseMusic/server](https://github.com/UnblockNeteaseMusic/server),  
and place it in the same directory with `QtUnblockNeteaseMusic.exe`
```
QtUnblockNeteaseMusic  
├─ ...  
├─ QtUnblockNeteaseMusic.exe  
└─ server  
   ├─ ...  
   └─ app.js
```
or
```
QtUnblockNeteaseMusic  
├─ ...  
├─ QtUnblockNeteaseMusic.exe  
└─ unblockneteasemusic.exe
```
3. Run `QtUnblockNeteaseMusic.exe` and set the necessary arguments  
(127.0.0.1 and a custom port other than 8080 is recommended for personal use)
5. Set the proxy in the Netease Cloud Music client accordingly & enjoy!

## Build
1. Set up the Qt development environment (Qt 6.4.0)
2. Clone the repository
3. Open the project in VSCode / Qt Creator
4. Compile & Run
- When compiling on Windows, MSVC v143 + Windows 11 SDK (10.0.22621.0) is recommended, to support Windows specific features like EcoQoS.
- If you want to generate a complete package with libraries,  
  copy `QtUnblockNeteaseMusic.exe` to a new directory and run `windeployqt.exe .\QtUnblockNeteaseMusic.exe`

## Thanks
This little tool wouldn't have been made possible without these incredible projects:
- [Qt](https://github.com/qt): Providing a powerful but easy-to-use development framework
- [nondanee/UnblockNeteaseMusic](https://github.com/nondanee/UnblockNeteaseMusic): The project that started everything
- [UnblockNeteaseMusic/server](https://github.com/UnblockNeteaseMusic/server): Maintaining and enhancing the UNM server

## To be solved
- ~~The log output from the enhanced server is messy ([#791](https://github.com/UnblockNeteaseMusic/server/issues/791))~~ Fixed in upstream ([#798](https://github.com/UnblockNeteaseMusic/server/pull/798))
- Build everything with cmake ~~Done~~  
  There's a bug in Qt 6 preventing lupdate from working with CMake, thus we have to lupdate manually
- Untested/unsupported on macOS, Linux, and Windows 8 or lower
- I'm a newbie
