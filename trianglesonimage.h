#ifndef TRIANGLESONIMAGE_H
#define TRIANGLESONIMAGE_H

#include <QVector>
#include <QImage>
#include "triangle.h"
#include "starimage.h"



class TrianglesOnImage
{
private:
    QVector<StarImage> stars;
public:
    TrianglesOnImage(QVector<StarImage> stars);
    QVector<Triangle> createTriangles();
};



#endif // TRIANGLESONIMAGE_H
