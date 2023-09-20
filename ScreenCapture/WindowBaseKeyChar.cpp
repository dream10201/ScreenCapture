﻿#include "WindowBase.h"


bool WindowBase::OnKeyDown(const unsigned int& key) {
    switch (key)
    {
    case VK_DELETE: {
        auto shape = dynamic_cast<Shape::Text*>(History::GetLastDrawShape());
        if (!shape) return false;
        shape->DeleteWord(false);
        return false;
    }
    case VK_UP: {
        if (state == State::start) {
            SetCursorPos(pixelX, --pixelY);
            return true;
        }
        else if (state == State::maskReady) {
            SetCutBox(cutBox.x0, cutBox.y0 - 1, cutBox.x1, cutBox.y1 - 1);
            Refresh();
            return true;
        }
        return true;
    }
    case VK_DOWN: {
        if (state == State::start) {
            SetCursorPos(pixelX, ++pixelY);
            return true;
        }
        else if (state == State::maskReady) {
            SetCutBox(cutBox.x0, cutBox.y0+1, cutBox.x1, cutBox.y1+1);
            Refresh();
            return true;
        }
        return true;
    }
    case VK_LEFT: {
        if (state == State::start) {
            SetCursorPos(--pixelX, pixelY);
            return true;
        }
        else if (state == State::maskReady) {
            SetCutBox(cutBox.x0 - 1, cutBox.y0, cutBox.x1 - 1, cutBox.y1);
            Refresh();
            return true;
        }
        auto shape = dynamic_cast<Shape::Text*>(History::GetLastDrawShape());
        if (!shape) return false;
        if (shape->moveCursorIndex(true)) {
            Refresh();
        }
        return false;
    }
    case VK_RIGHT: {
        if (state == State::start) {
            SetCursorPos(++pixelX, pixelY);
            return true;
        }
        else if (state == State::maskReady) {
            SetCutBox(cutBox.x0 + 1, cutBox.y0, cutBox.x1 + 1, cutBox.y1);
            Refresh();
            return true;
        }
        auto shape = dynamic_cast<Shape::Text*>(History::GetLastDrawShape());
        if (!shape) return false;
        if (shape->moveCursorIndex(false)) {
            Refresh();
        }
        return false;
    }
    case VK_ESCAPE: {
        quitApp(3);
        return false;
    }
    case VK_SHIFT: {
        IsShiftDown = true;
        return false;
    }
    case VK_CONTROL: {
        IsCtrlDown = true;
        return false;
    }
    case 82: { //R
        if (IsCtrlDown) {
            SetClipboardText(hwnd, getPixelRgb());
            quitApp(4);
            return true;
        }
        return false;
    }
    case 72: { //H
        if (IsCtrlDown) {
            SetClipboardText(hwnd, getPixelHex());
            quitApp(5);
            return true;
        }
        return false;
    }
    case 90: { //Z
        if (IsCtrlDown) {
            mouseEnterMainToolIndex = 9;
            History::Undo();
            return true;
        }
        return false;
    }
    case 89: { //Y
        if (IsCtrlDown) {
            mouseEnterMainToolIndex = 10;
            History::Redo();
            return true;
        }
        return false;
    }
    }
    return false;
}

bool WindowBase::OnKeyUp(const unsigned int& key)
{
    switch (key)
    {
    case VK_SHIFT: {
        IsShiftDown = false;

    }
    case VK_CONTROL: {
        IsCtrlDown = false;
    }
    }
    return false;
}

bool WindowBase::OnChar(const unsigned int& val) {
    if (val == 13) { //enter
        History::LastShapeDrawEnd();
        return 1;
    }
    auto shape = dynamic_cast<Shape::Text*>(History::GetLastDrawShape());
    if (!shape) return 0;
    if (val == 8) {
        shape->DeleteWord();
    }
    else
    {
        shape->InsertWord(std::wstring{(wchar_t)val});
    }
    return true;
}