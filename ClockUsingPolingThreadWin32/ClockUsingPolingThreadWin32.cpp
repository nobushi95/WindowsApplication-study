﻿// ClockUsingPolingThreadWin32.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "ClockUsingPolingThreadWin32.h"
#include <string>
#include <format>

#define MAX_LOADSTRING 100
constexpr auto SyncClockEventName = L"ClockUsingPolingThreadWin32_SyncClockEventName";

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

struct MyThreadArg
{
    HWND hWnd;
    HANDLE ContinueClockEvent;
    bool IsThreadContinue;
};

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI ThreadFunc(LPVOID);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR    lpCmdLine,
                      _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLOCKUSINGPOLINGTHREADWIN32, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行
    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLOCKUSINGPOLINGTHREADWIN32));

    MSG msg;

    // メインメッセージ ループ
    BOOL bRet;
    while ((bRet = GetMessage(&msg, nullptr, 0, 0)) != 0)
    {
        if (bRet == -1)
        {
            break;
        }
        else
        {
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    // ダブルクリックのウィンドウメッセージを受けてとれるようにする
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLOCKUSINGPOLINGTHREADWIN32));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CLOCKUSINGPOLINGTHREADWIN32);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & ~WS_SYSMENU & ~WS_THICKFRAME, // ~WS_THICKFRAMEでResizeできなくなる
                              CW_USEDEFAULT, CW_USEDEFAULT, 200, 100, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
        return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static MyThreadArg threadArg;
    static HANDLE threadHandle;
    static DWORD threadId;
    static bool isClockStopped = false;
    switch (message)
    {
        case WM_CREATE:
        {
            threadArg.ContinueClockEvent = CreateEventW(nullptr, TRUE, TRUE, SyncClockEventName);
            threadArg.IsThreadContinue = true;
            threadArg.hWnd = hWnd;
            threadHandle = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)ThreadFunc, &threadArg, 0, &threadId);
            break;
        }
        case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 選択されたメニューの解析:
            switch (wmId)
            {
                case IDM_ABOUT:
                {
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                    break;
                }
                case IDM_EXIT:
                {
                    DestroyWindow(hWnd);
                    break;
                }
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        }
        case WM_LBUTTONDOWN:
        {
            if (isClockStopped)
                SetEvent(threadArg.ContinueClockEvent);
            else
                ResetEvent(threadArg.ContinueClockEvent);

            isClockStopped = !isClockStopped;
            break;
        }
        case WM_LBUTTONDBLCLK:
        {
            DestroyWindow(hWnd);
            break;
        }
        case WM_RBUTTONDBLCLK:
        {
            DestroyWindow(hWnd);
            break;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: HDC を使用する描画コードをここに追加してください...
            EndPaint(hWnd, &ps);
            break;
        }
        case WM_DESTROY:
        {
            // スレッドを終了
            if (isClockStopped)
                SetEvent(threadArg.ContinueClockEvent);
            threadArg.IsThreadContinue = false;
            WaitForSingleObject(threadHandle, INFINITE);

            // ハンドルを閉じる
            CloseHandle(threadHandle);
            CloseHandle(threadArg.ContinueClockEvent);
            PostQuitMessage(0);
            break;
        }
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

DWORD WINAPI ThreadFunc(LPVOID threadArg)
{
    auto lpd = reinterpret_cast<MyThreadArg*>(threadArg);
    auto hdc = GetDC(lpd->hWnd);
    SetTextColor(hdc, RGB(0, 0, 255));

    while (lpd->IsThreadContinue)
    {
        Sleep(100);
        SYSTEMTIME st;
        GetLocalTime(&st);
        auto time = std::format(L"{:02d}:{:02d}:{:02d}", st.wHour, st.wMinute, st.wSecond);
        TextOutW(hdc, 5, 5, time.c_str(), time.length());
        WaitForSingleObject(lpd->ContinueClockEvent, INFINITE);
    }
    ReleaseDC(lpd->hWnd, hdc);
    return 0;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            return (INT_PTR)TRUE;

        case WM_COMMAND:
        {
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }
            break;
        }
    }
    return (INT_PTR)FALSE;
}
