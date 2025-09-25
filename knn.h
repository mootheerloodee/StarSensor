#ifndef KNN_H
#define KNN_H
#include <QPoint>
#include <QVector>

#include "starimage.h"

struct Star {
    int hip;
    double ra;
    double dec;
    Star() : hip(0), ra(0.0), dec(0.0) {}
    Star(int h, double r, double d) : hip(h), ra(r), dec(d) {}
};

// struct NeighborImage {
//     double distance_sq;
//     StarImage star;
// };

struct NeighborImage {
    double distance;
    StarImage star;
};


// struct NeighborImage {
//     double distance_sq;
//     StarImage star;
// };


class KNN
{
public:
    KNN(QString filename);
    QVector<StarImage> getIncorrectlyIdentifiedStars(QVector<StarImage> stars, double maxAngle);
    QVector<StarImage> improveIdentification(const QVector<StarImage>& stars, const QVector<StarImage>& incorrectlyIdentifiedStars);
private:
    QVector<Star> catalogue;
    QVector<Star> readCatalogue(QString filename);
    Star findStar(StarImage star);
    double calculateMaxDistance(const QVector<StarImage>& stars, double pixelScale);
    double calculateMaxCatalogDistance();
    double getRadOnPixel(const QVector<StarImage>& stars, const QVector<StarImage>& incorrectedStars);
};

#endif // KNN_H
