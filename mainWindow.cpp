#include "mainwindow.h"
#include "constants.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QImage>
#include <Qt>
#include <QString>
#include <QPushButton>
#include <QLineEdit>
#include <QRadioButton>
#include <QCheckBox>
#include <QLabel>
#include <QIntValidator>
#include <QMessageBox>
#include <QFontMetrics>
#include <QFileDialog>
#include <QColorDialog>
#include <QColor>
#include <QGraphicsBlurEffect>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    centralWidget = new QWidget(this); // 窗体中央部件
    setCentralWidget(centralWidget);

    imageDisplay = new ImgView(centralWidget);
    QHBoxLayout* main_layout = new QHBoxLayout(centralWidget); // 直接设置 main_layout 为 centralWidget 的布局

    QVBoxLayout* rightColumnLayout = new QVBoxLayout; // 不要设置为已设置布局的控件的布局，否则出现重复设置布局的消息
    // 1. 说明标签    
    QLabel* commentLabel = new QLabel(centralWidget);
    commentLabel->setText(Constants::COMMENT);
    commentLabel->setObjectName("commentLabel");  // 供QSS选择器用
    commentLabel->setWordWrap(true);
    commentLabel->setFixedHeight(350);
    rightColumnLayout->addWidget(commentLabel);

    // 2. 载入图像按钮组
    QGroupBox* loadButtonGroup = new QGroupBox(centralWidget);
    QHBoxLayout* loadButtonLayout = new QHBoxLayout;
    loadButtonLayout->setSpacing(5);
    // 指定了父控件，尽管是类成员的指针变量，也无需在析构函数中主动delete
    btnLoadImg = new QPushButton("载入图像", centralWidget);
    // connect函数的参数：发出信号的对象，发出信号的类型，接收信号的对象，槽函数
	connect(btnLoadImg, &QPushButton::clicked, this, [this]() {
												loadImg(QImage()); });
    btnMarkFrontMode = new QPushButton("选取前景", centralWidget);
    btnMarkFrontMode->setObjectName("select-front");
    // 在 MainWindow 的构造函数或其他初始化代码中 (.cpp)
    connect(btnMarkFrontMode, &QPushButton::clicked, this, [this]() {
        //  lambda 表达式内部，可以调用槽函数并向其传递参数
        enterMarkMode(true);
        });

    btnMarkBgMode = new QPushButton("选取背景", centralWidget);
    btnMarkBgMode->setObjectName("select-bg");
    // 在 MainWindow 的构造函数或其他初始化代码中 (.cpp)
    connect(btnMarkBgMode, &QPushButton::clicked, this, [this]() {
        enterMarkMode(false);
        });

    btnGetPixelMode = new QPushButton("查看图像颜色", centralWidget);
    connect(btnGetPixelMode, &QPushButton::clicked, this, &MainWindow::enterColorInfoMode);

    btnSaveResult = new QPushButton("保存结果", centralWidget);
    connect(btnSaveResult, &QPushButton::clicked, this, &MainWindow::saveResult);

    loadButtonLayout->addWidget(btnLoadImg);
    loadButtonLayout->addWidget(btnGetPixelMode);
    loadButtonLayout->addWidget(btnMarkFrontMode);
    loadButtonLayout->addWidget(btnMarkBgMode);
    loadButtonLayout->addWidget(btnSaveResult);
    loadButtonGroup->setLayout(loadButtonLayout);
    rightColumnLayout->addWidget(loadButtonGroup);

    // 3. 图像背景颜色控件组
    QGroupBox* colorGroup = new QGroupBox("指定背景颜色信息", centralWidget);
    colorGroup->setObjectName("c_group");
    QLabel* lblMax;
    QLabel* lblMin;

    // 3.1. RGB模式控件组
    QGridLayout* colorLayout = new QGridLayout;

    QLabel* rgbRLabel = new QLabel("<b style='color:red;'>R（红色）分量:</b>", centralWidget);
    minR = new ClickedClearLineEdit(centralWidget);
    minR->setPlaceholderText("0~255");
    minR->setValidator(new QIntValidator(0, 255, centralWidget));
    connect(minR, &ClickedClearLineEdit::textChanged, this, &MainWindow::validateInput);
    maxR = new ClickedClearLineEdit(centralWidget);
    maxR->setPlaceholderText("0~255");
    maxR->setValidator(new QIntValidator(0, 255, this));
    connect(maxR, &ClickedClearLineEdit::textChanged, this, &MainWindow::validateInput);
    lblMax = new QLabel("最大值：", centralWidget);
    lblMin = new QLabel("最小值：", centralWidget);
    colorLayout->addWidget(rgbRLabel, 1, 0);
    colorLayout->addWidget(lblMin, 1, 1);
    colorLayout->addWidget(minR, 1, 2);
    colorLayout->addWidget(lblMax, 1, 3);
    colorLayout->addWidget(maxR, 1, 4);

    QLabel* rgbGLabel = new QLabel("<b style='color:green;'>G（绿色）分量:</b>", centralWidget);
    minG = new ClickedClearLineEdit(centralWidget);
    minG->setPlaceholderText("0~255");
    minG->setValidator(new QIntValidator(0, 255, this));
    connect(minG, &ClickedClearLineEdit::textChanged, this, &MainWindow::validateInput);
    maxG = new ClickedClearLineEdit(centralWidget);
    maxG->setPlaceholderText("0~255");
    maxG->setValidator(new QIntValidator(0, 255, this));
    connect(maxG, &ClickedClearLineEdit::textChanged, this, &MainWindow::validateInput);
    lblMax = new QLabel("最大值：", centralWidget);
    lblMin = new QLabel("最小值：", centralWidget);
    colorLayout->addWidget(rgbGLabel, 2, 0);
    colorLayout->addWidget(lblMin, 2, 1);
    colorLayout->addWidget(minG, 2, 2);
    colorLayout->addWidget(lblMax, 2, 3);
    colorLayout->addWidget(maxG, 2, 4);

    QLabel* rgbBLabel = new QLabel("<b style='color:blue;'>B（蓝色）分量:</b>", centralWidget);
    minB = new ClickedClearLineEdit(centralWidget);
    minB->setPlaceholderText("0~255");
    minB->setValidator(new QIntValidator(0, 255, centralWidget));
    connect(minB, &ClickedClearLineEdit::textChanged, this, &MainWindow::validateInput);
    maxB = new ClickedClearLineEdit(centralWidget);
    maxB->setPlaceholderText("0~255");
    maxB->setValidator(new QIntValidator(0, 255, this));
    connect(maxB, &ClickedClearLineEdit::textChanged, this, &MainWindow::validateInput);
    lblMax = new QLabel("最大值：", centralWidget);
    lblMin = new QLabel("最小值：", centralWidget);
    colorLayout->addWidget(rgbBLabel, 3, 0);
    colorLayout->addWidget(lblMin, 3, 1);
    colorLayout->addWidget(minB, 3, 2);
    colorLayout->addWidget(lblMax, 3, 3);
    colorLayout->addWidget(maxB, 3, 4);

    minGray = new ClickedClearLineEdit(centralWidget);
    minGray->setPlaceholderText("0~255");
    minGray->setValidator(new QIntValidator(0, 255, this));
    connect(minGray, &ClickedClearLineEdit::textChanged, this, &MainWindow::validateInput);
    maxGray = new ClickedClearLineEdit(centralWidget);
    maxGray->setPlaceholderText("0~255");
    maxGray->setValidator(new QIntValidator(0, 255, centralWidget));
    connect(maxGray, &ClickedClearLineEdit::textChanged, this, &MainWindow::validateInput);
    lblMax = new QLabel("最大值：", centralWidget);
    lblMin = new QLabel("最小值：", centralWidget);
    QLabel* rgbGrayLabel = new QLabel("<b>灰度：</b>", centralWidget);
    colorLayout->addWidget(rgbGrayLabel, 4, 0);
    colorLayout->addWidget(lblMin, 4, 1);
    colorLayout->addWidget(minGray, 4, 2);
    colorLayout->addWidget(lblMax, 4, 3);
    colorLayout->addWidget(maxGray, 4, 4);
    colorGroup->setLayout(colorLayout);
    rightColumnLayout->addWidget(colorGroup);

    // 4. 清除背景和保存结果按钮
    QGroupBox* clearSaveGroup = new QGroupBox(centralWidget);
    QHBoxLayout* clearSaveLayout = new QHBoxLayout;

    btnClearBg = new QPushButton("清除背景", centralWidget);
    connect(btnClearBg, &QPushButton::clicked, this, &MainWindow::clearImgBg);

    btnChangeBg = new QPushButton("更换背景", centralWidget);
    connect(btnChangeBg, &QPushButton::clicked, this, &MainWindow::changeImgBg);

    btnShowOrigImg = new QPushButton("原始图像", centralWidget);
    connect(btnShowOrigImg, &QPushButton::clicked, this, &MainWindow::showOrigImg);

    btnShowResultImg = new QPushButton("处理结果", centralWidget);
    connect(btnShowResultImg, &QPushButton::clicked, this, &MainWindow::showResultImg);

    clearSaveLayout->addWidget(btnClearBg);
    clearSaveLayout->addWidget(btnChangeBg);
    clearSaveLayout->addWidget(btnShowResultImg);
    clearSaveLayout->addWidget(btnShowOrigImg);

    clearSaveGroup->setLayout(clearSaveLayout);
    rightColumnLayout->addWidget(clearSaveGroup);
    // 设置控件centralWidget的样式表，也可以在程序入口处通过app.setStyleSheet(styleSheet)设置程序全局样式
    centralWidget->setStyleSheet(Constants::STYLESHEET);

    main_layout->addWidget(imageDisplay);
    main_layout->addLayout(rightColumnLayout);
    // 禁用窗体最大化按钮及改变窗体大小功能
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowMaximizeButtonHint);
    // 禁用子控件时不要使用findChild方法遍历子控件，否则可能导致控件禁用后无法激活。应显式设置需遍历的控件的数组
    controlsToDisable = {  // 进入或退出标记区域模式或查看颜色信息模式时要禁用或激活的控件
        btnLoadImg, /*rgbMode, grayMode,*/ minR,
        maxR, minG, maxG, minB, maxB, minGray,
        maxGray, btnClearBg, btnChangeBg,
        btnSaveResult, btnShowOrigImg, btnMarkBgMode,
        btnMarkFrontMode, btnGetPixelMode,btnShowResultImg
    };
    cleaned = false; // 初始化去背景标志
    // 连接 ImgView::enableControls 信号到 enableControls 槽
    connect(imageDisplay, &ImgView::enableControls, this, &MainWindow::enableControls);
	// 图像加载完成信号连接到槽函数。图像拖放操作成功，将载入的图像作为参数传递给槽函数
	connect(imageDisplay, &ImgView::loadedImage, this, [this](QImage img) {
												loadImg(img); });
}


void MainWindow::loadImg(QImage img) {
	if (img.isNull()) {
		QString fileName = QFileDialog::getOpenFileName(this, "打开图像", "", "图像文件 (*.png *.jpg *.bmp)");
		if (fileName.isEmpty()) {
			return;
		}

		origImg = QImage(fileName);
		if (origImg.isNull()) {
			QMessageBox::warning(this, "错误", "无法加载图像！");
			return;
		}
	} else {
		origImg = img; // 如果传入的图像不为空，则使用传入的图像
	}	
    imageDisplay->clearMarkedAreas(2);  // 前景背景区域全部清空
    imageDisplay->setImage(origImg, true);
    cleaned = false;
    processedImg = QImage();
    enableControls();
}


void MainWindow::enterMarkMode(bool isFront) {
    if (origImg.isNull()) {
        QMessageBox::warning(this, "警告", "请先载入图像");
    }
    else {
        // 根据isFront标记决定清除前景区域还是清除背景区域
        // 标记为前景区，清除前景区(分支标志值为0)，否则清除背景区(分支标志值为1)
        int area = isFront ? 0 : 1;
        imageDisplay->clearMarkedAreas(area);
        imageDisplay->enterMarkMode(isFront);
        disableControls();
    }
}

// 进入查看颜色信息模式槽函数
void MainWindow::enterColorInfoMode() {
    if (origImg.isNull()) {
        QMessageBox::warning(this, "警告", "请先载入图像");
    }
    else {
        imageDisplay->enterGetPixelInfoMode(true);
        disableControls();
    }
}
// 对输入不能通过验证器时的处理
void MainWindow::validateInput() {
    QLineEdit* sender = qobject_cast<QLineEdit*>(this->sender());
    QString text = sender->text();
    // 除0外其它数字不许以0开头
    if (!sender->hasAcceptableInput() || (text.startsWith("0") && text.length() > 1)) {
        sender->clear();
    }
}
// 清除图像背景槽函数
void MainWindow::clearImgBg() {
    if (origImg.isNull()) {
        QMessageBox::warning(this, "提示", "请先加载图像！");
        return;
    }
    // nobgImg不存在则创建一个用于处理，并确保支持 Alpha 通道(QImage::Format_ARGB32处理效率较高)
    if (processedImg.isNull()) {
        processedImg = origImg.convertToFormat(QImage::Format_ARGB32);
    }
    int width = processedImg.width();
    int height = processedImg.height();
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            ImgView::AreaType areaType = imageDisplay->getPointArea(x, y);
            // 1. 处理前景区和背景区
            if (areaType == ImgView::AreaType::Front || areaType == ImgView::AreaType::Both) {
                continue;  //前景区及前景与背景重叠区跳过
            }
            if (areaType == ImgView::AreaType::Background) {
                processedImg.setPixelColor(x, y, Qt::transparent); // 背景区设置为透明色
                continue;
            }

            // 2. 获取像素颜色
            QColor pixelColor = processedImg.pixelColor(x, y);
            // 3. 判断是否符合背景色判定条件
            bool isBackground = false;  // 像素是否为背景的标志
            QString sMinR = minR->text();
            QString sMinG = minG->text();
            QString sMinB = minB->text();
            QString sMaxR = maxR->text();
            QString sMaxG = maxG->text();
            QString sMaxB = maxB->text();
            QString sMaxGray = maxGray->text();
            QString sMinGray = minGray->text();
            int iMinR = sMinR == "" ? 0 : sMinR.toInt();
            int iMinG = sMinG == "" ? 0 : sMinG.toInt();
            int iMinB = sMinB == "" ? 0 : sMinB.toInt();
            int iMaxR = sMaxR == "" ? 255 : sMaxR.toInt();
            int iMaxG = sMaxG == "" ? 255 : sMaxG.toInt();
            int iMaxB = sMaxB == "" ? 255 : sMaxB.toInt();
            int iMinGray = sMinGray == "" ? 0 : sMinGray.toInt();
            int iMaxGray = sMaxGray == "" ? 255 : sMaxGray.toInt();
            int r = pixelColor.red();
            int g = pixelColor.green();
            int b = pixelColor.blue();
            int gray = qGray(pixelColor.rgb());
            bool bR = r >= iMinR && r <= iMaxR;
            bool bG = g >= iMinG && g <= iMaxG;
            bool bB = b >= iMinB && b <= iMaxB;
            bool bGray = gray >= iMinGray && gray <= iMaxGray;
            if (iMinR > iMaxR || iMinG > iMaxG || iMinB > iMaxB || iMinGray > iMaxGray) {
                QMessageBox::warning(this, "警告", "指定的灰度最小值超过了最大值，请予以检查。图像背景不会发生变化！");
                return;
            }
            isBackground = bR && bG && bB && bGray;
            // 4. 如果某像素的背景标志为真，则设置为透明
            if (isBackground) {
                processedImg.setPixelColor(x, y, Qt::transparent); // 设置为透明色
            }
        }
    }
    cleaned = true; // 标记已执行清除操作    
    imageDisplay->clearMarkedAreas(2);
    imageDisplay->setImage(processedImg);
}
// 更换背景槽函数
void MainWindow::changeImgBg() {
    if (!cleaned) {
        QMessageBox::warning(this, "警告", "请先清除背景");
        return;
    }

    // 1. 创建选择对话框，利用QT内置对话框
    QDialog choiceDialog(this);
    choiceDialog.setWindowTitle("选择背景类型");

    QVBoxLayout* dialogLayout = new QVBoxLayout(&choiceDialog);
    QLabel* promptLabel = new QLabel("请选择要使用的背景类型：", &choiceDialog);
    QRadioButton* rbSolidColor = new QRadioButton("使用纯色背景", &choiceDialog);
    QRadioButton* rbImageBackground = new QRadioButton("使用图像背景", &choiceDialog);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &choiceDialog);

    rbSolidColor->setChecked(true); // 默认选中纯色

    dialogLayout->addWidget(promptLabel);
    dialogLayout->addWidget(rbSolidColor);
    dialogLayout->addWidget(rbImageBackground);
    dialogLayout->addWidget(buttonBox);
    // 连接QT内置对话框的信号和槽函数
    connect(buttonBox, &QDialogButtonBox::accepted, &choiceDialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &choiceDialog, &QDialog::reject);

    // 2. 显示对话框并获取用户选择
    if (choiceDialog.exec() == QDialog::Accepted) {
		// 创建一副临时图像用于中间处理
		QImage bgImg(processedImg.size(), QImage::Format_ARGB32_Premultiplied);
		bgImg.fill(Qt::transparent); // 初始化为透明

		QPainter painter(&bgImg);
		painter.setRenderHint(QPainter::Antialiasing); // 开启抗锯齿
        if (rbSolidColor->isChecked()) {
            // --- 用户选择纯色背景 ---
            QColor color = QColorDialog::getColor(Qt::white, this, "选择背景颜色");
			painter.fillRect(bgImg.rect(), color); // 填充背景颜色
            
        } else if (rbImageBackground->isChecked()) {
			// --- 用户选择图像背景 ---
			QString fileName = QFileDialog::getOpenFileName(this, "选择背景图像", "", "图像文件 (*.png *.jpg *.bmp)");
			if (fileName.isEmpty()) {
				return; // 用户取消了选择
			}
			QImage selectedBgImage(fileName);
			if (selectedBgImage.isNull()) {
				QMessageBox::warning(this, "错误", "无法加载背景图像！");
				return;
			}
			selectedBgImage = selectedBgImage.scaled(processedImg.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation); 
			painter.drawImage(bgImg.rect(), selectedBgImage); // 绘制背景图像
        }
		// 结束背景图像的绘制。背景图像或者是按用户指定的颜色绘制的纯色图像，或者是用户指定的背景图像
		painter.end(); 
		// 使用抗锯齿融合生成更换背景后的图像
		processedImg = antialiasedBlend(processedImg, bgImg, 5); 
		imageDisplay->clearMarkedAreas(2);
		imageDisplay->setImage(processedImg);
		cleaned = false; // 背景已被填充
    }
    // 如果用户点击 Cancel，则不执行任何操作
}

void MainWindow::showOrigImg() {
    if (origImg.isNull()) {
        QMessageBox::warning(this, "警告", "请先载入图像");
    }
    else {
        imageDisplay->clearMarkedAreas(2);
        // imageDisplay->clearScaledImg();
        imageDisplay->setImage(origImg, true);
    }
}
//  切换显示处理结果槽函数
void MainWindow::showResultImg() {
    if (processedImg.isNull()) {
        QMessageBox::warning(this, "警告", "请先处理图像");
    }
    else {
        imageDisplay->clearMarkedAreas(2);
        // imageDisplay->clearScaledImg();
        imageDisplay->setImage(processedImg, true);
    }
}
// 保存处理结果的槽函数
void MainWindow::saveResult() {
    // 检查 nobg_img_data 是否有效且包含图像数据
    // QImage::isNull() 会检查图像是否未初始化或加载失败
    if (processedImg.isNull()) {
        QMessageBox::warning(this, // 父窗口指针
            "警告", // 消息框标题
            "没有可保存的图像，请先处理图像"); // 消息内容
        return; // 如果没有图像，则直接返回
    }

    // 弹出文件保存对话框，让用户选择保存位置和文件名
    // 参数：父窗口, 对话框标题, 默认目录/文件名, 文件类型过滤器
    QString file_path = QFileDialog::getSaveFileName(
        this,
        "保存结果",
        "", // 默认文件名或目录留空
        "PNG 文件 (*.png);;所有文件 (*)" // 文件过滤器，默认选中 PNG
    );

    // 检查用户是否提供了有效的路径（即没有取消对话框）
    if (!file_path.isEmpty()) {
        // 确保文件名以 .png 结尾（如果用户没有输入）
        // Qt::CaseInsensitive 表示不区分大小写
        if (!file_path.endsWith(".png", Qt::CaseInsensitive)) {
            file_path += ".png";
        }

        // 尝试将 QImage 对象保存到指定的文件路径
        // 参数：文件路径, 文件格式 (null 表示从扩展名推断，显式指定 "PNG" 更可靠), 质量 (-1 表示默认/无损)
        bool success = processedImg.save(file_path, "PNG", -1);

        // 根据保存操作的返回值显示不同的提示信息
        if (success) {
            origImg = processedImg;
            QMessageBox::information(this, "提示", "图像已成功保存");
        }
        else {
            // 如果保存失败，显示错误信息
            QMessageBox::critical(this, "错误", QString("保存图像失败: %1").arg(file_path));
            // 在实际应用中，这里可能需要更详细的错误处理或日志记录
        }
    }
    // 如果 file_path 为空，表示用户点击了“取消”，则不执行任何操作
}

// 激活控件槽函数
void MainWindow::enableControls() {
    for (auto it = controlsToDisable.constBegin(); it != controlsToDisable.constEnd(); it++) {
        QWidget* widget = *it;
        widget->setEnabled(true);
    }

}
// 禁用列表中的控件，使用当前QT版本最为推荐的方式遍历
void MainWindow::disableControls() {
    for (auto it = controlsToDisable.constBegin(); it != controlsToDisable.constEnd(); it++) {
        QWidget* widget = *it;
        widget->setEnabled(false);
    }

}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Delete) {
        // 检查 origImg 是否存在
        if (origImg.isNull()) {
            QMessageBox::warning(this, "警告", "请先载入图像");
            return;
        }

        if (processedImg.isNull()) {
            processedImg = origImg.convertToFormat(QImage::Format_ARGB32);
        }
        int width = processedImg.width();
        int height = processedImg.height();
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                ImgView::AreaType areaType = imageDisplay->getPointArea(x, y);
                // 1. 处理前景区和背景区
                if (areaType == ImgView::AreaType::Front || areaType == ImgView::AreaType::Both) {
                    continue;  //前景区及前景与背景重叠区跳过
                }
                if (areaType == ImgView::AreaType::Background) {
                    processedImg.setPixelColor(x, y, Qt::transparent); // 背景区设置为透明色
                    continue;
                }
            }
        }

        // 清除背景区域
        imageDisplay->clearMarkedAreas(1);
        // 更新显示
        imageDisplay->setImage(processedImg);
        cleaned = true; // 标记背景已被清除
    }
    else if (event->key() == Qt::Key_R) {  // 恢复未处理状态
        imageDisplay->clearMarkedAreas(2);
        imageDisplay->setImage(origImg, true);
        cleaned = false;
        processedImg = QImage();
    }
    else {
        // 其他按键事件传递给父类处理
        QMainWindow::keyPressEvent(event);
    }
}

QImage MainWindow::getAlphaChannel(const QImage& image) {
	if (!image.hasAlphaChannel()) {
		return QImage();
	}
	QImage alphaMask(image.size(), QImage::Format_Alpha8);
	for (int y = 0; y < image.height(); ++y) {
		for (int x = 0; x < image.width(); ++x) {
			QRgb pixel = image.pixel(x, y);
			uchar alpha = qAlpha(pixel);
			alphaMask.setPixel(x, y, alpha);
		}
	}
	return alphaMask;
}

QImage MainWindow::antialiasedBlend(const QImage& foreground, const QImage& background, int blurRadius) {
	if (foreground.isNull() || background.isNull() || foreground.size() != background.size()) {
		return QImage();
	}

	QImage alphaMask = getAlphaChannel(foreground);
	QImage blurredAlpha;
	if (!alphaMask.isNull()) {
		QGraphicsScene scene;
		QGraphicsPixmapItem item(QPixmap::fromImage(alphaMask));
		QGraphicsBlurEffect blurEffect;
		blurEffect.setBlurRadius(blurRadius);
		item.setGraphicsEffect(&blurEffect);
		scene.addItem(&item);

		blurredAlpha = QImage(alphaMask.size(), QImage::Format_Alpha8);
		blurredAlpha.fill(0);
		QPainter alphaPainter(&blurredAlpha);
		scene.render(&alphaPainter, QRectF(blurredAlpha.rect()), QRectF(alphaMask.rect()));
		alphaPainter.end();
	}
	else {
		blurredAlpha = QImage(foreground.size(), QImage::Format_Alpha8);
		blurredAlpha.fill(255);
	}

	QImage blendedImage = background.copy();
	for (int y = 0; y < foreground.height(); ++y) {
		for (int x = 0; x < foreground.width(); ++x) {
			QRgb foregroundPixel = foreground.pixel(x, y);
			QRgb backgroundPixel = background.pixel(x, y);
			int originalAlpha = qAlpha(foregroundPixel);
			int blurredAlphaValue = qGray(blurredAlpha.pixel(x, y));

			if (originalAlpha < 1) {
				// 将透明的区域视为完全透明，直接使用背景色。透明区域的alpha值不会是绝对的0，所以用阈值判别
				blendedImage.setPixel(x, y, backgroundPixel);
			} else if (originalAlpha < 255) {
				// 在透明和完全不透明之间的区域，用前景及背景进行混合以产生抗锯齿效果
				qreal foregroundAlpha = blurredAlphaValue / 255.0;
				qreal backgroundAlpha = 1.0 - foregroundAlpha;

				int red = qBound(0, qRound(qRed(foregroundPixel) * foregroundAlpha + qRed(backgroundPixel) * backgroundAlpha), 255);
				int green = qBound(0, qRound(qGreen(foregroundPixel) * foregroundAlpha + qGreen(backgroundPixel) * backgroundAlpha), 255);
				int blue = qBound(0, qRound(qBlue(foregroundPixel) * foregroundAlpha + qBlue(backgroundPixel) * backgroundAlpha), 255);

				blendedImage.setPixel(x, y, qRgba(red, green, blue, originalAlpha));
				qDebug() << "blendedImage.pixel(x, y)" << qRgba(red, green, blue, originalAlpha);
			}
			else {
				// 对于不透明的区域，直接使用前景的像素
				blendedImage.setPixel(x, y, foregroundPixel);
			}
		}
	}

	return blendedImage;
}
