#ifndef WINUTILS_H
#define WINUTILS_H

#include <QApplication>
#include <Windows.h>

class WinUtils
{
public:
    WinUtils();

    static void setStartup(const bool &enable);
    static void setThrottle(const bool &enable);
    static void setWindowFrame(const WId &winId, const QString &theme);

private:
    static bool useLightTheme();
    static bool setDarkBorderToWindow(HWND hwnd, bool d);
};

#endif // WINUTILS_H
