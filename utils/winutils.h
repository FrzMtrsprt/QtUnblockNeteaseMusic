#ifndef WINUTILS_H
#define WINUTILS_H

#include <QString>
#include <Windows.h>

class WinUtils
{
public:
    WinUtils();
    static void setStartup(const bool &enable);
    static void setThrottle(const bool &enable);
    static void setWindowFrame(const HWND &hWnd, const QString &theme);
};

#endif // WINUTILS_H
