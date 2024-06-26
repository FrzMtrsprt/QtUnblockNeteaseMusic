#include "winutils.h"

#include <QFile>
#include <QProcess>

#include <Windows.h>
#include <ShlObj.h>
#include <uxtheme.h>
#include <wininet.h>

using namespace Qt::StringLiterals;

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
void WinUtils::setStartup(const bool &enable, const bool &silent)
{
    LPCSTR lpStartupKey = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";

    // Find app file name and remove name extension
    char *lpFileName = strrchr(__argv[0], '\\') + 1;
    char *context = NULL;
    LPCSTR lpValueName = strtok_s(lpFileName, ".", &context);

    if (enable)
    {
        char lpValueData[MAX_PATH];
        silent
            ? sprintf_s(lpValueData, MAX_PATH, "\"%s\" --silent", __argv[0])
            : sprintf_s(lpValueData, MAX_PATH, "\"%s\"", __argv[0]);

        const bool ok = SUCCEEDED(
            RegSetKeyValueA(
                HKEY_CURRENT_USER,
                lpStartupKey,
                lpValueName,
                REG_SZ, lpValueData, MAX_PATH));

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

bool WinUtils::setSystemProxy(const bool &enable, const QString &address)
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

    WCHAR proxy_server[MAX_PATH];
    const int length = address.toWCharArray(proxy_server);
    proxy_server[length] = L'\0';
    options[1].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
    options[1].Value.pszValue = proxy_server;

    WCHAR proxy_bypass[8] = L"<local>";
    options[2].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
    options[2].Value.pszValue = proxy_bypass;

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

bool WinUtils::isSystemProxy(const QString &address)
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
        WCHAR proxy_server[MAX_PATH];
        const int length = address.toWCharArray(proxy_server);
        proxy_server[length] = L'\0';

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

// Check if current user is administrator
bool WinUtils::isAdmin()
{
    return IsUserAnAdmin();
}

// Install CA certificate and return the output
std::tuple<bool, QString, QString> WinUtils::installCA(const QString &caPath)
{
    QFile caFile(caPath);
    caFile.open(QIODevice::ReadOnly);
    const QByteArray caData = caFile.readAll();
    caFile.close();
    if (caData.isEmpty())
    {
        return {false, QObject::tr("Unable to read CA certificate."), QString()};
    }

    // Convert CA certificate to binary
    // Reference: https://stackoverflow.com/a/34643021
    BYTE pbCryptBuf[0x1000];
    DWORD cbCryptBuf = sizeof(pbCryptBuf);
    if (!CryptStringToBinaryA(
            caData.constData(), caData.size(),
            CRYPT_STRING_BASE64_ANY,
            pbCryptBuf,
            &cbCryptBuf,
            NULL, NULL))
    {
        return {false, QObject::tr("Unable to convert CA certificate."),
                getErrorMessage()};
    }

    HCERTSTORE hStore = CertOpenStore(
        CERT_STORE_PROV_SYSTEM,
        NULL, NULL,
        CERT_SYSTEM_STORE_LOCAL_MACHINE,
        L"ROOT");
    if (!hStore)
    {
        switch (LOWORD(GetLastError()))
        {
        case ERROR_ACCESS_DENIED:
            return {false, QObject::tr("Access denied.\n"
                                       "Please run QtUnblockNeteaseMusic as "
                                       "Administrator from context menu."),
                    getErrorMessage()};
        default:
            return {false, QObject::tr("Unable to open CA certificate store."),
                    getErrorMessage()};
        }
    }

    if (!CertAddEncodedCertificateToStore(
            hStore,
            X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
            pbCryptBuf, cbCryptBuf,
            CERT_STORE_ADD_REPLACE_EXISTING,
            NULL))
    {
        CertCloseStore(hStore, 0);
        return {false, QObject::tr("Unable to add CA certificate to store."),
                getErrorMessage()};
    }

    CertCloseStore(hStore, 0);
    return {true, QObject::tr("The certificate is installed."), QString()};
}

QString WinUtils::getErrorMessage()
{
    DWORD dwErr = GetLastError();
    WCHAR lpMsgBuf[0x100];
    FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, GetLastError(),
        LANG_USER_DEFAULT,
        lpMsgBuf, sizeof(lpMsgBuf),
        NULL);
    return QObject::tr("Error %1: %2")
        .arg(QString::asprintf("0x%08x", dwErr))
        .arg(QString::fromWCharArray(lpMsgBuf));
}
