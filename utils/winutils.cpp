#include "winutils.h"

#include <QApplication>
#include <Windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi")
#include <Shlwapi.h>
#pragma comment(lib, "ShLwApi")
#include <strsafe.h>
#include <Uxtheme.h>
#pragma comment(lib, "Uxtheme")

enum : WORD
{
    DwmwaUseImmersiveDarkMode = 20,
    DwmwaUseImmersiveDarkModeBefore20h1 = 19
};

static PROCESS_POWER_THROTTLING_STATE Throttle{
    PROCESS_POWER_THROTTLING_CURRENT_VERSION,
    PROCESS_POWER_THROTTLING_EXECUTION_SPEED,
    PROCESS_POWER_THROTTLING_EXECUTION_SPEED};

static PROCESS_POWER_THROTTLING_STATE Unthrottle{
    PROCESS_POWER_THROTTLING_CURRENT_VERSION,
    PROCESS_POWER_THROTTLING_EXECUTION_SPEED,
    NULL};

WinUtils::WinUtils() {}

// Enable or disable startup
void WinUtils::setStartup(const bool &enable)
{
    LPCSTR lpStartupKey = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";

    // Find app file name and remove name extension
    LPSTR lpValueName = PathFindFileNameA(__argv[0]);
    PathRemoveExtensionA(lpValueName);
    bool bSucceed;

    if (enable)
    {
        // startup command: "{app path}" -silent
        const int cbData = 0xFF;
        char lpData[cbData] = "";

        // format %s with app path
        StringCbPrintfA(
            lpData, cbData,
            "\"%s\" -silent", __argv[0]);

        bSucceed = SUCCEEDED(
            RegSetKeyValueA(
                HKEY_CURRENT_USER,
                lpStartupKey,
                lpValueName,
                REG_SZ, lpData, cbData));
    }
    else
    {
        bSucceed = SUCCEEDED(
            RegDeleteKeyValueA(
                HKEY_CURRENT_USER,
                lpStartupKey,
                lpValueName));
    }

    if (bSucceed)
    {
        qDebug() << (enable ? "Startup set"
                            : "Startup deleted");
    }
}

// Enable or disable EcoQoS
// See https://devblogs.microsoft.com/performance-diagnostics/introducing-ecoqos/
void WinUtils::setThrottle(const bool &enable)
{
    HANDLE hProcess = GetCurrentProcess();
    SetProcessInformation(
        hProcess,
        ProcessPowerThrottling,
        enable ? &Throttle : &Unthrottle,
        sizeof(PROCESS_POWER_THROTTLING_STATE));
    SetPriorityClass(
        hProcess,
        enable ? IDLE_PRIORITY_CLASS : NORMAL_PRIORITY_CLASS);
}

// Enable basic window frame when theme is "Windows"
void WinUtils::setWindowFrame(const WId &winId, const QString &theme)
{
    const HWND hWnd = (HWND)winId;
    const QByteArray themeArray = theme.toUtf8();
    LPCSTR lpTheme = themeArray.data();

    if (lstrcmpiA(lpTheme, "Windows") == 0)
    {
        // Disable visual style
        SetWindowTheme(hWnd, TEXT(" "), TEXT(" "));

        qDebug() << "Set theme \"Window\" with classic border";
    }
    else
    {
        // Enable visual style & dark boarder
        SetWindowTheme(hWnd, TEXT("Explorer"), NULL);

        const bool bFusion = lstrcmpiA(lpTheme, "Fusion") == 0;
        const bool bDark = bFusion && !UseLightTheme();
        const bool bRet = setDarkBorderToWindow(hWnd, bDark);

        qDebug() << "Set theme"
                 << theme
                 << (bRet ? (bDark ? "with dark border" : "with light border") : "");
    }
}

// Query Windows theme from registry
bool WinUtils::UseLightTheme()
{
    DWORD buffer;
    DWORD cbData = sizeof(DWORD);

    const LSTATUS lReturn = RegGetValue(
        HKEY_CURRENT_USER,
        TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize"),
        TEXT("AppsUseLightTheme"),
        RRF_RT_REG_DWORD,
        NULL,
        &buffer,
        &cbData);
    if (lReturn == ERROR_SUCCESS)
    {
        return (bool)buffer;
    }
    return true;
}

// Set dark border to window
// Reference: qt/qtbase.git/tree/src/plugins/platforms/windows/qwindowswindow.cpp
bool WinUtils::setDarkBorderToWindow(HWND hwnd, bool d)
{
    const BOOL darkBorder = d ? TRUE : FALSE;
    const bool ok =
        SUCCEEDED(DwmSetWindowAttribute(hwnd, DwmwaUseImmersiveDarkMode, &darkBorder, sizeof(darkBorder)))
        || SUCCEEDED(DwmSetWindowAttribute(hwnd, DwmwaUseImmersiveDarkModeBefore20h1, &darkBorder, sizeof(darkBorder)));
    if (!ok)
        qWarning("%s: Unable to set dark window border.", __FUNCTION__);
    return ok;
}