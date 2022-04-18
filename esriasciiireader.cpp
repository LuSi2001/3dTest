#include "esriasciiireader.h"
#include <QDebug>

namespace ascii
{

EsriAsciiReader::EsriAsciiReader(const QString &fName) :
    m_file(fName)
{
    openFile();
    readContents();
    calculateNormals();
    closeFile();
}

bool EsriAsciiReader::openFile()
{
    if(m_file.open(QIODevice::ReadOnly)) return true;
    qDebug() << "Cannot open file '" + m_file.fileName() + "' with error: " + m_file.errorString();
    return false;
}

void EsriAsciiReader::calculateNormals()
{
    int count = 0;
    for(GLuint i : m_indices)
    {
        if(count % 3 == 0)
        {
            QVector3D norm = QVector3D::crossProduct(m_vertices[i].pos - m_vertices[i + 1].pos,
                                                     m_vertices[i].pos - m_vertices[i + 2].pos).normalized();
            m_vertices[i].norm = m_vertices[i + 1].norm = m_vertices[i + 2].norm = norm;
        }
        ++count;
    }
}

void EsriAsciiReader::closeFile()
{
    m_file.close();
}

/**
 * Interpretates the contents of an esri ascii file.
 */
void EsriAsciiReader::readContents()
{
    QTextStream in(&m_file);
    m_cols          = (in.readLine().split(" ", Qt::SkipEmptyParts))[1].toLong();
    m_rows          = (in.readLine().split(" ", Qt::SkipEmptyParts))[1].toLong();
    m_xllCorner     = (in.readLine().split(" ", Qt::SkipEmptyParts))[1].toDouble();
    m_yllCorner     = (in.readLine().split(" ", Qt::SkipEmptyParts))[1].toDouble();
    m_cellSize      = 1.0;
    double cellSize = (in.readLine().split(" ", Qt::SkipEmptyParts))[1].toDouble();
    m_noDataValue   = (in.readLine().split(" ", Qt::SkipEmptyParts))[1].toInt();

    m_vertices.reserve(m_cols * m_rows);
    m_indices.reserve((m_cols - 1) * (m_rows - 1) * 6);

    size_t col = 0, row = 0;
    double min = 100000, max = -100000;
    while(!in.atEnd())
    {
        QString line = in.readLine();
        for(const QString& str : line.split(" ", Qt::SkipEmptyParts))
        {
            if(col == m_cols)
            {
                col = 0;
                ++row;
            }
            if(not str.isEmpty())
            {
                if(col < m_cols - 1 and row < m_rows - 1)
                {
                    size_t resCol = col + row * m_cols;
                    m_indices.push_back(resCol);
                    m_indices.push_back(resCol + m_cols);
                    m_indices.push_back(resCol + 1);
                    m_indices.push_back(resCol + 1);
                    m_indices.push_back(resCol + m_cols);
                    m_indices.push_back(resCol + m_cols + 1);
                }
                double value = str.toDouble() * cellSize;
                min = std::min(value, min);
                max = std::max(value, max);
                m_vertices.emplace_back(QVector3D(row * m_cellSize, col * m_cellSize, value), QVector3D());
                ++col;
            }
        }
    }
    qDebug() << "Min:" << min << "Max:" << max;
}

} //namespace ascii
