#include "mainwindow.h"

#include <QAction>
#include <QFile>
#include <QFileDialog>
#include <QHeaderView>
#include <QLabel>
#include <QList>
#include <QSplitter>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QString>
#include <QStringConverter>
#include <QTabWidget>
#include <QTableView>
#include <QTextStream>
#include <QToolBar>

#include <exception>
#include <fstream>

#include "networkview.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("水准网平差 - 可视化");

    // 工具栏
    QToolBar *bar = addToolBar("main");
    bar->setMovable(false);
    QAction *actOpen = bar->addAction("打开数据");
    QAction *actRun = bar->addAction("运行平差");
    QAction *actExport = bar->addAction("导出报告");
    connect(actOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(actRun, &QAction::triggered, this, &MainWindow::runAdjust);
    connect(actExport, &QAction::triggered, this, &MainWindow::exportReport);

    // 左侧网图 + 右侧结果表格
    view_ = new NetworkView(this);
    pointModel_ = new QStandardItemModel(this);
    obsModel_ = new QStandardItemModel(this);
    pointTable_ = new QTableView(this);
    obsTable_ = new QTableView(this);
    pointTable_->setModel(pointModel_);
    obsTable_->setModel(obsModel_);
    pointTable_->horizontalHeader()->setStretchLastSection(true);
    obsTable_->horizontalHeader()->setStretchLastSection(true);

    QTabWidget *tabs = new QTabWidget(this);
    tabs->addTab(pointTable_, "待定点");
    tabs->addTab(obsTable_, "观测值");

    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(view_);
    splitter->addWidget(tabs);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({550, 550}); // 初始各占一半
    setCentralWidget(splitter);

    status_ = new QLabel("请打开标准水准网数据文件", this);
    statusBar()->addWidget(status_);
}

void MainWindow::openFile()
{
    QString path = QFileDialog::getOpenFileName(
        this, "打开水准网数据", "", "文本文件 (*.txt *.dat);;所有文件 (*)");
    if (path.isEmpty())
    {
        return;
    }
    std::ifstream fin(path.toStdString());
    if (!fin)
    {
        status_->setText("打开失败：" + path);
        return;
    }
    try
    {
        net_ = geo::parseLeveling(fin);
    }
    catch (const std::exception &e)
    {
        status_->setText(QString("解析失败：%1").arg(e.what()));
        return;
    }
    hasNet_ = true;
    hasResult_ = false;
    view_->setNetwork(net_);
    pointModel_->clear();
    obsModel_->clear();
    status_->setText(QString("已加载 %1，观测数 %2").arg(path).arg(net_.obs.size()));
}

void MainWindow::runAdjust()
{
    if (!hasNet_)
    {
        status_->setText("请先打开数据文件");
        return;
    }
    try
    {
        result_ = geo::adjustLeveling(net_);
    }
    catch (const std::exception &e)
    {
        status_->setText(QString("平差失败：%1").arg(e.what()));
        return;
    }
    hasResult_ = true;
    fillTables(result_);
    view_->setAdjustedHeights(result_.heights);
    status_->setText(QString("单位权中误差（1 km）：%1 m").arg(result_.sigma0, 0, 'f', 4));
}

void MainWindow::exportReport()
{
    if (!hasResult_)
    {
        status_->setText("请先运行平差");
        return;
    }
    QString path = QFileDialog::getSaveFileName(this, "导出报告", "report.txt", "文本文件 (*.txt)");
    if (path.isEmpty())
    {
        return;
    }
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        status_->setText("导出失败：" + path);
        return;
    }
    QTextStream out(&f);
    out.setEncoding(QStringConverter::Utf8);
    out << "==== 水准网间接平差结果 ====\n";
    std::size_t ui = 0;
    for (const auto &p : net_.points)
    {
        if (p.known)
        {
            continue;
        }
        out << QString("%1  %2 m  ±%3 m\n")
                   .arg(QString::fromStdString(p.name))
                   .arg(result_.heights[ui], 0, 'f', 4)
                   .arg(result_.sigmaH[ui], 0, 'f', 4);
        ++ui;
    }
    out << QString("单位权中误差（1 km）：%1 m\n").arg(result_.sigma0, 0, 'f', 4);
    f.close();
    status_->setText("已导出：" + path);
}

void MainWindow::fillTables(const geo::AdjustmentResult &r)
{
    // 待定点
    pointModel_->clear();
    pointModel_->setHorizontalHeaderLabels({"点名", "高程(m)", "中误差(m)"});
    std::size_t ui = 0;
    for (const auto &p : net_.points)
    {
        if (p.known)
        {
            continue;
        }
        QList<QStandardItem *> row;
        row << new QStandardItem(QString::fromStdString(p.name))
            << new QStandardItem(QString::number(r.heights[ui], 'f', 4))
            << new QStandardItem(QString::number(r.sigmaH[ui], 'f', 4));
        pointModel_->appendRow(row);
        ++ui;
    }

    // 观测值
    obsModel_->clear();
    obsModel_->setHorizontalHeaderLabels({"起点", "终点", "观测高差(m)", "改正数(m)", "路线(km)"});
    for (std::size_t k = 0; k < net_.obs.size(); ++k)
    {
        const auto &o = net_.obs[k];
        QList<QStandardItem *> row;
        row << new QStandardItem(QString::fromStdString(net_.points[o.from].name))
            << new QStandardItem(QString::fromStdString(net_.points[o.to].name))
            << new QStandardItem(QString::number(o.dh, 'f', 4))
            << new QStandardItem(QString::number(r.residuals[k], 'f', 4))
            << new QStandardItem(QString::number(o.lengthKm, 'f', 2));
        obsModel_->appendRow(row);
    }
}
