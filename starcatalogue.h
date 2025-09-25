#ifndef STARCATALOGUE_H
#define STARCATALOGUE_H
#include <QVector>
#include "triangle.h"

class StarCatalogue
{
private:
    QString filename;
    QVector<QVector<QString>> catalogueData;
public:
    StarCatalogue(QString filename);
    QVector<QVector<QString>> readCatalogue();
    QVector<QVector<QString>> getStarsInView(double alpha0, double delta0);
    QVector<QVector<QVector<QString>>> getStarsNeighbors();
    QVector<QPoint> getArrOfStars(QVector<QVector<QString>> starsNeighbors);
    QVector<QVector<Triangle>> getTrianglesfromCatalogue();
};

#endif // STARCATALOGUE_H
