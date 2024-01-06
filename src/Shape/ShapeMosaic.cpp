#include "ShapeMosaic.h"
#include <memory>
#include "../App.h"
#include "../WindowBase.h"
#include "../ToolSub.h"
#include "ShapeDragger.h"
#include "include/effects/Sk2DPathEffect.h"
#include "include/core/SkBlurTypes.h"
#include "include/effects/SkBlurMaskFilter.h"
#include "include/core/SkPathMeasure.h"
#include "include/core/SkColor.h"
#include "include/core/SkStream.h"
#include "include/encode/SkPngEncoder.h"

ShapeMosaic::ShapeMosaic(const int &x, const int &y) : ShapeBase(x, y)
{    
    isWip = false;
    initParams();
}

ShapeMosaic::~ShapeMosaic()
{
}

void ShapeMosaic::Paint(SkCanvas *canvas)
{
    SkPaint paint;
    if (pixmap) {        
        SkPathMeasure pathMeasure(path, false);
        float length = pathMeasure.getLength();
        SkPoint point;
        for (float distance = 0; distance < length; distance += size) {
            if (pathMeasure.getPosTan(distance, &point, nullptr)) {
                drawRectsByPoints(point, canvas);
            }
        }
    }
    else {
        canvas->saveLayer(nullptr, nullptr);
        auto win = App::GetWin();
        int rowNum = std::ceil((float)win->w / size);
        for (const auto& kv : colorCache)
        {
            paint.setColor(kv.second);
            int yIndex = (float)kv.first / (float)rowNum;
            auto xIndex = kv.first % rowNum;
            canvas->drawRect(SkRect::MakeXYWH(xIndex * size, yIndex * size, size, size), paint);
        }
    }
    paint.setAntiAlias(true);
    paint.setStroke(true);
    paint.setStrokeWidth(strokeWidth);
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
    paint.setStrokeJoin(SkPaint::kRound_Join);  
    paint.setBlendMode(SkBlendMode::kClear);
    path.setFillType(SkPathFillType::kInverseWinding);
    canvas->drawPath(path, paint);
    if (!pixmap) {
        canvas->restore();
    }
}

bool ShapeMosaic::OnMouseDown(const int &x, const int &y)
{
    auto win = App::GetWin();
    auto surfaceFront = win->surfaceFront;
    surfaceFront->writePixels(*win->pixSrc, 0, 0);
    auto canvas = win->surfaceFront->getCanvas();
    canvas->drawImage(win->surfaceBack->makeImageSnapshot(), 0, 0);
    SkImageInfo info = SkImageInfo::MakeN32Premul(win->w, win->h);
    auto addr = new unsigned char[win->w * win->h * 4];
    pixmap = new SkPixmap(info, addr, win->w * 4);
    surfaceFront->readPixels(*pixmap,0,0);
    canvas->clear(SK_ColorTRANSPARENT);
    path.moveTo(x, y);
    return false;
}

bool ShapeMosaic::OnMouseUp(const int &x, const int &y)
{
    delete[] pixmap->addr();
    delete pixmap;
    pixmap = nullptr;
    ShapeDragger::get()->disableDragger();
    return false;
}

bool ShapeMosaic::OnMouseMove(const int &x, const int &y)
{
    return false;
}

bool ShapeMosaic::OnMoseDrag(const int &x, const int &y)
{
    path.lineTo(x, y);
    auto win = App::GetWin();
    auto canvas = win->surfaceFront->getCanvas();
    canvas->clear(SK_ColorTRANSPARENT);
    Paint(canvas);
    win->Refresh();
    return false;
}

void ShapeMosaic::drawRectsByPoints(const SkPoint& point, SkCanvas* canvas)
{
    auto win = App::GetWin();
    int rowNum = std::ceil((float)win->w / size);
    int rectNum = std::ceil(strokeWidth*2 / size);
    int xIndex = (point.fX - strokeWidth) / size;
    int yIndex = (point.fY - strokeWidth) / size;
    SkColor4f colorSum = { 0, 0, 0, 0 };
    SkPaint paint;
    for (size_t i = yIndex; i < yIndex+rectNum; i++)
    {
        for (size_t j = xIndex; j < xIndex+rectNum; j++)
        {
            int key = i*rowNum+j;
            auto x = j * size;
            auto y = i * size;
            if (colorCache.contains(key)) {
                paint.setColor(colorCache[key]);
                canvas->drawRect(SkRect::MakeXYWH(x, y, size, size),paint);
            }
            else {
                int count{ 0 };
                for (size_t x1 = x; x1 <= x+size; x1 += 2)
                {
                    for (size_t y1 = y; y1 <= y+size; y1 += 2)
                    {
                        auto currentColor = pixmap->getColor4f(x1, y1);
                        colorSum.fR += currentColor.fR;
                        colorSum.fG += currentColor.fG;
                        colorSum.fB += currentColor.fB;
                        count++;
                    }
                }
                colorSum.fR /= count;
                colorSum.fG /= count;
                colorSum.fB /= count;
                colorSum.fA = 255;
                auto color = colorSum.toSkColor();
                paint.setColor(color);
                canvas->drawRect(SkRect::MakeXYWH(x, y, size, size), paint);
                colorCache.insert({ key, color });
            }
        }
    }

}

void ShapeMosaic::initParams()
{
    HoverIndex = 4;
    path.moveTo(startX, startY);
    auto tool = ToolSub::get();
    stroke = !tool->getFill();
    if (stroke)
    {
        auto stroke = tool->getStroke();
        if (stroke == 1)
        {
            strokeWidth = 36;
        }
        else if (stroke == 2)
        {
            strokeWidth = 66;
        }
        else
        {
            strokeWidth = 96;
        }
    }
}