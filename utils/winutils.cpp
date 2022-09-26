#include "winutils.h"

#include <QApplication>
#include <Windows.h>
#include <Shlwapi.h>
#pragma comment(lib, "ShLwApi")
#include <strsafe.h>
#include <Uxtheme.h>
#pragma comment(lib, "Uxtheme")

static LPCSTR lpSubKey =
    "Software\\Microsoft\\Windows\\CurrentVersion\\Run";

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
    // Find app file name and trim ".exe"
    LPSTR lpValueName = StrRChrA(__argv[0], NULL, '\\') + 1;
    StrTrimA(lpValueName, ".exe");
    LSTATUS lReturn;

    if (enable)
    {
        // startup command: "{app path}" -silent
        const int cbData = 0xFF;
        char lpData[cbData] = "";

        // format %s with app path
        StringCbPrintfA(
            lpData, cbData,
            "\"%s\" -silent", __argv[0]);

        lReturn = RegSetKeyValueA(
            HKEY_CURRENT_USER,
            lpSubKey,
            lpValueName,
            REG_SZ, lpData, cbData);
    }
    else
    {
        lReturn = RegDeleteKeyValueA(
            HKEY_CURRENT_USER,
            lpSubKey,
            lpValueName);
    }

    if (lReturn == ERROR_SUCCESS)
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
    LPCSTR lpTheme = theme.toUtf8().data();

    if (lstrcmpiA(lpTheme, "Windows") == 0)
    {
        // Turn off visual style
        SetWindowTheme((HWND)winId, TEXT(" "), TEXT(" "));
    }
    else
    {
        // Enable visual style
        SetWindowTheme((HWND)winId, TEXT("Explorer"), NULL);
    }
}
