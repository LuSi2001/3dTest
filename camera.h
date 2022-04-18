#ifndef CAMERA_H
#define CAMERA_H

#include "utils.h"
#include <QMatrix4x4>

namespace camera
{

class Camera
{
public:
    enum CameraType
    {
        Orthographic = 0,
        Perspective
    };
    struct CameraMovement
    {
        double      dolly       = 0;
        double      pan         = 0;
        double      pedestal    = 0;
        double      tilt        = 0;
        double      truck       = 0;
        double      zoom        = 0;
        QVector3D   translation;
        CameraMovement() = default;
        CameraMovement(const QVector3D& worldTrans, const QVector2D& hvTrans, const QVector2D& xyRot, double d, double z);
    };
    struct Viewport
    {
        QVector2D dimension;
        QVector2D pos;
        Viewport() = default;
        Viewport(const QVector2D& p, const QVector2D& d);
        Viewport(int x, int y, int w, int h);
    };

    Camera() = default;
    const CameraMovement& movement() const{return m_movement;}
    const QMatrix4x4& viewMatrix() const{return m_view;}
    const QMatrix4x4& projectionMatrix() const{return m_projection;}
    const QVector3D& center() const{return m_center;}
    const QVector3D& eye() const{return m_eye;}
    const QVector3D& front() const{return m_front;}
    const QVector3D& right() const{return m_right;}
    const QVector3D& translation() const{return m_movement.translation;}
    const QVector3D& up() const{return m_up;}
    double aspectRatio() const{return m_aspectRatio;}
    double dolly() const{return m_movement.dolly;}
    double farPlane() const{return m_farPlane;}
    double nearPlane() const{return m_nearPlane;}
    double pan() const{return m_movement.pan;}
    double pedestal() const{return m_movement.pedestal;}
    double tilt() const{return m_movement.tilt;}
    double truck() const{return m_movement.truck;}
    double verticalAngle() const{return m_verticalAngle;}
    double zoom() const{return m_movement.zoom;}
    QMatrix4x4& rViewMatrix(){return m_view;}
    QMatrix4x4& rProjectionMatrix(){return m_projection;}
    const Viewport& viewport() const{return m_viewport;}
    virtual void applySettings() = 0;
    virtual void dolly(double t);
    virtual void lookAt(const QVector3D& eye, const QVector3D& center, const QVector3D& up);
    virtual void orbit(double xDeg, double yDeg);
    virtual void pan(double xDeg);
    virtual void pedestal(double y);
    virtual void tilt(double yDeg);
    virtual void toDefault();
    virtual void translate(const QVector3D& v);
    virtual void truck(double x);
    virtual void zoom(double fac);
    virtual void zoomAt(const QVector2D& v, double factor) = 0;
    void matricesToIdentity();
    void setAspectRatio(double ratio);
    void setCenter(const QVector3D& v);
    void setEye(const QVector3D& v);
    void setFarPlane(double p);
    void setFront(const QVector3D& v);
    void setNearPlane(double p);
    void setRight(const QVector3D& v);
    void setUp(const QVector3D& v);
    void setVerticalAngle(double angle);
    void setViewport(const Viewport& v);
    void setViewport(int x, int y, int w, int h);

private:
    CameraMovement  m_movement;
    double          m_aspectRatio   = 4.0/3.0;
    double          m_farPlane      = 1000;
    double          m_nearPlane     = 2;
    double          m_verticalAngle = 45;
    QMatrix4x4      m_projection;
    QMatrix4x4      m_view;
    QVector3D       m_center;
    QVector3D       m_eye;
    QVector3D       m_front;
    QVector3D       m_right;
    QVector3D       m_up;
    Viewport        m_viewport;

protected:
    QVector3D   m_direction;
    void        calculateFront();
    void        calculateRight();
};

class OrthographicCamera : public Camera
{
public:
    OrthographicCamera() = default;
    double areaBottom() const{return m_area.bottom();}
    double areaLeft() const{return m_area.left();}
    double areaRight() const{return m_area.right();}
    double areaTop() const{return m_area.top();}
    const QRectF& rect() const{return m_area;}
    void applySettings() override;
    void setBottom(double bottom);
    void setLeft(double left);
    void setRect(double left, double right, double bottom, double top);
    void setRect(const QRectF& r);
    void setRight(double right);
    void setTop(double top);
    void toDefault() override;
    void zoomAt(const QVector2D& v, double factor) override;

private:
    QRectF m_area;
};

class PerspectiveCamera : public Camera
{
public:
    PerspectiveCamera() = default;
    void applySettings() override;
    void toDefault() override;
    void zoomAt(const QVector2D& v, double factor) override;
};

} //namespace cam

#endif // CAMERA_H
