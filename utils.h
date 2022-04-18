#ifndef UTILS_H
#define UTILS_H

#include <GL/gl.h>
#include <QVector3D>

namespace tv
{

/**
 * Contains a three dimensional vector of a triangle with its corresponding normal vector.
 */
struct Vertex3d
{
    QVector3D pos;
    QVector3D norm;
    Vertex3d(const QVector3D& p, const QVector3D& n) :
        pos(p), norm(n)
    {}
};

inline QPointF qv2ToQpf(const QVector2D& v)
{
    return {v.x(), v.y()};
}

inline QVector2D qpfToQv2(const QPointF& p)
{
    return {float(p.x()), float(p.y())};
}

inline QPointF screenPos(const QPointF& p, const QSizeF& screen)
{
    return p - QPointF(screen.width(), screen.height()) / 2.0;
}

inline QVector2D screenPosV(const QPointF& p, const QSizeF& screen)
{
    return qpfToQv2(p - QPointF(screen.width(), screen.height()) / 2.0);
}

inline QVector3D rotatePointAroundPoint(const QVector3D& v, const QVector3D& mv, const QVector3D& r, double degree)
{
    QVector3D res = v - mv;
    QVector3D tmp = res;
    double rad = qDegreesToRadians(degree);
    if(r.x() == 1)
    {
        res.setY(tmp.y() * qCos(rad) - tmp.z() * qSin(rad));
        res.setZ(tmp.y() * qSin(rad) + tmp.z() * qCos(rad));
    }
    if(r.y() == 1)
    {
        res.setX(tmp.x() * qCos(rad) + tmp.z() * qSin(rad));
        res.setZ(-tmp.x() * qSin(rad) + tmp.z() * qCos(rad));
    }
    if(r.z() == 1)
    {
        res.setX(tmp.x() * qCos(rad) - tmp.y() * qSin(rad));
        res.setY(tmp.x() * qSin(rad) + tmp.y() * qCos(rad));
    }
    res += mv;
    return res;
}

inline double formatDegree(double degree)
{
    while(degree < 0) degree += 360;
    while(degree >= 360) degree -= 360;
    return degree;
}

} //namespace tv

using Indices   = std::vector<GLuint>;
using Vertices  = std::vector<tv::Vertex3d>;

#endif // UTILS_H
