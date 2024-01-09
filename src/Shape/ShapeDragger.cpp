#include "ShapeDragger.h"
#include "../App.h"
#include "../WindowBase.h"
#include "../Timer.h"
#include "../Recorder.h"
#include "ShapeBase.h"

ShapeDragger* shapeDragger;
ShapeDragger::ShapeDragger()
{
    for (size_t i = 0; i < 8; i++)
    {
        draggers.push_back(SkRect::MakeEmpty());
        Cursors.push_back(Cursor::cursor::all);
    }
}

ShapeDragger::~ShapeDragger()
{
}

void ShapeDragger::Init()
{
    shapeDragger = new ShapeDragger();
}

ShapeDragger* ShapeDragger::Get()
{
    return shapeDragger;
}

void ShapeDragger::SetDragger(size_t index, float x, float y)
{
    draggers[index].setXYWH(x, y, Size, Size);
}

void ShapeDragger::ShowDragger(SkCanvas* canvas)
{   
    SkPaint paint;
    paint.setStroke(true);
    paint.setStrokeWidth(1);
    paint.setColor(SK_ColorBLACK);
    for (auto& dragger : draggers) {
        canvas->drawRect(dragger, paint);
    }
    visible = true;
}

bool ShapeDragger::HideDragger()
{
    auto recorder = Recorder::Get();
    if (recorder->CurShape) {
        return false;
    }
    auto win = App::GetWin();
    auto canvas = win->surfaceFront->getCanvas();
    canvas->clear(SK_ColorTRANSPARENT);
    win->refresh();
    visible = false;
    CurShape = nullptr;
    return true;
}

void ShapeDragger::DisableDragger(const int& startIndex)
{
    for (size_t i = startIndex; i < draggers.size(); i++)
    {
        draggers[i].setXYWH(-100, -100, Size, Size);
    }
}

int ShapeDragger::IndexMouseAt(const int& x, const int& y)
{
    if (!visible) {
        return -1;
    }
    for (size_t i = 0; i < draggers.size(); i++)
    {
        if (draggers[i].contains(x, y)) {
            Cursor::SetCursor(Cursors[i]);
            CurShape->HoverIndex = i;
            return i;
        }
    }
    return -1;
}


