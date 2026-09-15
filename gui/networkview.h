#pragma once
#include <QGraphicsView>
#include <vector>

#include "leveling.h"

class QResizeEvent;

// 控制网网图视图（环形布局，随窗口自适应缩放）
class NetworkView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit NetworkView(QWidget *parent = nullptr);

    void setNetwork(const geo::LevelingNetwork &net);
    void setAdjustedHeights(const std::vector<double> &heights); // 待定点平差高程

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void relayout();
    void refit(); // 按当前视图尺寸重新缩放

    geo::LevelingNetwork net_;
    std::vector<double> heights_;
    bool hasHeights_ = false;
};
