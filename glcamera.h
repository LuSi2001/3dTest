#ifndef GLCAMERA_H
#define GLCAMERA_H

#include "utils.h"
#include <QMatrix4x4>

namespace cam
{

class CameraCube
{
public:
    CameraCube(const QVector3D& center, double radius);
    const Indices& indexArray() const{return m_indices;}
    const Vertices& vertexArray() const{return m_vertices;};
    double radius() const{return m_radius;}
    const QVector3D& center() const{return m_center;}
    size_t numIndices() const{return m_indices.size();}
    size_t numVertices() const{return m_vertices.size();}

private:
    double      m_radius;
    Indices     m_indices;
    QVector3D   m_center;
    Vertices    m_vertices;
};

class GlCamera
{
public:
    enum CameraType
    {
        Orthographic = 0,
        Perspective
    };

    GlCamera() = default;
    CameraCube cube(double radius) const;
    const QMatrix4x4& modelView() const{return m_modelView;}
    const QMatrix4x4& projection() const{return m_projection;}
    const QVector3D& center() const{return m_center;}
    const QVector3D& eye() const{return m_eye;}
    const QVector3D& up() const{return m_up;}
    double farPlane() const{return m_farPlane;}
    double nearPlane() const{return m_nearPlane;}
    double pedestal() const{return m_pedestal;}
    double truck() const{return m_truck;}
    double zoom() const{return m_zoom;}
    QMatrix4x4& rModelView(){return m_modelView;}
    QMatrix4x4& rProjection(){return m_projection;}
    virtual void apply();
    virtual void toDefault();
    void lookAt(const QVector3D& eye, const QVector3D& center, const QVector3D& up);
    void pedestal(double y);
    void setCenter(const QVector3D& center);
    void setEye(const QVector3D& eye);
    void setFarPlane(double farPlane){m_farPlane = farPlane;}
    void setNearPlane(double nearPlane){m_nearPlane = nearPlane;}
    void setUp(const QVector3D& up);
    void setViewport(int x, int y, int w, int h);
    void truck(double x);
    void zoom(double factor){m_zoom *= factor;}

protected:
    double      m_farPlane      = 2.0;
    double      m_pedestal      = 0.0;
    double      m_nearPlane     = 1000.0;
    double      m_truck         = 0.0;
    double      m_zoom          = 1.0;
    int         m_viewportH     = 100;
    int         m_viewportW     = 100;
    int         m_viewportX     = 0;
    int         m_viewportY     = 0;
    QMatrix4x4  m_modelView;
    QMatrix4x4  m_projection;
    QVector2D   m_rotation;
    QVector3D   m_center;
    QVector3D   m_eye;
    QVector3D   m_front;
    QVector3D   m_origCenter;
    QVector3D   m_origEye;
    QVector3D   m_right;
    QVector3D   m_up;
};

class OrthographicCamera : public GlCamera
{
public:
    OrthographicCamera() = default;
    double bottom() const{return m_b;}
    double left() const{return m_l;}
    double right() const{return m_r;}
    double top() const{return m_t;}
    void apply() override;
    void setBottom(double bottom){m_b = bottom;}
    void setLeft(double left){m_l = left;}
    void setRect(double left, double right, double bottom, double top);
    void setRight(double right){m_r = right;}
    void setTop(double top){m_t = top;}
    void toDefault() override;
    void zoomAt(const QVector2D& v, double factor);

private:
    double m_b  = -50.0;
    double m_l  = -50.0;
    double m_r  = 50.0;
    double m_t  = 50.0;
};

class PerspectiveCamera : public GlCamera
{
public:
    PerspectiveCamera() = default;
    float arcballAngle(const QVector3D &va, const QVector3D &vb) const;
    double aspectRatio() const{return m_aspectRatio;}
    double dolly() const{return m_dolly;}
    double pan() const{return m_pan;}
    double tilt() const{return m_tilt;}
    double verticalAngle() const{return m_verticalAngle;}
    QVector3D arcballVector(const QVector2D& v, double sWidth, double sHeight) const;
    void apply() override;
    void dolly(double z);
    void orbit(const QVector2D &last, const QVector2D &cur);
    void pan(double xDeg);
    void setAspectRatio(double aspectRatio){m_aspectRatio = aspectRatio;}
    void setVerticalAngle(double verticalAngle){m_verticalAngle = verticalAngle;}
    void tilt(double yDeg);
    void toDefault() override;

private:
    double m_aspectRatio    = 4.0/3.0;
    double m_dolly          = 0.0;
    double m_orbitX         = 0.0;
    double m_orbitY         = 0.0;
    double m_pan            = 0.0;
    double m_tilt           = 0.0;
    double m_verticalAngle  = 60.0;
    QVector3D   m_direction;
};

} //namespace cam

#endif // GLCAMERA_H
