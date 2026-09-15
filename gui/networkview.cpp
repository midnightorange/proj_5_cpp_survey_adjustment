#include "networkview.h"

#include <QColor>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QPainter>
#include <QPen>
#include <QPointF>
#include <QResizeEvent>
#include <QString>
#include <cmath>

NetworkView::NetworkView(QWidget *parent) : QGraphicsView(parent)
{
    setScene(new QGraphicsScene(this));
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::ScrollHandDrag);
}

void NetworkView::setNetwork(const geo::LevelingNetwork &net)
{
    net_ = net;
    hasHeights_ = false;
    heights_.clear();
    relayout();
}

void NetworkView::setAdjustedHeights(const std::vector<double> &heights)
{
    heights_ = heights;
    hasHeights_ = true;
    relayout();
}

void NetworkView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    if (!net_.points.empty())
    {
        refit();
    }
}

void NetworkView::refit()
{
    if (scene() == nullptr)
    {
        return;
    }
    QRectF r = scene()->itemsBoundingRect();
    if (r.isEmpty())
    {
        return;
    }
    fitInView(r.adjusted(-40, -40, 40, 40), Qt::KeepAspectRatio);
}

void NetworkView::relayout()
{
    scene()->clear();
    const int n = static_cast<int>(net_.points.size());
    if (n == 0)
    {
        return;
    }

    // 环形布局（水准网无平面坐标，按点顺序均匀分布）
    const double radius = 240.0;
    std::vector<QPointF> pos(n);
    for (int i = 0; i < n; ++i)
    {
        double ang = 2.0 * M_PI * i / n - M_PI / 2.0;
        pos[i] = QPointF(radius * std::cos(ang), radius * std::sin(ang));
    }

    // 观测线（先画，避免覆盖点）
    for (const auto &o : net_.obs)
    {
        QGraphicsLineItem *line = scene()->addLine(
            pos[o.from].x(), pos[o.from].y(), pos[o.to].x(), pos[o.to].y());
        line->setPen(QPen(QColor("#808080")));
        QPointF mid = (pos[o.from] + pos[o.to]) / 2.0;
        QGraphicsTextItem *txt = scene()->addText(
            QString("%1 m / %2 km").arg(o.dh).arg(o.lengthKm));
        txt->setPos(mid);
        txt->setDefaultTextColor(QColor("#606060"));
        txt->setScale(0.7);
    }

    // 点
    int ui = 0;
    for (int i = 0; i < n; ++i)
    {
        const auto &p = net_.points[i];
        QGraphicsEllipseItem *e = scene()->addEllipse(
            pos[i].x() - 9, pos[i].y() - 9, 18, 18);
        e->setBrush(p.known ? QColor("#558ED5") : QColor("#ED7D31"));
        e->setPen(QPen(Qt::black));

        QString label = QString::fromStdString(p.name);
        if (!p.known && hasHeights_ && ui < static_cast<int>(heights_.size()))
        {
            label += QString("\n%1 m").arg(heights_[ui], 0, 'f', 4);
        }
        QGraphicsTextItem *name = scene()->addText(label);
        name->setPos(pos[i].x() + 14, pos[i].y() - 30);
        name->setDefaultTextColor(Qt::black);
        name->setScale(0.9);

        if (!p.known)
        {
            ++ui;
        }
    }

    refit();
}
