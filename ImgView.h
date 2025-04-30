#ifndef IMGVIEW_H
#define IMGVIEW_H


#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsPathItem>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPointF>
#include <QPainterPath>
#include <QPolygonF>
#include <QList>
#include <QColor>
#include <QImage>
#include <QLabel>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDragMoveEvent>
#include <QMimeData>
#include <QUrl>

class ImgView : public QGraphicsView
{
    Q_OBJECT

public:
    enum class AreaType {
        None,       // 不在任何区域内
        Front,      // 在前景区域内
        Background, // 在背景区域内
        Both        // 在前景和背景重叠区域内
    };

    explicit ImgView(QWidget* parent = nullptr);
    void setImage(const QImage& img, bool bFresh = false);
    void enterMarkMode(bool front);
    void clearMarkedAreas(int area);
    void enterGetPixelInfoMode(bool enable);

    // 点区域判断
    AreaType getPointArea(const QPointF& point) const;
    AreaType getPointArea(qreal x, qreal y) const;



signals:
    // 激活被禁用控件信号
    void enableControls();
	void loadedImage(const QImage& img); // 图片加载完成信号

protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void leaveEvent(QEvent* event) override;  // 隐藏颜色信息框
	// 启用拖放功能，必须重写dragMoveEvent，否则不能触发dropEvent
	void dragEnterEvent(QDragEnterEvent* event) override;
	void dropEvent(QDropEvent* event) override;
	void dragMoveEvent(QDragMoveEvent* event) override;

private:
    // 场景和图像项
    QGraphicsScene* scene;
    QGraphicsPixmapItem* pixmapItem;  // 场景中显示缩放后的图片的图像项目
    QImage origImg;  // 原始图片
    QImage scaledImg;  // 缩放后的图片
    qreal scaleFactor = 1.0;  //场景缩放比例
    QColor frontMarkColor = QColor(255, 148, 48);  // 前景标记框颜色
    QColor bgMarkColor = QColor(0, 175, 255);  // 背景标记框颜色

    // 视图宽度和高度
    const int viewWidth = 600;
    const int viewHeight = 660;

    // 颜色信息显示标签
    QLabel* lblColorInfo;

    // 标记状态
    bool isMarking = false;
    bool isMarkingFront = false;
    bool isGettingPixelInfo = false;

    // 多边形绘制状态
    bool isDrawingPolygon = false;
    QPolygonF currentPolygon;
    QPointF startPos;
    QPainterPath currentPath;
    QPainterPath origCurrentPath;

    // 临时绘制项
    QGraphicsPathItem* tempPathItem = nullptr;

    // 标记区域存储    
    QList<QGraphicsPathItem*> frontPathItems;
    QList<QGraphicsPathItem*> bgPathItems;

    QList<QPainterPath> origFrontPaths;
    QList<QPainterPath> origBgPaths;

    // 路径缓存
    mutable QPainterPath cachedOrigFrontPath;
    mutable QPainterPath cachedOrigBgPath;
    mutable bool pathsCacheValid = false;

    // 背景绘制参数
    static const int gridSize = 20;
    const QColor lightGray = QColor(200, 200, 200);
    const QColor white = QColor(255, 255, 255);

    // 辅助函数
    void finishShape(const QPainterPath& path);
    void updateTempShape(const QPointF& currentPos);
    void updatePathsCache() const;

    // 坐标转换
    QPointF viewPosToItemPos(const QPointF& viewPos);
};


#endif // IMGVIEW_H
