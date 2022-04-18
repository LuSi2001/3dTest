#include "glwidget.h"
#include "ui_glwidget.h"
#include <QDebug>
#include <QtMath>
#include <QMouseEvent>
#include <QWheelEvent>

GlWidget::GlWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    ui(new Ui::GlWidget),
    m_ascii(":/ascii/gebco_2021_n43.3135986328125_s38.3038330078125_w7.580566406250001_e10.491943359375.asc")
{
    ui->setupUi(this);
}

GlWidget::~GlWidget()
{
    delete ui;
}

void GlWidget::setupShaders()
{
    if(not m_shProg.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/vshader.glsl"))
    {
        qDebug() << "Cannot load vertex shader.";
        QCoreApplication::exit();
    }
    if(not m_shProg.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/fshader.glsl"))
    {
        qDebug() << "Cannot load fragment shader.";
        QCoreApplication::exit();
    }
    if(not m_shProg.link())
    {
        qDebug() << "Can't link shader pipeline.";
        QCoreApplication::exit();
    }
    if(not m_shProg.bind())
    {
        qDebug() << "Can't bind shader pipeline for use.";
        QCoreApplication::exit();
    }
}

/*********************************************
 * > Events
 * ******************************************/

//TODO: Camera Klasse aus projection und modelview

void GlWidget::initializeGL()
{
//    glClearColor(0, 0, 0, 1);
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glCullFace(GL_BACK);
//    f->glEnableClientState(GL_VERTEX_ARRAY);
//    f->glEnableClientState(GL_NORMAL_ARRAY);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCullFace(GL_BACK);

    //> CAMERA SETUP
    //-> ORTHOGRAPHIC
    m_otgCam.setNearPlane(2);
    m_otgCam.setFarPlane(5000);
    m_otgCam.lookAt({50, 50, 3000}, {50, 50, 0}, {0, 1, 0});
    //-> PERSPECTIVE
    m_pstCam.setNearPlane(2);
    m_pstCam.setFarPlane(15000);
    m_pstCam.setVerticalAngle(60.0);
    m_pstCam.lookAt({600, -500, 0}, {600, 350, 0}, {0, 0, 1});

    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(m_ascii.vertexArray().data(), m_ascii.numVertices() * sizeof(tv::Vertex3d));

    setupShaders();

//    m_vbo.allocate(m_terrain.triangleArray().data(), m_terrain.numTriangles() * sizeof(QVector3D));
//    m_vbo.release();
}

void GlWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float   w = float(width()) * .5f;
    float   h = float(height()) * .5f;

    switch(m_camMode)
    {
    case GlCam::Orthographic:
    {
        //> CAMERA VIEWPORT
        m_otgCam.setViewport(0, 0, width(), height());
        //> CAMERA FAR PLANE MEASURES
        m_otgCam.setRect(-w, w, -h, h);
        m_otgCam.apply();
        //> SET MATRICES
//        glMatrixMode(GL_PROJECTION);
//        glLoadMatrixf(m_otgCam.projection().data());
//        glMatrixMode(GL_MODELVIEW);
//        glLoadMatrixf(m_otgCam.modelView().data());
        m_shProg.setUniformValue("mvp_matrix", m_otgCam.projection() * m_otgCam.modelView());

        int vertLoc = m_shProg.attributeLocation("a_position");
        m_shProg.enableAttributeArray(vertLoc);
        m_shProg.setAttributeBuffer(vertLoc, GL_FLOAT, 0, 3, sizeof(tv::Vertex3d));

        int fragLoc = m_shProg.attributeLocation("a_coord");
        m_shProg.enableAttributeArray(fragLoc);
        m_shProg.setAttributeBuffer(fragLoc, GL_FLOAT, 0, 3, sizeof(tv::Vertex3d));

        glDrawElements(GL_TRIANGLES,
                       m_ascii.indexArray().size(),
                       GL_UNSIGNED_INT,
                       m_ascii.indexArray().data());

        break;
    }
    case GlCam::Perspective:
    {
        //> CAMERA VIEWPORT
        m_pstCam.setViewport(0, 0, width(), height());
        //> CAMERA FAR PLANE MEASURES
        m_pstCam.setAspectRatio(w / h);
        m_pstCam.apply();
        //> SET MATRICES
//        glMatrixMode(GL_PROJECTION);
//        glLoadMatrixf(m_pstCam.projection().data());
//        glMatrixMode(GL_MODELVIEW);
//        glLoadMatrixf(m_pstCam.modelView().data());

        m_shProg.setUniformValue("mvp_matrix", m_pstCam.projection() * m_pstCam.modelView());

        int vertLoc = m_shProg.attributeLocation("a_position");
        m_shProg.enableAttributeArray(vertLoc);
        m_shProg.setAttributeBuffer(vertLoc, GL_FLOAT, 0, 3, sizeof(tv::Vertex3d));

        int fragLoc = m_shProg.attributeLocation("a_coord");
        m_shProg.enableAttributeArray(fragLoc);
        m_shProg.setAttributeBuffer(fragLoc, GL_FLOAT, 0, 3, sizeof(tv::Vertex3d));

        glDrawElements(GL_TRIANGLES,
                       m_ascii.indexArray().size(),
                       GL_UNSIGNED_INT,
                       m_ascii.indexArray().data());

        break;
    }
    default:
    {
        break;
    }
    }

//    glViewport(0, 0, width(), height());

//    m_camera.setToIdentity();

//    m_camera.rProjection().frustum(-w * m_tf.sc.x(), w * m_tf.sc.x(),
//                                   -h * m_tf.sc.x(), h * m_tf.sc.x(),
//                                   1, 1000);

//    m_camera.perspective(60.0, w / h, 2, 15000.0);

//    m_camera.ortho(-w, w, -h, h, 2, 10000);

//    m_camera.lookAt({600, -500, 500}, {600, 350, 0}, {0, 0, 1});
//    m_camera.lookAt({50, 50, 3000}, {50, 50, 0}, {0, 1, 0});
//    m_camera.rModelView().rotate(0.01, QVector3D(0, 1, 0));

//    glColor3f(1, 0, 0);
//    glNormalPointer(GL_FLOAT, 2 * sizeof(QVector3D), m_ascii.vertexArray().data() + sizeof(QVector3D));
//    glVertexPointer(3, GL_FLOAT, 2 * sizeof(QVector3D), m_ascii.vertexArray().data());
//    glDrawElements(GL_TRIANGLES,
//                   m_ascii.indexArray().size(),
//                   GL_UNSIGNED_INT,
//                   m_ascii.indexArray().data());

//    glColor4f(0, 0, 1, 0.5);
//    glBegin(GL_QUADS);
//    glVertex2f(0, 0);
//    glVertex2f(1200, 0);
//    glVertex2f(1200, 700);
//    glVertex2f(0, 700);
//    glEnd();

    /*

    glColor3f(1, 0, 0);
//    glVertexPointer(3, GL_FLOAT, 0, m_terrain.vertexArray().data());
//    glDrawElements(GL_TRIANGLES,
//                   m_terrain.indexArray().size(),
//                   GL_UNSIGNED_INT,
//                   m_terrain.indexArray().data());
//    m_vbo.bind();
    glNormalPointer(GL_FLOAT, 2 * sizeof(QVector3D), m_terrain.vertexArray().data() + sizeof(QVector3D));
//    glVertexPointer(3, GL_FLOAT, 0, nullptr);
    glVertexPointer(3, GL_FLOAT, 2 * sizeof(QVector3D), m_terrain.vertexArray().data());
//    glDrawArrays(GL_TRIANGLES, 0, m_terrain.triangleArray().size());
    glDrawElements(GL_TRIANGLES,
                   m_terrain.indexArray().size(),
                   GL_UNSIGNED_INT,
                   m_terrain.indexArray().data());

                   */
}

void GlWidget::resizeGL(int w, int h)
{
    m_height = h;
    m_width = w;
}

void GlWidget::mouseMoveEvent(QMouseEvent *e)
{
    QPointF dis = e->position() - m_dragStart;

    switch(m_camMode)
    {
    case GlCam::Orthographic:
    {
        m_otgCam.truck(-dis.x() * m_otgCam.zoom());
        m_otgCam.pedestal(dis.y() * m_otgCam.zoom());
        break;
    }
    case GlCam::Perspective:
    {
        QVector3D eye = m_pstCam.eye();
        QVector3D center = m_pstCam.center();
        switch(e->buttons())
        {
        case Qt::LeftButton:
        {
            double dollyFac = (center - (eye + (center - eye).normalized() * m_pstCam.dolly())).length()/1000;
            m_pstCam.truck(-dis.x() * m_pstCam.zoom() * dollyFac);
            m_pstCam.pedestal(dis.y() * m_pstCam.zoom() * dollyFac);
            break;
        }
        case Qt::RightButton:
        {
            m_arcCur = e->position();
            m_pstCam.orbit(tv::qpfToQv2(m_arcStart), tv::qpfToQv2(m_arcCur));
            m_arcStart = m_arcCur;
            break;
        }
        default:
        {
            break;
        }
        }
        break;
    }
    default:
    {
        break;
    }
    }

    m_dragStart = e->position();
    update();
}

void GlWidget::mousePressEvent(QMouseEvent *e)
{
    m_dragStart = m_arcStart = e->position();
}

void GlWidget::mouseReleaseEvent(QMouseEvent *e)
{

}

void GlWidget::timerEvent(QTimerEvent *e)
{

}

void GlWidget::wheelEvent(QWheelEvent *e)
{
    if(e->buttons() != Qt::NoButton) return;
    int deltaY = e->angleDelta().y();
    bool front = deltaY > 0;
    float fac = front ? 0.8 : 1.25;

    switch(m_camMode)
    {
    case GlCam::Orthographic:
    {
        m_otgCam.zoomAt(tv::screenPosV(e->position(), size()), fac);
        break;
    }
    case GlCam::Perspective:
    {
        switch(e->modifiers())
        {
        case Qt::NoModifier:
        {
            m_pstCam.dolly(front ? 20 : -20);
            break;
        }
        case Qt::ControlModifier:
        {
            m_pstCam.zoom(fac);
            break;
        }
        case Qt::ShiftModifier:
        {
            m_pstCam.dolly(front ? 10 : -10);
            break;
        }
        default:
        {
            break;
        }
        }
        break;
    }
    default:
    {
        break;
    }
    }
//    m_camera.zoomAt(fac, qpfToQv3(screenPos(e->posF())));
    update();
}

/*********************************************
 * > Slots
 * ******************************************/

void GlWidget::setCameraMode(CamMode mode)
{
    m_camMode = mode;
    update();
}
