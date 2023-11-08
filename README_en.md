# QtUnblockNeteaseMusic

[![CodeFactor](https://www.codefactor.io/repository/github/frzmtrsprt/qtunblockneteasemusic/badge)](https://www.codefactor.io/repository/github/frzmtrsprt/qtunblockneteasemusic)
![GitHub commit activity](https://img.shields.io/github/commit-activity/m/FrzMtrsprt/QtUnblockNeteaseMusic)
![GitHub all releases](https://img.shields.io/github/downloads/FrzMtrsprt/QtUnblockNeteaseMusic/total)

A desktop client for UnblockNeteaseMusic, made with Qt.

## Features
- Choose the UNM server's starting arguments
- Remembers the options from the last run in a config file
- View real time log output from the server
- Dark theme support

## Supports
The original [nondanee/UnblockNeteaseMusic](https://github.com/nondanee/UnblockNeteaseMusic)

The enhanced [UnblockNeteaseMusic/server](https://github.com/UnblockNeteaseMusic/server)

## Screenshot
![Screenshot](https://github.com/FrzMtrsprt/QtUnblockNeteaseMusic/raw/main/screenshot.png)

## Usage
1. Download the latest version from [releases](https://github.com/FrzMtrsprt/QtUnblockNeteaseMusic/releases)
2. Install [Node.js Runtime](https://nodejs.org/en/download) if you wish to use script server instead of packaged executable
3. Download the server from [nondanee/UnblockNeteaseMusic](https://github.com/nondanee/UnblockNeteaseMusic) or [UnblockNeteaseMusic/server](https://github.com/UnblockNeteaseMusic/server), and place it in the same directory with `QtUnblockNeteaseMusic.exe`
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
4. Run `QtUnblockNeteaseMusic.exe` and set the necessary arguments
5. Set the proxy in the Netease Cloud Music client accordingly & enjoy!
> [!IMPORTANT]
> When used on your personal devices, avoid using the default `8080` port, and change the host address to `127.0.0.1` to avoid unwanted access from others.

> [!NOTE]
> Newer client versions have stricter SSL verification, so if you encountered the "connection failed" issue, enable the HTTPS port and install UnblockNeteaseMusic CA certificate. Right click on `QtUnblockNeteaseMusic.exe`, select `Run as administrator`, and click `Install certificate` in the `Advance` menu.

## Build
1. Set up the Qt development environment (Qt 6.4+)
2. Clone the repository
3. Open the project in Qt Creator / VS Code (extra extensions needed)
4. Compile & Run
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
