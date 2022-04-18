#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "esriasciiireader.h"
#include "glcamera.h"
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QtOpenGL/QOpenGLBuffer>
#include <QtOpenGLWidgets/QOpenGLWidget>

namespace Ui {
class GlWidget;
}

using EaReader  = ascii::EsriAsciiReader;
using GlCam     = cam::GlCamera;
using CamMode   = GlCam::CameraType;
using OtgCam    = cam::OrthographicCamera;
using PstCam    = cam::PerspectiveCamera;

class GlWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
    Q_OBJECT

public:
    explicit GlWidget(QWidget *parent = nullptr);
    ~GlWidget();

private:
    Ui::GlWidget*           ui;

    int                     m_height;
    int                     m_width;

    CamMode                 m_camMode   = GlCam::Orthographic;
    EaReader                m_ascii;
    OtgCam                  m_otgCam;
    PstCam                  m_pstCam;
    QPointF                 m_dragStart;

    QPointF                 m_arcStart;
    QPointF                 m_arcCur;

    QOpenGLBuffer           m_vbo;
    QOpenGLShaderProgram    m_shProg;

    void setupShaders();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void mouseMoveEvent(QMouseEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void timerEvent(QTimerEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;

public slots:
    void setCameraMode(CamMode mode);
};

#endif // GLWIDGET_H
