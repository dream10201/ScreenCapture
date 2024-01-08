#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "include/core/SkPath.h"
#include "State.h"
#include <vector>
#include "Timer.h"

class ShapeBase;
class Recorder
{
public:
    ~Recorder();
    static void init();
    static Recorder *get();
    bool OnMouseDown(const int &x, const int &y);
    bool OnMouseDownRight(const int& x, const int& y);
    bool OnMouseUp(const int &x, const int &y);
    bool OnMouseMove(const int &x, const int &y);
    bool OnMouseDrag(const int& x, const int& y);
    bool onChar(const unsigned int& val);
    bool onKeyDown(const unsigned int& val);
    bool onMouseWheel(const int& delta);
    void undo();
    void redo();
    void FinishPaint();
    std::vector<std::shared_ptr<ShapeBase>> shapes;
    ShapeBase* curShape{ nullptr };
private:
    //bool hideDragger(const int& id);
    //bool flashTextCursor(const int& id);
    Recorder();
    void createShape(const int& x, const int& y,const State& state);
    
};