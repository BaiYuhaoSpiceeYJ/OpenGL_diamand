#ifndef LESSON9WIDEGT_H
#define LESSON9WIDEGT_H

#include <QGLWidget>
#include <QOpenGLFunctions_1_1>
#include <QVector3D>
#include <QMatrix4x4>

class lesson9widegt: public QGLWidget, public QOpenGLFunctions_1_1
{
    Q_OBJECT
public:
    lesson9widegt(QWidget* parent = 0);
    ~lesson9widegt();
protected:
    void glPerspective( GLdouble fov, GLdouble aspectRatio, GLdouble zNear, GLdouble zFar );
    void initializeGL();
    void paintGL();
    void resizeGL( int width, int height );
    void drawFloor();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent (QMouseEvent *event);


    void keyPressEvent(QKeyEvent *event);           //处理键盘按下事件
    void wheelEvent (QWheelEvent *event);         //处理滚动按下事件
    void loadGLTextures();//加载纹理图片
    virtual GLuint      makeObject();
public slots:
    void setXRotation(int angle);//接受Slider值改变事件
    void setYRotation(int angle);//接受Slider值改变事件
    void setZRotation(int angle);//接受Slider值改变事件
private:
    QColor qtGreen;
    QColor qtPurple;
    bool fullscreen;//全屏
    bool light;//亮度
    GLfloat xRot, yRot, zRot;//旋转角度
    GLfloat zoom;//压缩
    GLfloat xSpeed, ySpeed;//速度
    GLuint texture[3];//纹理
    GLuint filter;//滤波器
    GLuint fogFilter;
    GLuint object;
    bool paintstate;
    bool mousePressState;
    QPoint lastPos;

    QVector3D m_eye,m_target;
    float m_ez = 5;
    int lightnum;
};

#endif // LESSON9WIDEGT_H
