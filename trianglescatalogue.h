#ifndef TRIANGLESCATALOGUE_H
#define TRIANGLESCATALOGUE_H
#include <QVector>
#include "trianglefromcatalogue.h"
#include "triangle.h"

class TrianglesCatalogue
{
public:
    TrianglesCatalogue(QString filename);
    QVector<StarImage> getHips(QVector<Triangle> triangles, QVector<StarImage> stars, double eps);
private:
    QVector<TriangleFromCatalogue> catalogueData;
    QVector<TriangleFromCatalogue> readCatalogue(QString filename);
    int getMaxHIP();
};

#endif // TRIANGLESCATALOGUE_H
