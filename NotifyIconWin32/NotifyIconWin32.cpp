#include "framework.h"
#include <shellapi.h>
#include "NotifyIconWin32.h"

#define MAX_LOADSTRING 100
constexpr auto MYTRAY_ID = 100;
constexpr auto WM_MYTRAY = (WM_APP + 0);

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR    lpCmdLine,
                      _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_NOTIFYICONWIN32, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NOTIFYICONWIN32));

    MSG msg;

    // メイン メッセージ ループ:
    BOOL bRet;
    while ((bRet = GetMessage(&msg, nullptr, 0, 0) != 0))
    {
        if (bRet == -1)
            break;
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

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NOTIFYICONWIN32));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_NOTIFYICONWIN32);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
        return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static NOTIFYICONDATA ni;
    static bool isTrayIconExist = false;
    switch (message)
    {
        case WM_RBUTTONDOWN:
        {
            // 右クリックでポップアップメニューを表示するように変更
            auto menu = LoadMenuW(hInst, MAKEINTRESOURCEW(IDC_NOTIFYICONWIN32));
            auto subMenu = GetSubMenu(menu, 0);
            POINT pt = { LOWORD(lParam), HIWORD(lParam) };
            ClientToScreen(hWnd, &pt);
            TrackPopupMenu(subMenu, TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, nullptr);
            DestroyMenu(menu);
            break;
        }
        case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 選択されたメニューの解析:
            switch (wmId)
            {
                case ID_TRAY_IN:
                {
                    // トレイアイコン表示
                    ni = { 0 };
                    ni.cbSize = sizeof(NOTIFYICONDATA);
                    ni.hWnd = hWnd;
                    ni.uID = MYTRAY_ID;
                    ni.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
                    ni.hIcon = (HICON)LoadImageW(hInst, MAKEINTRESOURCEW(IDI_SMALL), IMAGE_ICON, 0, 0, 0);
                    ni.uCallbackMessage = WM_MYTRAY;
                    lstrcpyW(ni.szTip, L"MY TASK TRAY");
                    Shell_NotifyIconW(NIM_ADD, &ni);
                    isTrayIconExist = true;
                    break;
                }
                case ID_TRAY_OUT:
                {
                    // トレイアイコン非表示
                    Shell_NotifyIconW(NIM_DELETE, &ni);
                    isTrayIconExist = false;
                    break;
                }
                case IDM_ABOUT:
                {
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                    break;
                }
                case ID_TRAY_EXIT:
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
        case WM_MYTRAY:
        {
            switch (lParam)
            {
                case WM_RBUTTONDOWN:
                {
                    // 右クリックでトレイアイコンの場所(クリック位置)にポップアップメニューを表示
                    auto menu = LoadMenuW(hInst, MAKEINTRESOURCEW(IDR_TRAY_MENU));
                    auto subMenu = GetSubMenu(menu, 0);
                    POINT pt{ 0 };
                    GetCursorPos(&pt);
                    // トレイアイコン以外の場所がクリックされるとメニューをすぐに外すためにWindowを前面に持ってくる
                    SetForegroundWindow(hWnd);
                    TrackPopupMenu(subMenu, TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, nullptr);
                    DestroyMenu(menu);
                    break;
                }
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        }
        case WM_DESTROY:
        {
            // トレイアイコンから削除
            if (isTrayIconExist)
                Shell_NotifyIconW(NIM_DELETE, &ni);
            PostQuitMessage(0);
            break;
        }
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

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
