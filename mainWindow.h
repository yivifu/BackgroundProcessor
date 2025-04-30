#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRadioButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include "ImgView.h"
#include <QVector>
#include <QDialogButtonBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QGraphicsBlurEffect>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    //Ui::MainWindow* ui;

    QWidget* centralWidget;

    // ImageDisplay* imageDisplay;
    ImgView* imageDisplay;
    QRadioButton* rgbMode;
    QRadioButton* grayMode;
    QLineEdit* minR;
    QLineEdit* maxR;
    QLineEdit* minG;
    QLineEdit* maxG;
    QLineEdit* minB;
    QLineEdit* maxB;
    QLineEdit* minGray;
    QLineEdit* maxGray;
    QPushButton* btnLoadImg;
    QPushButton* btnClearBg;
    QPushButton* btnChangeBg;
    QPushButton* btnSaveResult;
    QPushButton* btnShowOrigImg;
    QPushButton* btnMarkFrontMode;
    QPushButton* btnMarkBgMode;
    QPushButton* btnGetPixelMode;
    QPushButton* btnShowResultImg;
    QVector<QWidget*> controlsToDisable;
    QImage origImg;
    QImage processedImg;
    bool cleaned;

    void disableControls();
	QImage getAlphaChannel(const QImage& image);  // 获取 alpha 通道
	// 抗锯齿融合
	QImage antialiasedBlend(const QImage& foreground, const QImage& background, int blurRadius);
	


private slots:
    void loadImg(QImage image);
    void enterMarkMode(bool isFront);
    void enterColorInfoMode();
    void validateInput();
    void clearImgBg();
    void changeImgBg();
    void showOrigImg();
    void showResultImg();
    void saveResult();

    void enableControls();
};

class ClickedClearLineEdit : public QLineEdit {
	Q_OBJECT

public:
	ClickedClearLineEdit(QWidget* parent = nullptr) : QLineEdit(parent) {}

protected:
	// QLineEdit不触发单纯的鼠标点击信号，在鼠标按下事件中添加清空输入文本的功能
	void mousePressEvent(QMouseEvent* event) override {
		QLineEdit::mouseReleaseEvent(event);
		if (event->button() == Qt::LeftButton && !isReadOnly()) {
			clear();
		}
	}
};

#endif // MAINWINDOW_H
