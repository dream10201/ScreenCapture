﻿#include "MainWin.h"
#include <format>

void MainWin::drawMaskBoxes() {
    PaintCtx->setFillStyle(BLRgba32(0, 0, 0, 160));
    PaintCtx->fillBoxArray(maskBoxes, 8);

    auto str = std::format("X:{} Y:{} W:{} H:{}", cutBox.x0, cutBox.y0, cutBox.x1 - cutBox.x0, cutBox.y1 - cutBox.y0);
    auto font = Font::Get()->fontText;
    font->setSize(20.0);
    BLFontMetrics fm = font->metrics();
    BLTextMetrics tm;
    BLGlyphBuffer gb;
    gb.setUtf8Text(str.c_str()); //utf8.c_str()
    font->shape(gb);
    font->getTextMetrics(gb, tm);
    


    BLBox infoTextBox;
    infoTextBox.x0 = cutBox.x0;
    if (infoTextBox.x0 < x) {
        infoTextBox.x0 = x;
    }
    infoTextBox.y0 = cutBox.y0 - 46;
    if (infoTextBox.y0 < 0) {
        infoTextBox.y0 = cutBox.y1 + 6;
    }
    if (infoTextBox.y0 + 40 > h) {
        infoTextBox.y0 = cutBox.y0 + 6;
    }
    infoTextBox.x1 = infoTextBox.x0 + tm.boundingBox.x1 - tm.boundingBox.x0 + 18;
    infoTextBox.y1 = infoTextBox.y0 +40;
    PaintCtx->fillBox(infoTextBox);

    
    
    
    PaintCtx->setFillStyle(BLRgba32(255,255, 255));
    PaintCtx->fillUtf8Text(BLPoint(infoTextBox.x0+8, infoTextBox.y1-12), *font, str.c_str());
}

void MainWin::SetCutBoxWhenLeftButtonDown() {
    dragStartCutBoxStartPos = BLPoint(cutBox.x0, cutBox.y0);
    dragStartCutBoxEndPos = BLPoint(cutBox.x1, cutBox.y1);
    if (mouseInMaskBoxIndex < 8) {
        SetCutBox(MouseDownPos.x, MouseDownPos.y);
    }
}

void MainWin::MouseMoveWhenMaskReady(const int& x, const int& y) {
    int index = -1;
    for (size_t i = 0; i < 8; i++)
    {
        if (maskBoxes[i].contains(x, y)) {
            index = i;
            break;
        }
    }
    if (index == -1) index = 8;
    if (mouseInMaskBoxIndex == index) return;
    mouseInMaskBoxIndex = index;
    LPCTSTR cursor = IDC_SIZENWSE;
    switch (index)
    {
        case 1:
        case 5:
        {
            cursor = IDC_SIZENS;
            break;
        }
        case 2:
        case 6:
        {
            cursor = IDC_SIZENESW;
            break;
        }
        case 3:
        case 7:
        {
            cursor = IDC_SIZEWE;
            break;
        }
        case 8:
        {
            cursor = IDC_SIZEALL;
            break;
        }
    }
    ChangeCursor(cursor);
}

void MainWin::SetCutBox(const int& x1, const int& y1, const int& x2, const int& y2)
{

    SetBoxByPos(cutBox, x1,y1, x2,y2);
    if (cutBox.x0 < 0) {
        auto boxWidth = cutBox.x1 - cutBox.x0;
        cutBox.x0 = 0;
        cutBox.x1 = boxWidth;
    }
    if (cutBox.x1 > w) {
        auto boxWidth = cutBox.x1 - cutBox.x0;
        cutBox.x1 = w;
        cutBox.x0 = cutBox.x1 - boxWidth;
    }
    if (cutBox.y0 < 0) {
        auto boxHeight = cutBox.y1 - cutBox.y0;
        cutBox.y0 = 0;
        cutBox.y1 = boxHeight;
    }
    if (cutBox.y1 > h) {
        auto boxHeight = cutBox.y1 - cutBox.y0;
        cutBox.y1 = h;
        cutBox.y0 = cutBox.y1 - boxHeight;
    }

    maskBoxes[0].x0 = 0;
    maskBoxes[0].x1 = cutBox.x0;
    maskBoxes[1].x0 = cutBox.x0;
    maskBoxes[1].x1 = cutBox.x1;
    maskBoxes[2].x0 = cutBox.x1;
    maskBoxes[2].x1 = w;
    maskBoxes[3].x0 = cutBox.x1;
    maskBoxes[3].x1 = w;
    maskBoxes[4].x0 = cutBox.x1;
    maskBoxes[4].x1 = w;
    maskBoxes[5].x0 = cutBox.x0;
    maskBoxes[5].x1 = cutBox.x1;
    maskBoxes[6].x0 = 0;
    maskBoxes[6].x1 = cutBox.x0;
    maskBoxes[7].x0 = 0;
    maskBoxes[7].x1 = cutBox.x0;

    maskBoxes[0].y0 = 0;
    maskBoxes[0].y1 = cutBox.y0;
    maskBoxes[1].y0 = 0;
    maskBoxes[1].y1 = cutBox.y0;
    maskBoxes[2].y0 = 0;
    maskBoxes[2].y1 = cutBox.y0;
    maskBoxes[3].y0 = cutBox.y0;
    maskBoxes[3].y1 = cutBox.y1;
    maskBoxes[4].y0 = cutBox.y1;
    maskBoxes[4].y1 = h;
    maskBoxes[5].y0 = cutBox.y1;
    maskBoxes[5].y1 = h;
    maskBoxes[6].y0 = cutBox.y1;
    maskBoxes[6].y1 = h;
    maskBoxes[7].y0 = cutBox.y0;
    maskBoxes[7].y1 = cutBox.y1;

    Refresh();
}


void MainWin::SetCutBox(const int& x, const int& y)
{
    switch (mouseInMaskBoxIndex)
    {
        case 0: {            
            dragStartCutBoxStartPos.x = x;
            dragStartCutBoxStartPos.y = y;
            break;
        }
        case 1: {
            dragStartCutBoxStartPos.y = y;
            break;
        }
        case 2: {
            dragStartCutBoxStartPos.y = y;
            dragStartCutBoxEndPos.x = x;
            break;
        }
        case 3: {
            dragStartCutBoxEndPos.x = x;
            break;
        }
        case 4: {
            dragStartCutBoxEndPos.x = x;
            dragStartCutBoxEndPos.y = y;
            break;
        }
        case 5: {
            dragStartCutBoxEndPos.y = y;
            break;
        }
        case 6: {
            dragStartCutBoxEndPos.y = y;
            dragStartCutBoxStartPos.x = x;
            break;
        }
        case 7: {
            dragStartCutBoxStartPos.x = x;
            break;
        }
        case 8: {
            auto xSpan = x - MouseDownPos.x;
            auto ySpan = y - MouseDownPos.y;
            dragStartCutBoxStartPos.x += xSpan;
            dragStartCutBoxStartPos.y += ySpan;
            dragStartCutBoxEndPos.x += xSpan;
            dragStartCutBoxEndPos.y += ySpan;
            MouseDownPos = POINT(x,y);
        }
    }
    SetCutBox(dragStartCutBoxStartPos.x, dragStartCutBoxStartPos.y, dragStartCutBoxEndPos.x, dragStartCutBoxEndPos.y);
}