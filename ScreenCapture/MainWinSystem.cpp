#include "MainWin.h"
#include "dwmapi.h"
#include "resource.h"
#include <Commctrl.h>
#include <Shobjidl.h> 
#include <atlbase.h>
#include <Gdiplus.h> 
#include <format>

LRESULT CALLBACK MainWin::RouteWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCCREATE)
    {
        CREATESTRUCT* pCS = reinterpret_cast<CREATESTRUCT*>(lParam);
        LPVOID pThis = pCS->lpCreateParams;
        SetWindowLongPtr(hWnd, 0, reinterpret_cast<LONG_PTR>(pThis));
    }
    auto obj = reinterpret_cast<MainWin*>(GetWindowLongPtrW(hWnd, 0));
    return obj->WindowProc(hWnd, msg, wParam, lParam);
}



void MainWin::createWindow()
{
    WNDCLASSEX wcx{};
    wcx.cbSize = sizeof(wcx);
    wcx.style = CS_HREDRAW | CS_VREDRAW;
    wcx.lpfnWndProc = &MainWin::RouteWindowMessage;
    wcx.cbWndExtra = sizeof(MainWin*);
    wcx.hInstance = hinstance;
    wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcx.lpszClassName = L"ScreenCapture";
    if (!RegisterClassEx(&wcx))
    {
        MessageBox(NULL, L"注册窗口类失败", L"系统提示", NULL);
        return;
    }
    hwnd = CreateWindowEx(0, wcx.lpszClassName, wcx.lpszClassName, WS_OVERLAPPEDWINDOW, painter->x, painter->y, painter->w, painter->h, NULL, NULL, hinstance, static_cast<LPVOID>(this));
    BOOL attrib = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &attrib, sizeof(attrib));
}

void MainWin::showWindow()
{
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    ChangeCursor(IDC_ARROW);
}

void MainWin::initScaleFactor()
{
    int dpi = GetDpiForWindow(hwnd);
    switch (dpi) {
    case 96:
        scaleFactor = 1.0;
        break;
    case 120:
        scaleFactor = 1.25;
        break;
    case 144:
        scaleFactor = 1.5;
        break;
    case 168:
        scaleFactor = 1.75;
        break;
    case 192:
        scaleFactor = 2.0;
        break;
    case 216:
        scaleFactor = 2.25;
        break;
    case 240:
        scaleFactor = 2.5;
        break;
    case 288:
        scaleFactor = 3.0;
        break;
    case 336:
        scaleFactor = 3.5;
        break;
    default:
        scaleFactor = 1.0;
        break;
    }
}

LRESULT CALLBACK MainWin::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_NCCALCSIZE:
        {
            return 0;
        }
        case WM_SETCURSOR: {
            return 1;
        }
        case WM_TIMER: {
            if (wParam == 999) {
                History::LastShapeDraw(MouseDownPos, MouseDownPos);
            }
            else if (wParam == 998) {
                saveClipboard();
            }
            return 1;
        }
        case WM_PAINT:
        {
            auto paintCtx = painter->PaintBoard();
            paintCtx->setFillStyle(BLRgba32(0, 0, 0, 180));
            paintCtx->fillBoxArray(maskBoxes, 8);
            paintCtx->setStrokeStyle(BLRgba32(22, 119, 255, 255));
            paintCtx->setStrokeWidth(cutBoxBorderWidth);
            paintCtx->strokeBox(cutBox);
            drawToolMain();
            paintCtx->end();
            painter->PaintOnWindow(hwnd);
            return 0;
        }
        case WM_LBUTTONDBLCLK:
        {
            saveClipboard();
            return 0;
        }
        case WM_RBUTTONDOWN:
        {
            POINT point = getMousePoint(lParam);
            rightBtnDown(point);
            return 0;
        }
        case WM_MOUSEMOVE:
        {
            POINT point = getMousePoint(lParam);
            mouseMove(point);
            return 0;
        }
        case WM_LBUTTONDOWN:
        {
            if (state >= State::maskReady) {
                static auto t1 = std::chrono::system_clock::now();
                auto t2 = std::chrono::system_clock::now();
                auto count = floor<std::chrono::milliseconds>(t2 - t1).count();
                if (count > 0 && count < 300) {
                    if (state == State::text) {
                        if (History::Get()->size() < 1) return 0;
                        auto shape = (Shape::Text*)History::Get()->at(History::Get()->size() - 1);
                        shape->onlyDrawText = true;
                        shape->Draw(0, 0, 0, 0);
                        SetTimer(hwnd, 998, 60, (TIMERPROC)NULL);
                    }
                    else
                    {
                        saveClipboard();
                    }                    
                    return 0;
                }
                t1 = t2;
            }
            POINT point = getMousePoint(lParam);
            leftBtnDown(point);
            return 0;
        }
        case WM_LBUTTONUP:
        {
            POINT point = getMousePoint(lParam);
            leftBtnUp(point);
            return 0;
        }
        case WM_ERASEBKGND:
        {
            return 1;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 1;
        }
        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE: {
                    return 0;
                }
                case VK_SHIFT: {
                    IsShiftDown = true;
                    return 0;
                }                             
            }
            return 0;
        }
        case WM_KEYUP: 
        {
            switch (wParam)
            {
                case VK_SHIFT: {
                    IsShiftDown = false;
                    return 0;
                }
            }
            return 0;
        }
        case WM_CHAR:
        {
            if (state != State::text) return 0;
            auto history = History::Get();
            if (history->size() < 1) return 0;
            if (wParam == 13) {
                History::LastShapeDrawEnd();
                return 1;
            }
            auto shape = (Shape::Text*)history->at(history->size() - 1);
            if (wParam == 8) {
                shape->DeleteWord();
            }
            else
            {
                shape->InsertWord(std::wstring{(wchar_t)wParam});
            } 
            return 1;
        }
        case WM_IME_STARTCOMPOSITION:
        {
            break;
        }
        case WM_IME_ENDCOMPOSITION:
        {
            break;
        }
        case WM_IME_NOTIFY:
        {
            switch (wParam)
            {
                case IMN_SETOPENSTATUS:
                    break;
                default:
                    break;
            }
            break;
        }
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}


POINT MainWin::getMousePoint(const LPARAM& lParam)
{
    POINT point;
    point.x = GET_X_LPARAM(lParam);
    point.y = GET_Y_LPARAM(lParam);
    return point;
}


void MainWin::saveFile()
{
    SYSTEMTIME localTime;
    GetLocalTime(&localTime);
    std::wstring name = std::to_wstring(localTime.wYear)+ 
        std::to_wstring(localTime.wMonth) + 
        std::to_wstring(localTime.wDay) + 
        std::to_wstring(localTime.wHour) + 
        std::to_wstring(localTime.wMinute) + 
        std::to_wstring(localTime.wSecond) + 
        std::to_wstring(localTime.wMilliseconds);
    ATL::CComPtr<IFileSaveDialog> saveDialog;
    auto result = saveDialog.CoCreateInstance(CLSID_FileSaveDialog);
    saveDialog->SetDefaultExtension(L"png");
    saveDialog->SetFileName(name.c_str());
    saveDialog->SetTitle(L"保存文件");
    //saveDialog->SetFilter(L"Image\0*.png\0");
    if (FAILED(result)) return;
    result = saveDialog->SetOptions(FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST | FOS_OVERWRITEPROMPT);
    if (FAILED(result)) return;
    result = saveDialog->Show(hwnd);
    if (FAILED(result)) return;
    CComPtr<IShellItem> item;
    result = saveDialog->GetResult(&item);
    if (FAILED(result)) return;
    LPWSTR filePath;
    item->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
    auto w = cutBox.x1 - cutBox.x0;
    auto h = cutBox.y1 - cutBox.y0;
    BLImage imgSave(w, h, BL_FORMAT_PRGB32);
    painter->paintCtx->begin(imgSave);
    painter->paintCtx->blitImage(BLPoint(0,0), *painter->bgImage, BLRectI(cutBox.x0, cutBox.y0, w, h));
    painter->paintCtx->blitImage(BLPoint(0,0), *painter->canvasImage, BLRectI(cutBox.x0, cutBox.y0, w, h));
    painter->paintCtx->end();
    auto filePathUtf8 = ConvertToUTF8(filePath);
    imgSave.writeToFile(filePathUtf8.c_str());
    CoTaskMemFree(filePath);
    CloseWindow(hwnd);
    PostQuitMessage(0);    
}
void MainWin::saveClipboard()
{
    size_t x = painter->x + cutBox.x0 + cutBoxBorderWidth / 2;
    size_t y = painter->y + cutBox.y0 + cutBoxBorderWidth / 2;
    size_t w = cutBox.x1 - cutBox.x0- cutBoxBorderWidth;
    size_t h = cutBox.y1 - cutBox.y0- cutBoxBorderWidth;
    HDC ScreenDC = ::GetDC(NULL);
    HDC hMemDC = CreateCompatibleDC(ScreenDC);
    HBITMAP hBitmap = CreateCompatibleBitmap(ScreenDC, w, h);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
    StretchBlt(hMemDC, 0, 0, w, h, ScreenDC, x, y, w, h, SRCCOPY);
    hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
    DeleteDC(hMemDC);
    DeleteObject(hOldBitmap);
    OpenClipboard(hwnd);
    EmptyClipboard();
    SetClipboardData(CF_BITMAP, hBitmap);
    CloseClipboard();
    ReleaseDC(NULL,ScreenDC);
    CloseWindow(hwnd);
    PostQuitMessage(0);
}