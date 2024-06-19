#include <windows.h>
#include <string>

// Глобальные переменные
HINSTANCE hInst;
LPCWSTR szTitle = L"File Cleaner";
LPCWSTR szWindowClass = L"WINAPI_FILE_CLEANER";

// Прототипы функций
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DeleteTemporaryFiles(HWND hWnd, const wchar_t* directory);
void ScanTemporaryFiles(HWND hWnd, const wchar_t* directory);

// Точка входа приложения
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassExW(&wcex))
    {
        MessageBox(NULL, L"Call to RegisterClassEx failed!", L"Windows Desktop Guided Tour", NULL);
        return 1;
    }

    hInst = hInstance;

    HWND hWnd = CreateWindowW(
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        500, 300,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!hWnd)
    {
        MessageBox(NULL, L"Call to CreateWindow failed!", L"Windows Desktop Guided Tour", NULL);
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

// Оконная процедура
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case 1: // ID кнопки "Delete"
            DeleteTemporaryFiles(hWnd, L"C:\\Windows\\Temp");
            break;
        case 2: // ID кнопки "Scan"
            ScanTemporaryFiles(hWnd, L"C:\\Windows\\Temp");
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_CREATE:
        CreateWindow(L"BUTTON", L"Scan Temporary Files", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 10, 220, 40, hWnd, (HMENU)2, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), nullptr);
        CreateWindow(L"BUTTON", L"Delete Temporary Files", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 60, 220, 40, hWnd, (HMENU)1, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), nullptr);
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Функции для сканирования и удаления временных файлов
void ScanTemporaryFiles(HWND hWnd, const wchar_t* directory)
{
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile((std::wstring(directory) + L"\\*").c_str(), &findFileData);
    unsigned long long totalSize = 0;
    int fileCount = 0;

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                std::wstring filePath = std::wstring(directory) + L"\\" + std::wstring(findFileData.cFileName);
                LARGE_INTEGER size;
                size.LowPart = findFileData.nFileSizeLow;
                size.HighPart = findFileData.nFileSizeHigh;
                totalSize += size.QuadPart;
                fileCount++;
            }
        } while (FindNextFile(hFind, &findFileData) != 0);

        FindClose(hFind);
        std::wstring message = L"Files found: " + std::to_wstring(fileCount) + L"\nTotal size: " + std::to_wstring(totalSize) + L" bytes";
        MessageBox(hWnd, message.c_str(), L"Scan Complete", MB_OK | MB_ICONINFORMATION);
    }
    else {
        MessageBox(hWnd, L"No files found.", L"Error", MB_OK | MB_ICONERROR);
    }
}

void DeleteTemporaryFiles(HWND hWnd, const wchar_t* directory)
{
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile((std::wstring(directory) + L"\\*").c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        MessageBox(hWnd, L"No files found to delete.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    do {
        if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            std::wstring filePath = std::wstring(directory) + L"\\" + std::wstring(findFileData.cFileName);
            if (DeleteFile(filePath.c_str()) == 0) {
                MessageBox(hWnd, (L"Failed to delete file: " + std::wstring(findFileData.cFileName)).c_str(), L"Error", MB_OK | MB_ICONERROR);
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
    MessageBox(hWnd, L"Temporary files deleted successfully.", L"Operation Complete", MB_OK | MB_ICONINFORMATION);
}
