#include "ShapeText.h"
#include "../WindowMain.h"
#include "../ToolSub.h"
#include "../AppFont.h"
#include <locale>
#include <codecvt>
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRect.h"
#include "include/core/SkSurface.h"
#include "include/core/SkString.h"
#include "include/utils/SkTextUtils.h"
#include "include/core/SkFontMetrics.h"

ShapeText::ShapeText(const int &x, const int &y) : ShapeBase(x, y)
{
    initParams();
}

ShapeText::~ShapeText()
{
}

bool ShapeText::OnMouseDown(const int &x, const int &y)
{
    IsWIP = false;
    auto font = AppFont::Get()->fontText;
    font->setSize(fontSize);
    SkFontMetrics metrics;
    font->getMetrics(&metrics);
    auto lineHeight = metrics.fBottom - metrics.fTop;
    SkRect lineRect;
    float left{ (float)x-10 }, top{ (float)y-10 }, width{ 20 }, height{ 0 };
    for (size_t i = 0; i < lines.size(); i++)
    {
        auto data = lines[i].data();
        auto length = wcslen(data) * 2;
        font->measureText(data, length, SkTextEncoding::kUTF16, &lineRect);
        width = std::max(width, lineRect.width());
        height += lineHeight;
        if (i == 0) {
            top += lineRect.top();
        }
    }
    if (width > 20) {
        width += 20;
    }
    if (height == 0) {
        height = lineHeight;
    }
    rect.setXYWH(left, top, width, height);
    activeKeyboard(x, y+lineHeight-10);
    WindowMain::get()->Refresh();
    return false;
}

bool ShapeText::OnMouseMove(const int &x, const int &y)
{
    return false;
}

bool ShapeText::OnMouseUp(const int &x, const int &y)
{
    return false;
}

bool ShapeText::OnMoseDrag(const int &x, const int &y)
{
    IsWIP = false;
    WindowMain::get()->Refresh();
    return false;
}

bool ShapeText::OnPaint(SkCanvas *canvas)
{
    SkPaint paint;
    paint.setColor(color);
    paint.setStroke(true);
    paint.setStrokeWidth(2);
    canvas->drawRect(rect, paint);
    auto font = AppFont::Get()->fontText;
    SkFontMetrics metrics;
    font->getMetrics(&metrics);
    font->setSize(fontSize); 
    paint.setStroke(false);
    SkRect textBounds;
    auto height = metrics.fBottom - metrics.fTop;
    paint.setAntiAlias(true);
    float x{(float)startX}, y{ (float)startY };
    for (const auto& line : lines) {
        auto data = line.data();
        auto length = wcslen(data) * 2;
        SkDebugf(std::to_string(length).c_str());
        SkDebugf("\n");
        SkTextUtils::Draw(canvas, data, length,SkTextEncoding::kUTF16,x, y, *font, paint,SkTextUtils::kLeft_Align);
        y += height;
    }
    if (!ShowCursor) {
        return false;
    }

    auto lineHeight = metrics.fBottom - metrics.fTop;
    float inputCursorX{ rect.left() + 10 }, inputCursorTop{ rect.top() + lineHeight * lineIndex + 10 };
    float inputCursorBottom{ rect.top() + lineHeight * (lineIndex + 1) - 10 };
    if (wordIndex > 0) {
        SkRect lineRect;
        auto subStr = lines[lineIndex].substr(0, wordIndex);
        auto data = subStr.data();
        auto length = wcslen(data) * 2;
        font->measureText(data, length, SkTextEncoding::kUTF16, &lineRect);
        inputCursorX += lineRect.width();
    }
    canvas->drawLine(inputCursorX, inputCursorTop, inputCursorX, inputCursorBottom, paint);
    //font->measureText(str.c_str(), str.size(), SkTextEncoding::kUTF8, &textBounds);
    //SkScalar x = startX - textBounds.width() / 2 - textBounds.left();
    //SkScalar y = startY + textBounds.height() / 2 - textBounds.bottom();    
    //canvas->drawSimpleText(str.c_str(), str.size(), SkTextEncoding::kUTF8, x, y, *font, paint);

    //paint.setStroke(false);
    //auto data = text.data();
    //auto length = wcslen(data) * 2;
    //auto font = AppFont::Get()->fontText;
    //font->setSize(fontSize);


    //std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    //// 创建多行宽字符文本
    //std::wstring text = L"This is a multi-line text.\nSecond line.";
    //// 分割文本为多行
    //std::vector<SkString> lines;
    //std::wstring::size_type pos = 0;
    //while ((pos = text.find(L'\n', pos)) != std::wstring::npos) {
    //    auto str = converter.to_bytes(text.substr(0, pos));
    //    lines.push_back(SkString(str.c_str()));
    //    text.erase(0, pos + 1);
    //}
    //auto str = converter.to_bytes(text.substr(0, pos));
    //lines.push_back(SkString(str.c_str()));
    //SkFontMetrics metrics;
    //font->getMetrics(&metrics);
    //auto height = metrics.fBottom - metrics.fTop;
    //for (const auto& line : lines) {
    //    SkTextUtils::DrawString(canvas, line.c_str(), startX, startY, *font, paint,SkTextUtils::kLeft_Align);
    //    startY += height; // 下一行的位置在当前行下方加上文本大小
    //}
    return false;
}

void ShapeText::initParams()
{

    auto tool = ToolSub::get();
    color = tool->getColor();
}
void ShapeText::activeKeyboard(long x, long y)
{
    auto win = WindowMain::get();
    if (HIMC himc = ImmGetContext(win->hwnd))
    {
        COMPOSITIONFORM comp = {};
        comp.ptCurrentPos.x = x;
        comp.ptCurrentPos.y = y;
        comp.dwStyle = CFS_FORCE_POSITION;
        ImmSetCompositionWindow(himc, &comp);
        CANDIDATEFORM cand = {};
        cand.dwStyle = CFS_CANDIDATEPOS;
        cand.ptCurrentPos.x = x;
        cand.ptCurrentPos.y = y;
        ImmSetCandidateWindow(himc, &cand);
        ImmReleaseContext(win->hwnd, himc);
    }
}
void ShapeText::InsertWord(const std::wstring& word)
{
    //text = text.substr(0, cursorIndex) + word + text.substr(cursorIndex);
    //cursorIndex += 1;
    WindowMain::get()->Refresh();
}