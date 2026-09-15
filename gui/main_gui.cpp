#include <QApplication>
#include <QMessageLogContext>
#include <QString>

#include "mainwindow.h"

static QtMessageHandler g_previousHandler = nullptr;

// 屏蔽 libpng 的无害 iCCP 色彩配置警告，其余消息原样输出
static void filteredMessageHandler(QtMsgType type, const QMessageLogContext &context,
                                   const QString &msg)
{
    if (msg.contains(QStringLiteral("iCCP")))
    {
        return;
    }
    if (g_previousHandler)
    {
        g_previousHandler(type, context, msg);
    }
}

int main(int argc, char **argv)
{
    g_previousHandler = qInstallMessageHandler(filteredMessageHandler);

    QApplication app(argc, argv);
    MainWindow w;
    w.resize(1100, 700);
    w.show();
    return app.exec();
}
