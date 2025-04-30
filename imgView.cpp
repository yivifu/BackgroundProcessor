#include "imgview.h"
#include <QPainter>
#include <QDebug>
#include <QtMath>
#include <QScrollBar>

ImgView::ImgView(QWidget* parent) : QGraphicsView(parent)
{
    setFixedSize(viewWidth, viewHeight);
    scaleFactor = 1.0;
    scene = new QGraphicsScene(this);
    setScene(scene);

    pixmapItem = new QGraphicsPixmapItem();
	pixmapItem->setAcceptDrops(true); // <--- 添加这行
    scene->addItem(pixmapItem);
	setMouseTracking(true); // 启用鼠标跟踪

    // 设置滚动条策略
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  // 隐藏水平混动条
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  // 隐藏垂直滚动条
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

	setAcceptDrops(true); // 启用拖放功能
	viewport()->setAcceptDrops(true); // <--- 显式添加这行

    // 初始化颜色信息显示框
    lblColorInfo = new QLabel(this);
    lblColorInfo->setStyleSheet("background-color: beige; margin: 5px; width: 90px;");
    lblColorInfo->setVisible(false);
    lblColorInfo->setFixedWidth(90);
}

void ImgView::setImage(const QImage& img, bool bFresh)
{
    origImg = img;
    if (tempPathItem) {
        scene->removeItem(tempPathItem);
        delete tempPathItem;
        tempPathItem = nullptr;
    }

    isDrawingPolygon = false;
    currentPolygon.clear();
    pathsCacheValid = false;

    if (origImg.isNull()) {
        pixmapItem->setPixmap(QPixmap());
        scene->setSceneRect(QRectF());
        return;
    }
    if (bFresh) { // 用1.0乘是为了自动转换为浮点数，否则因整数除法会导致scaleFactor=0
        scaleFactor = qMin(1.0 * viewWidth / origImg.width(), 1.0 * viewHeight / origImg.height());
        if (scaleFactor > 1.0)
            scaleFactor = 1.0;
    }
    scaledImg = origImg.scaled(origImg.size() * scaleFactor, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    // 重绘图片
    scene->setSceneRect(0, 0, scaledImg.width(), scaledImg.height());
    pixmapItem->setPixmap(QPixmap::fromImage(scaledImg));
    // 设置图形项可移动
    pixmapItem->setFlag(QGraphicsItem::ItemIsMovable);

    if (bFresh) {
        QPointF sceneOffset = QPointF(0.0, 0.0);
        sceneOffset.setX((viewWidth - scaledImg.width()) / 2);
        sceneOffset.setY((viewHeight - scaledImg.height()) / 2);
        QPointF scenePos = mapToScene(sceneOffset.toPoint());
        pixmapItem->setPos(scenePos);  // 将图片放在场景原点
    }
}

void ImgView::enterMarkMode(bool isFront)
{
    if (origImg.isNull()) return;
    setCursor(Qt::CrossCursor);
    isMarking = true;
    isMarkingFront = isFront;
    isGettingPixelInfo = false;
}

void ImgView::enterGetPixelInfoMode(bool enable)
{
    if (origImg.isNull() && enable) return;
    isMarking = false;
    isGettingPixelInfo = enable;
    if (enable) {
        setCursor(Qt::CrossCursor);
    }
    else {
        setCursor(Qt::ArrowCursor);
        lblColorInfo->setVisible(false);  // 显示颜色信息框
        emit enableControls();  // 触发激活被禁用控件信号
    }
}

void ImgView::mousePressEvent(QMouseEvent* event)
{
    // 不是标记区域状态也不是查看颜色信息状态，由父控件处理
    if (!isMarking && !isGettingPixelInfo) {
        setCursor(Qt::ClosedHandCursor);
        QGraphicsView::mousePressEvent(event);
        return;
    }

    // 查看颜色信息状态下点击鼠标，退出查看颜色信息状态
    if (isGettingPixelInfo) {
        enterGetPixelInfoMode(false);
        return;
    }
	// 以下实际上都隐含 isMarking == true，即处于标记区域模式下
    QPointF scenePos = mapToScene(event->pos());
    if (event->button() == Qt::LeftButton) { // 点击的是鼠标左键
        if (event->modifiers() & Qt::ControlModifier) { //同时按下了Ctrl键
            // 矩形模式
            if (!isDrawingPolygon) {
                startPos = scenePos;
                if (tempPathItem) {
                    scene->removeItem(tempPathItem);
                    delete tempPathItem;
                }
                tempPathItem = new QGraphicsPathItem();
                tempPathItem->setPen(QPen(Qt::red, 0, Qt::DashLine));
                scene->addItem(tempPathItem);
            }
        }
        else {
            // 多边形模式
            if (!isDrawingPolygon) {
                isDrawingPolygon = true;
                currentPolygon.clear();
                tempPathItem = new QGraphicsPathItem();
                tempPathItem->setPen(QPen(Qt::red, 0, Qt::DashLine));
                scene->addItem(tempPathItem);
            }
            currentPolygon << scenePos;
            updateTempShape(scenePos);
        }
    }
    else if (event->button() == Qt::RightButton && isDrawingPolygon) {  // 点击的是鼠标右键且处于多边形模式下
        currentPolygon << scenePos;
        if (currentPolygon.size() >= 3) {
            QPainterPath path;
            path.addPolygon(currentPolygon);
            path.closeSubpath();
            finishShape(path);  // 结束多边形模式的处理
			// 删除临时路径项，否下会有一个包含原点的路径项显示在视图中
			if (tempPathItem) {
				scene->removeItem(tempPathItem);
				delete tempPathItem;
				tempPathItem = nullptr;
			}
        }
		isDrawingPolygon = false;
        currentPolygon.clear();
	} // 点击的是鼠标右键且多边形绘制已完成（或处于矩形模式下）
	else if (event->button() == Qt::RightButton && (!isDrawingPolygon)) { 
		// 退出标记模式
		isMarking = false;
		setCursor(Qt::ArrowCursor);
		emit enableControls();
	}
}

void ImgView::mouseMoveEvent(QMouseEvent* event)
{
    // 不是标记区域模式也不是查看颜色信息模式，由父控件处理
    if (!isMarking && !isGettingPixelInfo) {
        if (event->button() == Qt::LeftButton)
            setCursor(Qt::ClosedHandCursor);
        QGraphicsView::mouseMoveEvent(event);
        return;
    }

    QPointF scenePos = mapToScene(event->pos());

    if (isGettingPixelInfo) {  // 处于查看颜色信息模式
        QPoint origPos = viewPosToItemPos(event->pos()).toPoint();
        if (origImg.rect().contains(origPos)) {
            QRgb color = origImg.pixel(origPos);
            int gray = qGray(color);  // 计算灰度
            QString info = QString("<b style='color:red'>R:%1</b><br><b style='color:green'>G:%2"
                "</b><br><b style='color:blue'>B:%3</b><br><b>Gray:%4</b>")
                .arg(qRed(color))
                .arg(qGreen(color))
                .arg(qBlue(color))
                .arg(gray);  // 拼接颜色信息字符串
            lblColorInfo->setText(info);

            int infoWidth = lblColorInfo->width();
            int infoHeight = lblColorInfo->height();
            // 计算颜色信息提示框的显示位置，默认提示框左上角显示在鼠标位置右下10px处，可能超出控件右边或下边范围导致提示框不可见时移动到左边或上边
            int posX = event->pos().x() + infoWidth + 10 < viewWidth ? event->pos().x() + 10 :
                event->pos().x() - infoWidth - 10;
            int posY = event->pos().y() + infoHeight + 10 < viewHeight ? event->pos().y() + 10 :
                event->pos().y() - infoHeight - 10;
            lblColorInfo->move(posX, posY);
            lblColorInfo->setVisible(true);  // 显示颜色信息框
        }
        else {
            lblColorInfo->setVisible(false);  // 隐藏颜色信息框
        }
        return;
    }
    // 处于标记区域模式，更新临时区域形状
    updateTempShape(scenePos);
}

void ImgView::mouseReleaseEvent(QMouseEvent* event)
{
    // 不是标记区域状态也不是查看颜色信息状态，由父控件处理
    if (!isMarking || isDrawingPolygon) {
        setCursor(Qt::ArrowCursor);
        QGraphicsView::mouseReleaseEvent(event);
        return;
    }
    // 由于多边形绘制已在mousePress事件中处理完且删除了tempPathItem，
    // 因此鼠标左键弹起时如果存在tempPathItem，即为绘制矩形结束
    if (event->button() == Qt::LeftButton && tempPathItem) {
        QPointF endPos = mapToScene(event->pos());
        QRectF rect = QRectF(startPos, endPos).normalized();

        if (rect.width() > 1 && rect.height() > 1) {
            QPainterPath path;
            path.addRect(rect);
            finishShape(path);
        }

        scene->removeItem(tempPathItem);
        delete tempPathItem;
        tempPathItem = nullptr;
    }
}
// 支持以鼠标滚轮缩放图片，缩放锚点为鼠标所在位置
void ImgView::wheelEvent(QWheelEvent* event)
{
    if (origImg.isNull()) {
        QGraphicsView::wheelEvent(event);
        return;
    }
    if (isMarking || isGettingPixelInfo) {
        return;
    }

    QPointF mousePos = event->position(); // 视图坐标系

    qreal zoomFactor = 1.0;
    if (event->angleDelta().y() > 0) {
        zoomFactor = 1.15;
    }
    else {
        zoomFactor = 1 / 1.15;
    }
    // 限制缩放范围
    qreal newScaleFactorAttempt = scaleFactor * zoomFactor;
    if (newScaleFactorAttempt > 10) {
        zoomFactor = 10 / scaleFactor;
    }
    else if (newScaleFactorAttempt < 0.1) {
        zoomFactor = 0.1 / scaleFactor;
    }
    // 如果 zoomFactor 没有改变（达到边界），则无需继续
    if (qFuzzyCompare(zoomFactor, 1.0)) {
        return;
    }

    QPointF mouseScenePos = mapToScene(mousePos.toPoint());
    QPointF itemScenePos = pixmapItem->pos(); // 缩放前的 Item 场景位置
    // 使用QT图形视图框架内置的功能取得鼠标相对于 Item 左上角的偏移，更能保证准确性
    QPointF offsetToMouse = pixmapItem->mapFromScene(mouseScenePos); // item的局部坐标系

    // 更新总缩放因子
    scaleFactor *= zoomFactor;

    // 计算 pixmapItem 为了保持鼠标锚点需要移动到的最终场景位置
    QPointF targetMouseScenePos = itemScenePos + offsetToMouse * zoomFactor;
    QPointF deltaMove = mouseScenePos - targetMouseScenePos;
    QPointF finalItemPos = itemScenePos + deltaMove;

    // --- 关键步骤：在移动父项前，手动缩放子项的局部路径 ---
    QTransform scaleLocalTransform;
    scaleLocalTransform.scale(zoomFactor, zoomFactor);

    for (auto it = frontPathItems.constBegin(); it != frontPathItems.constEnd(); ++it) {
        // 获取当前局部路径，缩放它，然后设置回去
        QGraphicsPathItem* item = *it;
        item->setPath(scaleLocalTransform.map(item->path()));
    }

    for (auto it = bgPathItems.constBegin(); it != bgPathItems.constEnd(); ++it) {
        // 获取当前局部路径，缩放它，然后设置回去
        QGraphicsPathItem* item = *it;
        item->setPath(scaleLocalTransform.map(item->path()));
    }

    // --- 更新父项 pixmapItem ---
    scaledImg = origImg.scaled(origImg.size() * scaleFactor, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pixmapItem->setPixmap(QPixmap::fromImage(scaledImg));
    pixmapItem->setPos(finalItemPos); // 设置最终位置以保持鼠标锚点

    pathsCacheValid = false; // 标记缓存失效
}
// 鼠标离开则隐藏颜色信息框，以免查看颜色信息时鼠标离开图片显示区后颜色信息框仍然显示
void ImgView::leaveEvent(QEvent* event) {
    if (lblColorInfo->isVisible()) {
        lblColorInfo->setVisible(false);  // 颜色信息提示框设为不可见
    }
    QGraphicsView::leaveEvent(event); // 调用基类的 leaveEvent
}
// 处理拖放事件
void ImgView::dragEnterEvent(QDragEnterEvent* event){
	if (event->mimeData()->hasUrls()) {
		event->acceptProposedAction(); // 接受拖放操作
	} else {
		event->ignore(); // 忽略其他类型的拖放
	}
}
// 处理拖放事件
void ImgView::dragMoveEvent(QDragMoveEvent* event) {
	if (event->mimeData()->hasUrls()) {
		event->acceptProposedAction(); // 接受拖放操作
	}
	else {
		event->ignore(); // 忽略其他类型的拖放
	}
}
// 处理拖放事件
void ImgView::dropEvent(QDropEvent* event) {
	const QMimeData* mimeData = event->mimeData();
	if (mimeData->hasUrls()) {
		QList<QUrl> urlList = mimeData->urls();
		if (!urlList.isEmpty()) {
			QString filePath = urlList.first().toLocalFile();
			QImage image(filePath);
			if (!image.isNull()) {
				emit loadedImage(image); // 发送图片拖放事件信号
			}
		}
	}
}

// 绘制背景网格
void ImgView::drawBackground(QPainter* painter, const QRectF& rect)
{
	QGraphicsView::drawBackground(painter, rect); // 调用基类方法
	// 保存当前变换
    painter->save();
    painter->resetTransform();
    // 固定绘制开始和结束位置，可以是视图区背景固定
    for (int x = 0; x < viewWidth; x += gridSize) {
        for (int y = 0; y < viewHeight; y += gridSize) {
            bool isLightGray = ((x / gridSize) + (y / gridSize)) % 2 == 0;
            painter->setBrush(isLightGray ? lightGray : white);
            painter->setPen(Qt::NoPen);
            painter->drawRect(x, y, gridSize, gridSize);
        }
    }
    // 恢复变换
    painter->restore();
}
// 完成当前路径的保存
void ImgView::updateTempShape(const QPointF& currentPos)
{
    if (!tempPathItem) return;

    QPainterPath path;
    if (isDrawingPolygon) { // 以多边形模式选取区域
        if (currentPolygon.isEmpty()) return;

        QPolygonF previewPoly = currentPolygon;
        previewPoly << currentPos;
        path.addPolygon(previewPoly);
        if (previewPoly.size() > 2) {
            path.closeSubpath();
        }
    }
    else {  // 以矩形模式标记区域
        QRectF rect = QRectF(startPos, currentPos).normalized();
        path.addRect(rect);
    }

    tempPathItem->setPath(path);
}
// 完成形状路径的保存，分别保存为原始图片像素坐标系中的路径和场景中的图形项
void ImgView::finishShape(const QPainterPath& path)
{
	// 1. 使用 mapFromScene 将场景路径直接映射到 Item 坐标系
    // pathInItemCoords 中的坐标是相对于 pixmapItem 左上角的，范围是 scaledImg 的尺寸
    QPainterPath pathInItemCoords = pixmapItem->mapFromScene(path);

    // 2. 将 Item 坐标路径缩放为原始图片坐标
    QTransform transformToOrig;
    transformToOrig.scale(1.0 / scaleFactor, 1.0 / scaleFactor); // 缩放
    QPainterPath origPath = transformToOrig.map(pathInItemCoords);

    // 3. 创建显示用的路径项，使用 Item 坐标系路径，并设置 pixmapItem 为父项
    QGraphicsPathItem* pathItem = new QGraphicsPathItem(pathInItemCoords, pixmapItem);
    pathItem->setPen(QPen(isMarkingFront ? frontMarkColor : bgMarkColor, 1));
    pathItem->setBrush(QBrush(isMarkingFront ?
        QColor(frontMarkColor.red(), frontMarkColor.green(), frontMarkColor.blue(), 60) :
        QColor(bgMarkColor.red(), bgMarkColor.green(), bgMarkColor.blue(), 60)));
    // 不需要再调用 scene->addItem(pathItem)，父子关系会自动处理

    if (isMarkingFront) {
        frontPathItems << pathItem;
        origFrontPaths << origPath; // 存储转换后的原始路径
    }
    else {
        bgPathItems << pathItem;
        origBgPaths << origPath;   // 存储转换后的原始路径
    }

    pathsCacheValid = false; // 标记缓存失效
}
// 清除标记区域
void ImgView::clearMarkedAreas(int area)
{
    if (area == 0 || area == 2) {
        qDeleteAll(frontPathItems);  // 清除前景路径图形项
        frontPathItems.clear();
        origFrontPaths.clear();  // 清除原始前景路径
    }
    if (area == 1 || area == 2) {
        qDeleteAll(bgPathItems);  // 清除前景路径图形项
        bgPathItems.clear();
        origBgPaths.clear();  // 清除原始背景路径
    }
    pathsCacheValid = false;
}


// 将以原始图片像素坐标系坐标保存的前景区和背景区分别合并
void ImgView::updatePathsCache() const
{
    cachedOrigFrontPath = QPainterPath();
    cachedOrigBgPath = QPainterPath();
    for (const QPainterPath& path : origFrontPaths) {
        cachedOrigFrontPath = cachedOrigFrontPath.united(path);
    }
    for (const QPainterPath& path : origBgPaths) {
        cachedOrigBgPath = cachedOrigBgPath.united(path);
    }

    pathsCacheValid = true;
}

// 视图坐标系坐标转换为图片项局部坐标系坐标
QPointF ImgView::viewPosToItemPos(const QPointF& viewPos) {
    QPointF scenePos = mapToScene(viewPos.toPoint());
    QPointF itemPosInScene = pixmapItem->pos();
    return (scenePos - itemPosInScene) / scaleFactor;
}
// 判断原始图片上的点是否在前景区、背景区还是在两区的重叠区里
ImgView::AreaType ImgView::getPointArea(const QPointF& point) const
{
    if (!pathsCacheValid) {
        updatePathsCache();
    }
    bool inFront = cachedOrigFrontPath.contains(point);
    bool inBackground = cachedOrigBgPath.contains(point);

    if (inFront && inBackground) {
        return AreaType::Both;
    }
    else if (inFront) {
        return AreaType::Front;
    }
    else if (inBackground) {
        return AreaType::Background;
    }

    return AreaType::None;
}

ImgView::AreaType ImgView::getPointArea(qreal x, qreal y) const
{
    return getPointArea(QPointF(x, y));
}
