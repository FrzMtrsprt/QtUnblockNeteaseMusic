#include "winutils.h"

#include <QProcess>

#include <Windows.h>
#include <uxtheme.h>
#include <wininet.h>

using namespace Qt::Literals::StringLiterals;

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
    char *lpFileName = strrchr(__argv[0], '\\') + 1;
    char *context = NULL;
    LPCSTR lpValueName = strtok_s(lpFileName, ".", &context);

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

    qDebug("Setting %s window frame for theme \"%s\"",
           bClassic ? "classic"
                    : "normal",
           szTheme.data());

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

bool WinUtils::setSystemProxy(const bool &enable, const QString &address, const QString &port)
{
    INTERNET_PER_CONN_OPTION_LISTW optionList;
    INTERNET_PER_CONN_OPTIONW options[3];

    optionList.dwSize = sizeof(optionList);
    optionList.pszConnection = NULL;
    optionList.dwOptionCount = 3;
    optionList.pOptions = options;

    options[0].dwOption = INTERNET_PER_CONN_FLAGS;
    options[0].Value.dwValue = enable ? PROXY_TYPE_PROXY
                                      : PROXY_TYPE_DIRECT;

    WCHAR proxy_server[32];
    (address + ":" + port).toWCharArray(proxy_server);
    options[1].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
    options[1].Value.pszValue = proxy_server;

    options[2].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
    options[2].Value.pszValue = NULL;

    if (InternetSetOptionW(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION,
                           &optionList, optionList.dwSize))
    {
        InternetSetOptionW(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);
        InternetSetOptionW(NULL, INTERNET_OPTION_REFRESH, NULL, 0);
        return true;
    }
    qWarning("%s: Unable to set system proxy.", __FUNCTION__);
    return false;
}

bool WinUtils::isSystemProxy(const QString &address, const QString &port)
{
    INTERNET_PER_CONN_OPTION_LISTW optionList;
    INTERNET_PER_CONN_OPTIONW options[2];

    optionList.dwSize = sizeof(optionList);
    optionList.pszConnection = NULL;
    optionList.dwOptionCount = 2;
    optionList.pOptions = options;

    options[0].dwOption = INTERNET_PER_CONN_FLAGS;
    options[1].dwOption = INTERNET_PER_CONN_PROXY_SERVER;

    if (InternetQueryOptionW(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION,
                             &optionList, &optionList.dwSize))
    {
        WCHAR proxy_server[32];
        (address + ":" + port).toWCharArray(proxy_server);

        if (options[0].Value.dwValue & PROXY_TYPE_PROXY &&
            lstrcmpW(options[1].Value.pszValue, proxy_server) == 0)
        {
            return true;
        }
    }
    else
    {
        qWarning("%s: Unable to get system proxy.", __FUNCTION__);
    }
    return false;
}

// Install CA certificate and return the output
std::tuple<bool, QString, QString> WinUtils::installCA(const QString &caPath)
{
    QProcess process;
    process.start(u"certutil"_s, {u"-addstore"_s, u"-f"_s, u"root"_s, caPath},
                  QProcess::ReadOnly);
    process.waitForFinished();
    const QString detail = QString::fromLocal8Bit(process.readAllStandardOutput());
    switch (LOWORD(process.exitCode()))
    {
    case ERROR_SUCCESS:
        return {true,
                QObject::tr("UnblockNeteaseMusic CA is installed."),
                detail};
    case ERROR_ACCESS_DENIED:
        return {false,
                QObject::tr("Access denied.\n"
                            "Please run QtUnblockNeteaseMusic as "
                            "Administrator from context menu."),
                detail};
    default:
        return {false,
                QObject::tr("Unknown error occured.\n"
                            "Please see details below."),
                detail};
    }
}
