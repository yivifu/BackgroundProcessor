#include "mainwindow.h"

#include <QApplication>
#include <QScreen>
#include <QRect>
#include <QFontDatabase>
#include <QIcon>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    // 设置应用程序窗体图标
    QIcon appIcon(":/icons/bgremover_icon.png"); 
    app.setWindowIcon(appIcon);
    // 加载资源文件中的字体
    int fontId = QFontDatabase::addApplicationFont(":/fonts/霞鹜文楷_subset.ttf");
    if (fontId == -1) {
        qWarning() << "Failed to load font file!";
    }
    else {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (!fontFamilies.isEmpty()) {
            // 设置全局字体
            QFont font(fontFamilies.at(0), 11); // 字体族名称和大小
            QApplication::setFont(font);
        }
    }
    MainWindow w;
    // 对于构造函数中未设置窗体尺寸的窗体，可以先show再用qDebug()输出数据，以供设置窗体尺寸的参考
    // w.show();
    // qDebug() << "width:" <<w.width() << ",heght:" << w.height() ;
    // 设置窗体尺寸，不设置将无法使窗体居中
    w.resize(1180, 618);
    // 获取屏幕的几何信息
    QScreen* screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();

    // 计算窗口水平居中的位置
    int x = (screenGeometry.width() - w.width()) / 2;
    int y = 100; // 距离屏幕顶部20像素

    // 设置窗口位置
    w.move(x, y);
	// 设置窗口标题
	w.setWindowTitle("Background Processor");
    // 居中显示窗口,先显示再移动位置将导致屏幕上显示窗口移动过程
    w.show();
    return app.exec();
}
