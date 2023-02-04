#ifndef WINUTILS_H
#define WINUTILS_H

#include <QStyle>
#include <QWindow>

#include <Windows.h>

class WinUtils
{
public:
    WinUtils();

    static void setStartup(const bool &enable);
    static void setThrottle(const bool &enable);
    static void setWindowFrame(const WId &winId, const QStyle *style);
    static bool setSystemProxy(const bool &enable, const QString &address, const QString &port);
    static bool isSystemProxy(const QString &address, const QString &port);

private:
    static bool setVisualStyleToWindow(const HWND &hWnd, const bool &enable);
};

#endif // WINUTILS_H
