#include "winutils.h"

#include <QString>
#include <QStyle>
#include <QWindow>

#include <Windows.h>
#include <dwmapi.h>
#include <shlwapi.h>
#include <uxtheme.h>

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
    0UL};

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

        const bool ok = SUCCEEDED(
            RegSetKeyValueA(
                HKEY_CURRENT_USER,
                lpStartupKey,
                lpValueName,
                REG_SZ, lpData, MAX_PATH));

        if (!ok)
        {
            qWarning("%s: Unable to set startup.", __FUNCTION__);
        }
    }
    else
    {
        const bool ok = SUCCEEDED(
            RegDeleteKeyValueA(
                HKEY_CURRENT_USER,
                lpStartupKey,
                lpValueName));

        if (!ok)
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
void WinUtils::setWindowFrame(const WId &winId, const QStyle *style)
{
    const HWND hWnd = (HWND)winId;
    const QByteArray szTheme = style->name().toUtf8();
    const bool bClassic = lstrcmpiA(szTheme, "Windows") == 0;

    qDebug("Setting theme \"%s\" %s",
           szTheme.data(),
           bClassic ? "with classic border"
                    : "");

    setVisualStyleToWindow(hWnd, !bClassic);
}

// Disable / enable visual style
bool WinUtils::setVisualStyleToWindow(const HWND &hWnd, const bool &enable)
{
    LPCWSTR pszSubAppName = enable ? NULL : L" ";
    LPCWSTR pszSubIdList = enable ? NULL : L" ";
    const bool ok = SUCCEEDED(SetWindowTheme(hWnd, pszSubAppName, pszSubIdList));
    if (!ok)
    {
        qWarning("%s: Unable to set visual style.", __FUNCTION__);
    }
    return ok;
}
