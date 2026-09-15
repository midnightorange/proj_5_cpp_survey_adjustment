#pragma once
#include <QMainWindow>

#include "leveling.h"

class QLabel;
class QStandardItemModel;
class QTableView;
class NetworkView;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void openFile();
    void runAdjust();
    void exportReport();

private:
    void fillTables(const geo::AdjustmentResult& r);

    NetworkView* view_ = nullptr;
    QTableView* pointTable_ = nullptr;
    QTableView* obsTable_ = nullptr;
    QStandardItemModel* pointModel_ = nullptr;
    QStandardItemModel* obsModel_ = nullptr;
    QLabel* status_ = nullptr;

    geo::LevelingNetwork net_;
    geo::AdjustmentResult result_;
    bool hasNet_ = false;
    bool hasResult_ = false;
};
