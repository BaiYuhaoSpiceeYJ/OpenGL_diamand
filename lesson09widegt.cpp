#pragma execution_character_set("utf-8")
#include "lesson09widegt.h"
#include <QKeyEvent>
#include <QTimer>
#include <QDebug>
GLfloat lightAmbient[4] = { 0.5, 0.5, 5.5, 1.0 };//环境光参数，半亮白色
GLfloat lightDiffuse[4] = { 1.0, 5.0, 5.0, 1.0 }; //漫射光参数
GLfloat lightPosition[4] = { 5.0, 5.0, 5.0, 1.0 };//光源位置
GLfloat lightPositiona[4] = { 7.0, 8.0, 5.0, 1.0 };//光源位置
GLuint fogMode[3] = { GL_EXP, GL_EXP2, GL_LINEAR };
GLfloat fogColor[4] = { 0.5, 0.5, 0.5, 1.0 };

lesson9widegt::lesson9widegt( QWidget* parent )
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    m_target = QVector3D(0,0,-1);
    lightnum = 0;
    mousePressState = false;
    paintstate = true;
    xRot = yRot = zRot = 0.0;
    zoom = -15.8;
    xSpeed = ySpeed = 0.0;
    filter = 0;
    light = false;
    fogFilter = 0;
    setGeometry( 500,400, 640, 480 );
    QTimer *timer = new QTimer(this);                   //创建一个定时器
    //将定时器的计时信号与updateGL()绑定
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    timer->start(10);                                   //以10ms为一个计时周期
}

lesson9widegt::~lesson9widegt()
{

}

void lesson9widegt::glPerspective( GLdouble fov, GLdouble aspectRatio, GLdouble zNear, GLdouble zFar )
{
    // 使用OpenGL函数，但是需要添加math.h头文件
    GLdouble rFov = fov * 3.14159265 / 180.0;
    glFrustum( -zNear * tan( rFov / 2.0 ) * aspectRatio,
               zNear * tan( rFov / 2.0 ) * aspectRatio,
               -zNear * tan( rFov / 2.0 ),
               zNear * tan( rFov / 2.0 ),
               zNear, zFar );
}

void lesson9widegt::initializeGL()
{
    initializeOpenGLFunctions();//初始化
    loadGLTextures();
    glEnable( GL_TEXTURE_2D );//启用2D
    glShadeModel( GL_SMOOTH ); //启用阴影平滑
    glClearColor( 0.0, 0.0, 0.0, 1.0 );//设置颜色
    glClearDepth( 1.0 );//设置深度缓存
    glEnable( GL_DEPTH_TEST ); //启用深度测试
    glDepthFunc( GL_LEQUAL ); //所作深度测试的类型
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );//告诉系统对透视进行修正
    glLightfv( GL_LIGHT1, GL_AMBIENT, lightAmbient );//创建光源 glLightfv()
    glLightfv( GL_LIGHT2, GL_DIFFUSE, lightDiffuse );//创建光源 glLightfv()
    glLightfv( GL_LIGHT3, GL_POSITION, lightPosition );//创建光源 glLightfv()
    glLightfv( GL_LIGHT4, GL_POSITION, lightPositiona );//创建光源 glLightfv()
    glEnable(GL_LIGHT1); //启用一号光源
    glEnable(GL_LIGHT2); //启用一号光源
    glEnable(GL_LIGHT3); //启用一号光源
    glEnable(GL_LIGHT4); //启用一号光源
    glEnable(GL_LIGHTING); //开光
    glFogi( GL_FOG_MODE, fogMode[fogFilter] );//雾气模式
    glFogfv( GL_FOG_COLOR, fogColor );//雾气颜色
    glFogf( GL_FOG_DENSITY, 0.05 );//雾气密度.
    glHint( GL_FOG_HINT, GL_DONT_CARE );//告诉系统对透视进行修正
    glFogf( GL_FOG_START, 0.0 );//雾气密度
    glFogf( GL_FOG_END, 1.0 );//雾气密度
    glEnable( GL_FOG );
    object = makeObject();              // 生成一个OpenGL显示列表
}

void lesson9widegt::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();
    glTranslatef(  0.0,  1.0, zoom );
    glRotatef(xRot / 16.0, 1.0f, 0.0f, 0.0f);//旋转函数glRotatef()x
    glRotatef(yRot / 16.0, 0.0f, 1.0f, 0.0f);//旋转函数glRotatef()y
    glRotatef(zRot / 16.0, 0.0f, 0.0f, 1.0f);//旋转函数glRotatef()z

    glBindTexture(GL_TEXTURE_2D, texture[filter] );
    drawFloor();
    glCallList( object );

    xRot += xSpeed;
    yRot += ySpeed;
}

void lesson9widegt::resizeGL(int width, int height)
{
    //摄像机
    QMatrix4x4 camera;
    m_eye.setZ(m_ez);
    camera.lookAt(m_eye,m_eye + m_target,QVector3D(0,1,0));

    glViewport(0,0, width, height);//指定视口位置。
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glPerspective( 45.0, (GLfloat)width/(GLfloat)height, 0.1, 100.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}

void lesson9widegt::mousePressEvent(QMouseEvent *event)
{
    qDebug()<<"paintstate"<<paintstate;
    update();
    lastPos = event->pos();
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void lesson9widegt::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        update();
    }
}

void lesson9widegt::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        update();
    }
}

void lesson9widegt::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        update();
    }
}

void lesson9widegt::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();
    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + 8 * dy);
        setZRotation(zRot + 8 * dx);
    }
    lastPos = event->pos();
}

void lesson9widegt::drawFloor()
{
    texture[1] = bindTexture(QPixmap(":/image/data/zuan1.png")); //载入位图并转换成纹理
    glBegin(GL_QUADS);
    glVertex3f(  -3.0,  3.0, -1.0 );
    glVertex3f(  -3.0,  -3.0, -1.0 );
    glVertex3f(  3.0,  -3.0, -1.0 );
    glVertex3f(  3.0,  3.0, -1.0 );
    glEnd();

}

void lesson9widegt::mouseReleaseEvent(QMouseEvent *event)
{
    mousePressState = false;
}

void lesson9widegt::keyPressEvent(QKeyEvent *event)
{
    switch ( event->key() )
    {
    case  Qt::Key_Space:
        ySpeed = 0;
        xSpeed = 0;
        break;
    case  Qt::Key_Meta:
        break;
    case Qt::Key_L:
        glDisable(GL_LIGHT1); //启用一号光源
        glDisable(GL_LIGHT2); //启用一号光源
        glDisable(GL_LIGHT3); //启用一号光源
        glDisable(GL_LIGHT4); //启用一号光源
        if(lightnum == 0){
            glEnable(GL_LIGHT1); //启用一号光源
        }else if(lightnum == 1){
            glEnable(GL_LIGHT2); //启用一号光源
        }else if(lightnum == 2){
            glEnable(GL_LIGHT3); //启用一号光源
        }else if(lightnum == 3){
            glEnable(GL_LIGHT4); //启用一号光源
        }
        lightnum++;
        if(lightnum > 4){
            lightnum = 0;
        }
        updateGL();
        break;
    case Qt::Key_F:
        filter += 1;;
        if ( filter > 2 )
        {
            filter = 0;
        }
        updateGL();
        break;
    case Qt::Key_G:
        fogFilter += 1;;
        if ( fogFilter > 2 )
        {
            fogFilter = 0;
        }
        glFogi( GL_FOG_MODE, fogMode[fogFilter] );
        updateGL();
        break;
    case Qt::Key_PageUp:
        zoom -= 0.2;
        updateGL();
        break;
    case Qt::Key_PageDown:
        zoom += 0.2;
        updateGL();
        break;
    case Qt::Key_Up:
        xSpeed -= 0.5;
        updateGL();
        break;
    case Qt::Key_Down:
        xSpeed += 0.5;
        updateGL();
        break;
    case Qt::Key_Right:
        ySpeed += 0.5;
        updateGL();
        break;
    case Qt::Key_Left:
        ySpeed -= 0.5;
        updateGL();
        break;
    case Qt::Key_F2:
        fullscreen = !fullscreen;
        if ( fullscreen )
        {
            showFullScreen();
        }
        else
        {
            showNormal();
            setGeometry( 0, 0, 640, 480 );
        }
        update();
        break;
    case Qt::Key_Escape:
        close();
    }
}

void lesson9widegt::wheelEvent(QWheelEvent *event)
{
    if (! event->pixelDelta().isNull()) {
        m_ez += event->pixelDelta().y();
    } else if (!event->angleDelta().isNull()) {
        m_ez += (event->angleDelta() / 120).y();
    }
    QPoint p = event->angleDelta();
    if(p.y () >= 120)
    {
        zoom += 0.2;
        updateGL();
    }else
    {
        zoom -= 0.2;
        updateGL();
    }
}

void lesson9widegt::loadGLTextures()
{
    QImage tex, buf;
    if ( !buf.load( ":/image/data/zuan.png" ) )
    {
        qWarning( "Could not read image file, using single-color instead." );
        QImage dummy;
        dummy.fill( QColor(22,66,99));
        buf = dummy;
    }
    tex = QGLWidget::convertToGLFormat( buf );
    //glGenTextures用来生成纹理的函数。函数根据纹理参数返回n个纹理索引
    glGenTextures( 3, &texture[0] );
    // 一幅具有真实感的图像或者照片作为纹理贴到一个矩形上
    glBindTexture( GL_TEXTURE_2D, texture[0] );
    //映射需要重新构造纹理图像,这样就会造成应用到多边形上的图像失真),
    //这时就可用glTexParmeteri()函数来确定如何把纹理象素映射成像素.
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    //生成一个2D纹理（Texture）
    glTexImage2D( GL_TEXTURE_2D, 0, 3, tex.width(), tex.height(), 0,
                  GL_RGBA, GL_UNSIGNED_BYTE, tex.bits() );

    glBindTexture( GL_TEXTURE_2D, texture[1] );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexImage2D( GL_TEXTURE_2D, 0, 3, tex.width(), tex.height(), 0,
                  GL_RGBA, GL_UNSIGNED_BYTE, tex.bits() );

    glBindTexture( GL_TEXTURE_2D, texture[2] );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexImage2D( GL_TEXTURE_2D, 0, 3, tex.width(), tex.height(), 0,
                  GL_RGBA, GL_UNSIGNED_BYTE, tex.bits() );

}
//摄像机视角 到光源和纹理贴图
GLuint lesson9widegt::makeObject()
{
    GLuint list;
    list = glGenLists( 1 );
    glNewList( list, GL_COMPILE );
    qglColor(Qt::white);                      // Shorthand for glColor3f or glIndex
    glLineWidth( 1.0 );
    if(paintstate){
        //第一级点
        glBegin( GL_LINES );
        glVertex3f(  2.0,  0.0, 3.0 );      glVertex3f(  0.0,  0.0, 0.0 );
        glVertex3f(  1.0,  1.732, 3.0 );    glVertex3f(  0.0,  0.0, 0.0 );
        glVertex3f(  -1.0,  -1.732, 3.0 );  glVertex3f(  0.0,  0.0, 0.0 );
        glVertex3f(  -2.0,  0.0, 3.0 );     glVertex3f(  0.0,  0.0, 0.0 );
        glVertex3f(  -1.0,  1.732, 3.0 );   glVertex3f(  0.0,  0.0, 0.0 );
        glVertex3f(  1.0,  -1.732, 3.0 );   glVertex3f(  0.0,  0.0, 0.0 );
        glEnd();

        //第二级点
        glBegin( GL_LINES );
        glVertex3f(  0.8*1.732,  -0.8, 3.5 );   glVertex3f(  0.0,  0.0, 0.0 );
        glVertex3f(  0.8*1.732,  0.8, 3.5 );    glVertex3f(  0.0,  0.0, 0.0 );
        glVertex3f(  0.0,  1.6, 3.5 );          glVertex3f(  0.0,  0.0, 0.0 );
        glVertex3f(  -0.8*1.732,  0.8, 3.5 );   glVertex3f(  0.0,  0.0, 0.0 );
        glVertex3f(  -0.8*1.732,  -0.8, 3.5 );  glVertex3f(  0.0,  0.0, 0.0 );
        glVertex3f(  0.0,  -1.6, 3.5 );         glVertex3f(  0.0,  0.0, 0.0 );
        glEnd();

        //第一级点连接
        glBegin( GL_LINES );
        glVertex3f(  -2.0,  0.0, 3.0 );     glVertex3f(  -1.0,  -1.732, 3.0 );
        glVertex3f(  -1.0,  1.732, 3.0 );   glVertex3f(  -2.0,  0.0, 3.0 );
        glVertex3f(  -1.0,  1.732, 3.0 );   glVertex3f(  1.0,  1.732, 3.0 );
        glVertex3f(  1.0,  1.732, 3.0 );    glVertex3f(  2.0,  0.0, 3.0 );
        glVertex3f(  2.0,  0.0, 3.0 );      glVertex3f(  1.0,  -1.732, 3.0 );
        glVertex3f(  1.0,  -1.732, 3.0 );   glVertex3f(  -1.0,  -1.732, 3.0 );
        glEnd();

        //第二级点连接
        glBegin( GL_LINES );
        glColor3f( 0.0, 0.0, 1.0 );//设置当前色为蓝色
        glVertex3f(  0.8*1.732,  -0.8, 3.5 );  glVertex3f(  0.8*1.732,  0.8, 3.5 );
        glVertex3f(  0.8*1.732,  0.8, 3.5 );   glVertex3f(  0.0,  1.6, 3.5 );
        glVertex3f(  0.0,  1.6, 3.5 );         glVertex3f(  -0.8*1.732,  0.8, 3.5 );
        glColor3f( 0.0, 1.0, 1.0 );//设置当前色为蓝色
        glVertex3f(  -0.8*1.732,  0.8, 3.5 );  glVertex3f(  -0.8*1.732,  -0.8, 3.5 );
        glVertex3f(  -0.8*1.732,  -0.8, 3.5 ); glVertex3f(  0.0,  -1.6, 3.5 );
        glVertex3f(  0.0,-1.6,3.5);            glVertex3f(  0.8*1.732,  -0.8, 3.5 );
        glEnd();

        /*点位对应
    //1==(2,0,3);        7==(0.8*1.732,-0.8,3.5);
    //2==(1,1.732,3);    8==(0.8*1.732,0.8,3.5);
    //3==(-1,1.732,3);   9==(0,1.6,3.5);
    //4==(-2,0,3);       10==(-0.8*1.732,0.8,3.5);
    //5==(-1,-1.732,3);  11==(-0.8*1.732,-0.8,3.5);
    //6==(1,-1.732,3);   12==(0,-1.6,3.5);
    //0==(0,0,0);        13==(0.0,0.0,3.5 )
    */
        //一二级点连接
        glBegin( GL_LINES );
        glVertex3f( 1,-1.732,3 );         glVertex3f( 0.8*1.732,-0.8,3.5);//6-7
        glVertex3f(0.8*1.732,-0.8,3.5);   glVertex3f(2,0,3);//7-1
        glVertex3f(0.8*1.732,0.8,3.5);    glVertex3f(2,0,3);//8-1
        glVertex3f(0.8*1.732,0.8,3.5);    glVertex3f(1,1.732,3);//8-2
        glVertex3f(0,1.6,3.5);            glVertex3f(1,1.732,3);//9-2
        glVertex3f(0,1.6,3.5);            glVertex3f(-1,1.732,3);//9-3
        glVertex3f(-0.8*1.732,0.8,3.5);   glVertex3f(-1,1.732,3);//10-3
        glVertex3f(-0.8*1.732,0.8,3.5);   glVertex3f(-2,0,3);//10-4
        glVertex3f(-0.8*1.732,-0.8,3.5);  glVertex3f(-2,0,3);//11-4
        glVertex3f(-0.8*1.732,-0.8,3.5);  glVertex3f(-1,-1.732,3);//11-5
        glVertex3f(0,-1.6,3.5);           glVertex3f(-1,-1.732,3);//12-5
        glVertex3f(0,-1.6,3.5);           glVertex3f(1,-1.732,3);//12-6
        glEnd();

        //二级中心点连接
        glBegin( GL_LINES );
        glVertex3f(  0.8*1.732,  -0.8, 3.5 );   glVertex3f(  0.0,  0.0, 3.5 );
        glVertex3f(  0.8*1.732,  0.8, 3.5 );    glVertex3f(  0.0,  0.0, 3.5 );
        glVertex3f(  0.0,  1.6, 3.5 );          glVertex3f(  0.0,  0.0, 3.5 );
        glVertex3f(  -0.8*1.732,  0.8, 3.5 );   glVertex3f(  0.0,  0.0, 3.5 );
        glVertex3f(  -0.8*1.732,  -0.8, 3.5 );  glVertex3f(  0.0,  0.0, 3.5 );
        glVertex3f(  0.0,  -1.6, 3.5 );         glVertex3f(  0.0,  0.0, 3.5 );
        glEnd();

        //    }else{
        //贴图
        glBegin( GL_TRIANGLES );
        ///////////////////////顶面
        //13-8-9
        glTexCoord2f( 1.0, 0.0 );glVertex3f(  0.0,  0.0,  3.5 ); //13顶点
        glTexCoord2f( 0.0, 0.0 );glVertex3f(  0.8*1.732, -0.8,  3.5 ); //顶点
        glTexCoord2f( 0.0, 1.0 );glVertex3f( 0.8*1.732, 0.8,  3.5 ); //顶点
        //13-7-8
        glTexCoord2f( 1.0, 0.0 );glVertex3f(  0.0,  0.0,  3.5 ); //顶点
        glTexCoord2f( 0.0, 0.0 );glVertex3f(  0.8*1.732, 0.8,  3.5 ); //顶点
        glTexCoord2f( 0.0, 1.0 );glVertex3f( 0.0,  1.6, 3.5 ); //顶点
        //13-12-7
        glTexCoord2f( 1.0, 0.0 );glVertex3f(  0.0,  0.0,  3.5 ); //顶点
        glTexCoord2f( 0.0, 0.0 );glVertex3f( 0,-1.6,3.5 ); //顶点
        glTexCoord2f( 0.0, 1.0 );glVertex3f( 0.8*1.732,-0.8,3.5 ); //顶点
        //13-12-11
        glTexCoord2f( 1.0, 0.0 );glVertex3f(  0.0,  0.0,  3.5 ); //顶点
        glTexCoord2f( 0.0, 0.0 );glVertex3f( -0.8*1.732,-0.8,3.5); //顶点
        glTexCoord2f( 0.0, 1.0 );glVertex3f( 0,-1.6,3.5 ); //顶点
        //13-11-10
        glTexCoord2f( 1.0, 0.0 );glVertex3f(-0.8*1.732,0.8,3.5); //顶点
        glTexCoord2f( 0.0, 0.0 );glVertex3f( -0.8*1.732,-0.8,3.5); //顶点
        glTexCoord2f( 0.0, 1.0 );glVertex3f( 0.0,  0.0,  3.5 ); //顶点
        //13-10-9
        glTexCoord2f( 1.0, 0.0 );glVertex3f(  0.0,  0.0,  3.5 ); //顶点
        glTexCoord2f( 0.0, 0.0 );glVertex3f( -0.8*1.732,0.8,3.5); //顶点
        glTexCoord2f( 0.0, 1.0 );glVertex3f( 0,1.6,3.5 ); //顶点

        ///////////////////////椎体测面
        //0-1-2
        glTexCoord2f( 1.0, 0.0 );glVertex3f(  0.0,  0.0,  0.0 ); //顶点
        glTexCoord2f( 0.0, 0.0 );glVertex3f( 2,0,3); //顶点
        glTexCoord2f( 0.0, 1.0 );glVertex3f( 1,1.732,3 ); //顶点
        //0-2-3
        glTexCoord2f( 1.0, 0.0 );glVertex3f(  0.0,  0.0,  0.0 ); //顶点
        glTexCoord2f( 0.0, 0.0 );glVertex3f( -1,1.732,3); //顶点
        glTexCoord2f( 0.0, 1.0 );glVertex3f( 1,1.732,3 ); //顶点
        //0-3-4
        glTexCoord2f( 1.0, 0.0 );glVertex3f(  0.0,  0.0,  0.0 ); //顶点
        glTexCoord2f( 0.0, 0.0 );glVertex3f( -1,1.732,3); //顶点
        glTexCoord2f( 0.0, 1.0 );glVertex3f( -2,0,3 ); //顶点
        //0-4-5
        glTexCoord2f( 1.0, 0.0 );glVertex3f(  0.0,  0.0,  0.0 ); //顶点
        glTexCoord2f( 0.0, 0.0 );glVertex3f( -1,-1.732,3); //顶点
        glTexCoord2f( 0.0, 1.0 );glVertex3f( -2,0,3 ); //顶点
        //0-5-6
        glTexCoord2f( 1.0, 0.0 );glVertex3f(  0.0,  0.0,  0.0 ); //顶点
        glTexCoord2f( 0.0, 0.0 );glVertex3f( -1,-1.732,3); //顶点
        glTexCoord2f( 0.0, 1.0 );glVertex3f( 1,-1.732,3 ); //顶点
        //0-6-1
        glTexCoord2f( 1.0, 0.0 );glVertex3f(  0.0,  0.0,  0.0 ); //顶点
        glTexCoord2f( 0.0, 0.0 );glVertex3f( 2,0,3); //顶点
        glTexCoord2f( 0.0, 1.0 );glVertex3f( 1,-1.732,3 ); //顶点

        ///////////////////////钻石面1-2-3-4-5-6
        //1-7-8
        glTexCoord2f( 1.0, 0.0 );glVertex3f( 2,0,3); //顶点
        glTexCoord2f( 0.0, 0.0 );glVertex3f( 0.8*1.732,-0.8,3.5); //顶点
        glTexCoord2f( 0.0, 1.0 );glVertex3f( 0.8*1.732,0.8,3.5); //顶点
        //2-8-9
        glTexCoord2f( 1.0, 0.0 );glVertex3f( 1,1.732,3); //顶点
        glTexCoord2f( 0.0, 0.0 );glVertex3f( 0.8*1.732,0.8,3.5); //顶点
        glTexCoord2f( 0.0, 1.0 );glVertex3f( 0,1.6,3.5); //顶点
        //3-9-10
        glTexCoord2f( 1.0, 0.0 );glVertex3f( -1,1.732,3); //顶点
        glTexCoord2f( 0.0, 0.0 );glVertex3f( 0,1.6,3.5); //顶点
        glTexCoord2f( 0.0, 1.0 );glVertex3f( -0.8*1.732,0.8,3.5); //顶点
        //4-10-11
        glTexCoord2f( 1.0, 0.0 );glVertex3f( -2,0,3); //顶点
        glTexCoord2f( 0.0, 0.0 );glVertex3f( -0.8*1.732,-0.8,3.5); //顶点
        glTexCoord2f( 0.0, 1.0 );glVertex3f( -0.8*1.732,0.8,3.5); //顶点
        //5-11-12
        glTexCoord2f( 1.0, 0.0 );glVertex3f( -1,-1.732,3); //顶点
        glTexCoord2f( 0.0, 0.0 );glVertex3f( -0.8*1.732,-0.8,3.5); //顶点
        glTexCoord2f( 0.0, 1.0 );glVertex3f( 0,-1.6,3.5); //顶点
        //6-7-12
        glTexCoord2f( 1.0, 0.0 );glVertex3f( 1,-1.732,3); //顶点
        glTexCoord2f( 0.0, 0.0 );glVertex3f( 0.8*1.732,-0.8,3.5); //顶点
        glTexCoord2f( 0.0, 1.0 );glVertex3f( 0,-1.6,3.5); //顶点

        ///////////////////////钻石面7-8-9-10-11-12
        //7-6-1
        glTexCoord2f( 1.0, 0.0 );glVertex3f(0.8*1.732,-0.8,3.5); //顶点
        glTexCoord2f( 0.0, 0.0 );glVertex3f( 1,-1.732,3); //顶点
        glTexCoord2f( 0.0, 1.0 );glVertex3f(2,0,3); //顶点
        //8-1-2
        glTexCoord2f( 1.0, 0.0 );glVertex3f(0.8*1.732,0.8,3.5); //顶点
        glTexCoord2f( 0.0, 0.0 );glVertex3f(1,1.732,3); //顶点
        glTexCoord2f( 0.0, 1.0 );glVertex3f(2,0,3); //顶点
        //9-2-3
        glTexCoord2f( 1.0, 0.0 );glVertex3f(0,1.6,3.5); //顶点
        glTexCoord2f( 0.0, 0.0 );glVertex3f(1,1.732,3); //顶点
        glTexCoord2f( 0.0, 1.0 );glVertex3f(-1,1.732,3); //顶点
        //10-3-4
        glTexCoord2f( 1.0, 0.0 );glVertex3f(-0.8*1.732,0.8,3.5); //顶点
        glTexCoord2f( 0.0, 0.0 );glVertex3f(-2,0,3); //顶点
        glTexCoord2f( 0.0, 1.0 );glVertex3f(-1,1.732,3); //顶点
        //11-4-5
        glTexCoord2f( 1.0, 0.0 );glVertex3f(-0.8*1.732,-0.8,3.5); //顶点
        glTexCoord2f( 0.0, 0.0 );glVertex3f(-2,0,3); //顶点
        glTexCoord2f( 0.0, 1.0 );glVertex3f(-1,-1.732,3); //顶点
        //12-5-6
        glTexCoord2f( 1.0, 0.0 );glVertex3f(0,-1.6,3.5); //顶点
        glTexCoord2f( 0.0, 0.0 );glVertex3f(1,-1.732,3); //顶点
        glTexCoord2f( 0.0, 1.0 );glVertex3f(-1,-1.732,3); //顶点
        /*点位对应
    //1==(2,0,3);        7==(0.8*1.732,-0.8,3.5);
    //2==(1,1.732,3);    8==(0.8*1.732,0.8,3.5);
    //3==(-1,1.732,3);   9==(0,1.6,3.5);
    //4==(-2,0,3);       10==(-0.8*1.732,0.8,3.5);
    //5==(-1,-1.732,3);  11==(-0.8*1.732,-0.8,3.5);
    //6==(1,-1.732,3);   12==(0,-1.6,3.5);
    //0==(0,0,0);        13==(0.0,0.0,3.5 )
    */
        glEnd();
    }
    glEndList();

    return list;
}

