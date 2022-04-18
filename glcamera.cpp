#include "glcamera.h"

#include "utils.h"
#include <QtMath>
#include <GL/gl.h>

namespace cam
{

/**********************************************
 * >CameraCube
 * ********************************************/

CameraCube::CameraCube(const QVector3D& center, double radius) :
    m_radius(radius),
    m_center(center)
{

}

/**********************************************
 * >GlCamera
 * ********************************************/

void GlCamera::apply()
{
//    glViewport(m_viewportX, m_viewportY, m_viewportW, m_viewportH);
    m_projection.setToIdentity();
    m_modelView.setToIdentity();
}

void GlCamera::lookAt(const QVector3D &eye, const QVector3D &center, const QVector3D &up)
{
    setEye(eye);
    setCenter(center);
    setUp(up);
}

void GlCamera::pedestal(double y)
{
    m_pedestal += y;
    m_eye += m_up * y;
    m_center += m_up * y;
}

void GlCamera::setCenter(const QVector3D &center)
{
    m_center = m_origCenter = center;
    m_front = (m_center - m_eye).normalized();
    setUp(m_up);
}

void GlCamera::setEye(const QVector3D &eye)
{
    m_eye = m_origEye = eye;
    m_front = (m_center - m_eye).normalized();
    setUp(m_up);
}

void GlCamera::setUp(const QVector3D &up)
{
    m_up = QVector3D::crossProduct(QVector3D::crossProduct(m_front, up), m_front).normalized();
    m_right = QVector3D::crossProduct(m_front, m_up);
}

void GlCamera::setViewport(int x, int y, int w, int h)
{
    m_viewportX = x;
    m_viewportY = y;
    m_viewportW = w;
    m_viewportH = h;
}

void GlCamera::toDefault()
{
    m_eye = m_origEye;
    m_center = m_origCenter;
    m_farPlane      = 2.0;
    m_pedestal      = 0.0;
    m_nearPlane     = 1000.0;
    m_truck         = 0.0;
    m_zoom          = 1.0;
    m_viewportH     = 100;
    m_viewportW     = 100;
    m_viewportX     = 0;
    m_viewportY     = 0;
    m_projection.setToIdentity();
    m_modelView.setToIdentity();
}

void GlCamera::truck(double x)
{
    m_truck += x;
    m_eye += m_right * x;
    m_center += m_right * x;
}

/**********************************************
 * >OrthographicCamera
 * ********************************************/

void OrthographicCamera::apply()
{
    GlCamera::apply();
    m_projection.ortho(m_l * m_zoom, m_r * m_zoom, m_b * m_zoom, m_t * m_zoom, m_nearPlane, m_farPlane);
    m_modelView.lookAt(m_eye, m_center, m_up);
}

void OrthographicCamera::setRect(double left, double right, double bottom, double top)
{
    m_l = left;
    m_r = right;
    m_b = bottom;
    m_t = top;
}

void OrthographicCamera::toDefault()
{
    GlCamera::toDefault();
}

void OrthographicCamera::zoomAt(const QVector2D &v, double factor)
{
    double lastZoom = m_zoom;
    m_zoom *= factor;
    QVector3D vec = QVector3D(-v.x(), v.y(), 0.0) * (m_zoom - lastZoom);
    m_truck += vec.x();
    m_pedestal += vec.y();
    m_eye += m_up * vec.y() + m_right * vec.x();
    m_center += m_up * vec.y() + m_right * vec.x();
}

/**********************************************
 * >PerspectiveCamera
 * ********************************************/

float PerspectiveCamera::arcballAngle(const QVector3D& va, const QVector3D& vb) const
{
    return qAcos(qMin(1.0f, QVector3D::dotProduct(va, vb)));
}

QVector3D PerspectiveCamera::arcballVector(const QVector2D &v, double sWidth, double sHeight) const
{
    QVector3D p = QVector3D(1.0 * v.x() / sWidth * 2.0 - 1.0,
                   1.0 * v.y() / sHeight * 2.0 - 1.0,
                   0.0);
    p.setY(-p.y());
    double opSqr = p.x() * p.x() + p.y() * p.y();
    if(opSqr <= 1) p.setZ(sqrt(1 - opSqr));
    else p.normalize();
    return p;
}

void PerspectiveCamera::apply()
{
    GlCamera::apply();
    m_projection.perspective(m_verticalAngle * m_zoom, m_aspectRatio, m_nearPlane, m_farPlane);
    m_modelView.lookAt(m_eye, m_center, m_up);
}

void PerspectiveCamera::dolly(double z)
{
    m_dolly += z;
    m_eye += m_front * z;
}

void PerspectiveCamera::orbit(const QVector2D& last, const QVector2D& cur)
{
    if(cur == last) return;
    QVector2D d = cur - last;
    d = {d.x() / m_viewportW, d.y() / m_viewportH};
    double rotSpeed = 1;
    double xRad = m_direction.x() + rotSpeed * d.x();
    double yRad = m_direction.y() + rotSpeed * d.y();
    QVector3D dir(cos(xRad) * cos(yRad),
                  sin(yRad),
                  sin(xRad) * cos(yRad));
    m_direction = QVector3D(yRad, yRad, 0);
    setEye(m_center + dir * (m_eye - m_center).length());
//    double xRot = qRadiansToDegrees(rotSpeed * d.x());
//    double yRot = qRadiansToDegrees(rotSpeed * d.y());
//    m_rotation.setX(tv::formatDegree(m_rotation.x() + xRot));
//    m_rotation.setY(tv::formatDegree(m_rotation.y() + yRot));
//    QMatrix4x4 rot;
//    rot.rotate(xRot, {0, 0, 1});
//    if(m_rotation.x() <= 45 or m_rotation.x() >= 315) rot.rotate(yRot, {1, 0, 0});
//    else if(m_rotation.x() > 45 and m_rotation.x() < 135) rot.rotate(-yRot, {0, 1, 0});
//    else if(m_rotation.x() < 315 and m_rotation.x() > 225) rot.rotate(-yRot, {0, 1, 0});
//    else rot.rotate(-yRot, {1, 0, 0});
//    m_eye = m_center + ((m_eye - m_center).toVector4D() * rot).toVector3D();
//    QVector3D right = m_center + ((m_eye + m_right - m_center).toVector4D() * rot).toVector3D();
//    m_right = (right - m_eye).normalized();
//    m_front = (m_center - m_eye).normalized();
}

void PerspectiveCamera::pan(double xDeg)
{
    m_pan += xDeg;
}

void PerspectiveCamera::tilt(double yDeg)
{
    m_tilt += yDeg;
}

void PerspectiveCamera::toDefault()
{
    m_aspectRatio    = 4.0/3.0;
    m_dolly          = 0.0;
    m_pan            = 0.0;
    m_tilt           = 0.0;
    m_verticalAngle  = 60.0;
    GlCamera::toDefault();
}

} //namespace cam
