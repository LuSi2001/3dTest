#include "camera.h"

namespace camera
{

/*****************************************
 *> CAMERAMOVEMENT
 * **************************************/

//------->Public

Camera::CameraMovement::CameraMovement(const QVector3D& worldTrans, const QVector2D& hvTrans, const QVector2D& xyRot, double d, double z) :
    dolly(d), pan(xyRot.x()), pedestal(hvTrans.y()), tilt(xyRot.y()), truck(hvTrans.x()), zoom(z), translation(worldTrans)
{
}

/*****************************************
 *> VIEWPORT
 * **************************************/

//------->Public

Camera::Viewport::Viewport(const QVector2D& p, const QVector2D& d) :
    dimension(d),
    pos(p)
{
}

Camera::Viewport::Viewport(int x, int y, int w, int h) :
    dimension(w, h),
    pos(x, y)
{
}

/*****************************************
 *> CAMERA
 * **************************************/

//------->Public

void Camera::dolly(double t)
{
    m_movement.dolly += t;
    QVector3D resEye = m_eye + m_front * t;
    if((resEye - m_eye).length() < (m_center - m_eye).length()) m_eye += m_front * t;
}

void Camera::lookAt(const QVector3D& eye, const QVector3D& center, const QVector3D& up)
{
    setEye(eye);
    setCenter(center);
    setUp(up);
}

void Camera::matricesToIdentity()
{
    m_projection.setToIdentity();
    m_view.setToIdentity();
}

void Camera::orbit(double xDeg, double yDeg)
{
    double xRad = qDegreesToRadians(xDeg);
    double yRad = qDegreesToRadians(yDeg);
    QVector3D dir(m_direction.x() * cos(xRad) * cos(yRad),
                  m_direction.y() * sin(yRad),
                  m_direction.z() * sin(xRad) * cos(yRad));
    m_direction = dir;
    setEye(m_center + dir * (m_eye - m_center).length());
}

void Camera::pan(double xDeg)
{
    m_movement.pan += xDeg;
}

void Camera::pedestal(double y)
{
    m_movement.pedestal += y;
    QVector3D up = QVector3D::crossProduct(QVector3D::crossProduct(m_front, m_up), m_front).normalized();
    m_eye += up * y;
    m_center += up * y;
}

void Camera::setAspectRatio(double ratio)
{
    m_aspectRatio = ratio;
}

void Camera::setCenter(const QVector3D& v)
{
    m_center = v;
    calculateFront();
}

void Camera::setEye(const QVector3D& v)
{
    m_eye = v;
    calculateFront();
}

void Camera::setFarPlane(double p)
{
    m_farPlane = p;
}

void Camera::setFront(const QVector3D& v)
{
    m_front = v;
}

void Camera::setNearPlane(double p)
{
    m_nearPlane = p;
}

void Camera::setRight(const QVector3D& v)
{
    m_right = v;
}

void Camera::setUp(const QVector3D& v)
{
    m_up = QVector3D::crossProduct(QVector3D::crossProduct(m_front, v), m_front).normalized();
    calculateRight();
}

void Camera::setVerticalAngle(double angle)
{
    m_verticalAngle = angle;
}

void Camera::setViewport(const Camera::Viewport& v)
{
    m_viewport = v;
}

void Camera::setViewport(int x, int y, int w, int h)
{
    m_viewport = Camera::Viewport(x, y, w, h);
}

void Camera::tilt(double yDeg)
{
    m_movement.tilt += yDeg;
}

void Camera::translate(const QVector3D& v)
{
    m_movement.translation += v;
}

void Camera::truck(double x)
{
    m_movement.truck += x;
    m_eye += m_right * x;
    m_center += m_right * x;
}

void Camera::toDefault()
{
    matricesToIdentity();
    m_aspectRatio   = 4.0/3.0;
    m_farPlane      = 1000;
    m_nearPlane     = 2;
    m_verticalAngle = 45;
    m_center        = QVector3D();
    m_eye           = QVector3D();
    m_front         = QVector3D();
    m_right         = QVector3D();
    m_up            = QVector3D();
    m_movement      = CameraMovement();
    m_viewport      = Viewport();
}

void Camera::zoom(double fac)
{
    m_movement.zoom *= fac;
}

//------->Protected

void Camera::calculateFront()
{
    m_front = (m_center - m_eye).normalized();
}

void Camera::calculateRight()
{
    m_right = QVector3D::crossProduct(m_front, m_up).normalized();
}

/*****************************************
 *> ORTHOGRAPHICCAMERA
 * **************************************/

//------->Public

void OrthographicCamera::applySettings()
{
    Camera::matricesToIdentity();
    double z = zoom();
    rProjectionMatrix().ortho(m_area.left() * z,
                              m_area.right() * z,
                              m_area.bottom() * z,
                              m_area.top() * z,
                              nearPlane(),
                              farPlane());
    rViewMatrix().lookAt(eye(), center(), up());
}

void OrthographicCamera::setBottom(double bottom)
{
    m_area.setBottom(bottom);
}

void OrthographicCamera::setLeft(double left)
{
    m_area.setLeft(left);
}

void OrthographicCamera::setRect(double left, double right, double bottom, double top)
{
    m_area.setLeft(left);
    m_area.setRight(right);
    m_area.setTop(top);
    m_area.setBottom(bottom);
}

void OrthographicCamera::setRect(const QRectF& r)
{
    m_area = r;
}

void OrthographicCamera::setRight(double right)
{
    m_area.setRight(right);
}

void OrthographicCamera::setTop(double top)
{
    m_area.setTop(top);
}

void OrthographicCamera::toDefault()
{
    Camera::toDefault();
    m_area = QRectF();
}

void OrthographicCamera::zoomAt(const QVector2D &v, double factor)
{
    double lastZoom = zoom();
    zoom(factor);
    QVector3D vec(-v.x(), v.y(), 0.0);
    vec *= (zoom() - lastZoom);
    truck(vec.x());
    pedestal(vec.y());
    QVector3D upv = up();
    QVector3D rv = right();
    setEye(eye() + upv * vec.y() + rv * vec.x());
    setCenter(center() + upv * vec.y() + rv * vec.x());
}

/*****************************************
 *> PERSPECTIVECAMERA
 * **************************************/

//------->Public

void PerspectiveCamera::applySettings()
{
    Camera::matricesToIdentity();
    rProjectionMatrix().perspective(verticalAngle() * zoom(), aspectRatio(), nearPlane(), farPlane());
    rViewMatrix().lookAt(eye(), center(), up());
}

void PerspectiveCamera::toDefault()
{
    Camera::toDefault();
}

void PerspectiveCamera::zoomAt(const QVector2D& v, double factor)
{
    Q_UNUSED(v);
    Q_UNUSED(factor);
}

} //namespace cam
