#ifndef MARKING_H
#define MARKING_H

#include <QVector>
#include <QImage>
#include <QColor>

#include "starimage.h"

class Marking
{
public:
    int maxLabel;
private:
    QImage image;
    int height;
    int width;
public:
    Marking(QImage image, int height, int width);

    QVector<int> labelObjects();
    QPair<QImage, QVector<StarImage>> getMarkedImage();
    QPair<QImage, QPointF> getSpecificStar(int index, const QVector<QPointF>& stars);
};

#endif // MARKING_H

