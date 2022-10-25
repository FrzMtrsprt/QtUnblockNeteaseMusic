#include "winutils.h"

#include <QApplication>
#include <Windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi")
#include <Shlwapi.h>
#pragma comment(lib, "ShLwApi")
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

    if (enable)
    {
        // startup command: "{app path}" -silent
        char lpData[MAX_PATH];

        // format %s with app path
        sprintf_s(lpData, MAX_PATH, "\"%s\" -silent", __argv[0]);

        if (!SUCCEEDED(
                RegSetKeyValueA(
                    HKEY_CURRENT_USER,
                    lpStartupKey,
                    lpValueName,
                    REG_SZ, lpData, MAX_PATH)))
        {
            qWarning("%s: Unable to set startup.", __FUNCTION__);
        }
    }
    else
    {
        if (!SUCCEEDED(
                RegDeleteKeyValueA(
                    HKEY_CURRENT_USER,
                    lpStartupKey,
                    lpValueName)))
        {
            qWarning("%s: Unable to delete startup.", __FUNCTION__);
        }
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

// Set window frame according to theme
void WinUtils::setWindowFrame(const WId &winId, const QString &theme)
{
    const HWND hWnd = (HWND)winId;
    const QByteArray szTheme = theme.toUtf8();
    const bool bClassic = lstrcmpiA(szTheme, "Windows") == 0;
    const bool bDarkAware = lstrcmpiA(szTheme, "Fusion") == 0;

    // Use dark window frame only when:
    // Theme is not classic, theme is darkmode aware, and Windows is in darkmode
    const bool bDark = !bClassic && bDarkAware && useDarkTheme();

    qDebug("Setting theme \"%s\" %s",
           szTheme.data(),
           bClassic ? "with classic light border"
                    : (bDark ? "with dark border"
                             : "with light border"));

    setDarkBorderToWindow(hWnd, bDark);

    // Disable / enable visual style
    if (!SUCCEEDED(bClassic ? SetWindowTheme(hWnd, L" ", L" ")
                            : SetWindowTheme(hWnd, NULL, NULL)))
    {
        qWarning("%s: Unable to set visual style.", __FUNCTION__);
    }
}

// Query Windows theme from registry
bool WinUtils::useDarkTheme()
{
    DWORD dwData = 1;
    DWORD cbData = sizeof(DWORD);

    RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme",
        RRF_RT_REG_DWORD,
        NULL,
        &dwData,
        &cbData);

    return dwData == 0;
}

// Set dark border to window
// Reference: qt/qtbase.git/tree/src/plugins/platforms/windows/qwindowswindow.cpp
bool WinUtils::setDarkBorderToWindow(const HWND &hwnd, const bool &d)
{
    const BOOL darkBorder = d ? TRUE : FALSE;
    const bool ok =
        SUCCEEDED(
            DwmSetWindowAttribute(
                hwnd, DwmwaUseImmersiveDarkMode, &darkBorder, sizeof(darkBorder))) ||
        SUCCEEDED(
            DwmSetWindowAttribute(
                hwnd, DwmwaUseImmersiveDarkModeBefore20h1, &darkBorder, sizeof(darkBorder)));
    if (!ok)
        qWarning("%s: Unable to set dark window border.", __FUNCTION__);
    return ok;
}
