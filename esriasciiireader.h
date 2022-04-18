#ifndef ESRIASCIIREADER_H
#define ESRIASCIIREADER_H

#include "utils.h"
#include <QFile>

namespace ascii
{

class EsriAsciiReader
{
public:
    EsriAsciiReader(const QString& string);
    const Indices& indexArray() const{return m_indices;}
    const Vertices& vertexArray() const{return m_vertices;};
    double cellSize() const{return m_cellSize;}
    double xllCorner() const{return m_xllCorner;}
    double yllCorner() const{return m_yllCorner;}
    int noDataValue() const{return m_noDataValue;}
    size_t numCols() const{return m_cols;}
    size_t numIndices() const{return m_indices.size();}
    size_t numRows() const{return m_rows;}
    size_t numVertices() const{return m_vertices.size();}

private:
    double  m_cellSize;
    double  m_xllCorner;
    double  m_yllCorner;
    int     m_noDataValue;
    QFile   m_file;
    size_t  m_cols;
    size_t  m_rows;

    Indices     m_indices;
    Vertices    m_vertices;

    bool openFile();
    void calculateNormals();
    void closeFile();
    void readContents();
};

} //namespace ascii

#endif // ESRIASCIIREADER_H
