#pragma once

#include <QMouseEvent>
#include <QPainter>
#include <QObject>
#include "ShapeBase.h"

class ShapeMosaic:public ShapeBase
{
    Q_OBJECT
public:
    ShapeMosaic(QObject* parent = nullptr, const QString&& btnName = "rectFill");
    virtual ~ShapeMosaic();
    virtual void paint(QPainter* painter) override;
    void paintDragger(QPainter* painter) override;
    void mouseMove(QMouseEvent* event) override;
    void mouseDrag(QMouseEvent* event) override;
    void mousePress(QMouseEvent* event) override;
    void mouseRelease(QMouseEvent* event) override;
    virtual void mouseOnShape(QMouseEvent* event);
public:
    QRect shape;
    bool isFill{ false };
    int strokeWidth{ 2 };
    QColor color{ Qt::red };
protected:
private:
    void resetDragger();
private:
    QPoint topLeft, rightBottom,pressPos;
};