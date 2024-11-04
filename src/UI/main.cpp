#include "WindowMain.h"

#include <Windows.h>
#include <DbgHelp.h>

#include <QMessageBox>
#include <opencv2/core/utils/logger.hpp>

static std::mutex mtx;

static bool isOpen()
{
    HANDLE hMutex = CreateMutex(NULL, true, TEXT("T08lJ8CJmJiyoxdV"));
    return GetLastError() == ERROR_ALREADY_EXISTS;
}

static long ExceptionFilter(_EXCEPTION_POINTERS* ExceptionInfo)
{
    std::lock_guard<std::mutex> lock(mtx);
    WCHAR szFileName[MAX_PATH]{ 0 };
    WCHAR szVersion[]{ TEXT("MHY_Scanner") };
    SYSTEMTIME stLocalTime;
    GetLocalTime(&stLocalTime);
    wsprintfW(szFileName, L"%s-%04d%02d%02d-%02d%02d%02d.dmp",
              szVersion, stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
              stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond);
    HANDLE hFile{ CreateFileW(szFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL) };
    if (hFile != INVALID_HANDLE_VALUE)
    {
        MINIDUMP_EXCEPTION_INFORMATION ExInfo;
        ExInfo.ThreadId = GetCurrentThreadId();
        ExInfo.ExceptionPointers = ExceptionInfo;
        ExInfo.ClientPointers = FALSE;
        //写入 dmp 文件
        BOOL bOK{ MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL) };
        CloseHandle(hFile);
        LPCWSTR lpText = L"即将退出，已写入小型内存转储";
        LPCWSTR lpCaption = L"意外崩溃！";
        UINT uType = MB_OK | MB_ICONINFORMATION;
        MessageBoxW(NULL, lpText, lpCaption, uType);
    }
    return EXCEPTION_EXECUTE_HANDLER;
}

int main(int argc, char* argv[])
{
    UINT utf8 = 65001;
    SetConsoleOutputCP(utf8);
    SetUnhandledExceptionFilter(ExceptionFilter);
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
    QApplication a(argc, argv);
    if (isOpen())
    {
        HWND hwnd = FindWindowW(NULL, L"MHY扫码器");
        if (hwnd == NULL)
        {
            return 0;
        }
        if (GetForegroundWindow() == hwnd)
        {
            return 0;
        }
        ShowWindow(hwnd, SW_MINIMIZE);
        ShowWindow(hwnd, SW_SHOWNORMAL);
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
        SetForegroundWindow(hwnd);
        SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
        return -1;
    }
    WindowMain w;
    w.show();
    return a.exec();
}
