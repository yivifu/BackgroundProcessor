#include "constants.h"
#include <QString>

const QString Constants::COMMENT = R"(
     <h2 style='color:red;text-align:center;'>说明</h2>
     <p style='text-indent:24px;margin:0 0 2 0;'>1、点击<b style='color:blue;'>选取前景</b>或<b style='color:blue;'>选取背景</b>
     按钮进入前景或背景区域选取模式。选取的前景区域不会被删除，选取的背景区域总是会被删除。重新选取前景或背景时会清除此前已标记的前景或背景区域。进入区域
选取模式会禁用操作控件，<b style='color:blue;'>点击鼠标右键</b>可退出区域选取模式并激活操作控件。</p>
     <p style='text-indent:24px;margin:0 0 2 0;'>2、区域选取模式下<b style='color:blue;'>点击鼠标左键</b>可以指定多边形顶点画出多边形标记前景或背景，
<b style='color:blue;'>点击鼠标右键</b>完成多边形绘制。<b style='color:blue;'>按住Ctrl键并按住鼠标左键拖拽</b>可以画矩形标记前景或背景。支持鼠标拖放载入图像。</p>
     <p style='text-indent:24px;margin:0 0 2 0;'>3、点击<b style='color:blue;'>查看图像颜色</b>按钮，将鼠标在图像上移动可查看鼠标位置
像素点的颜色信息，以供确定背景颜色范围时参考。在颜色信息查看模式下会禁用操作控件，<b style='color:blue;'>鼠标点击图像显示区域</b>可退出该模式并激活操作控件。</p>
     <p style='text-indent:24px;margin:0 0 2 0;'>4、点击<b style='color:blue;'>原始图像</b>按钮，可以在图像显示区显示原始图像并重新处理。
     依次点击<b style='color:blue;'>处理结果</b>按钮和<b style='color:blue;'>原始图像</b>按钮，可以对比观察处理效果。</p>
     <p style='text-indent:24px;margin:0 0 5 0;'>5、选取背景后按下<b style='color:blue;'>delete键</b>可以删除选中的背景。
     按下<b style='color:blue;'>R键</b>可以撤销所有操作。</p>
)";

const QString Constants::STYLESHEET = R"(
    QPushButton {
        background-color: rgba(245, 245, 120, 0.7);
        color: rgb(0, 0, 255);
        border: 1px solid rgba(255, 255, 255, 0.3);
        padding: 5px 10px;
        border-radius: 4px;
        font-weight: bold;
    }
    #select-front{
        color:rgb(255, 148, 48);
    }
    #select-bg{
        color:#00BFFF;
    }
    QPushButton:hover {
        background-color: rgba(245, 245, 220, 0.5);
    }
    QPushButton:pressed {
        background-color: rgba(245, 245, 120, 0.9);
    }
    #commentLabel {
        background-color: beige;
        border: 1px solid rgba(0, 0, 0, 0.2);
        border-radius: 10px;
        padding: 5px;
        font-size:14px;
    }
    QLineEdit {
        background-color:#FFFFFF;
        color:#6D6E6B;
        border-radius:8px;
        border:2px solid #E8EAEC;
        height:25px;
        qproperty-alignment:AlignHCenter;
    }
    QLineEdit:hover {
        color:#6D6E6B;
        border:2px solid #2F89FC;
    }
    QLineEdit:focus {
        color:#6D6E6B;
        border:2px solid #29F089;
    }
    QLineEdit:disabled {
        background-color:#F0F0F0;
    }
    QGroupBox#c_group::title {
        font-weight: bold;
        color:blue;
    }
)";
