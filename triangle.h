#ifndef TRIANGLE_H
#define TRIANGLE_H
#include <QPoint>
#include "starimage.h"

class Triangle {
public:
    StarImage star1;
    StarImage star2;
    StarImage star3;

    int i;
    int j;
    int k;

    double p;
    double q;

    Triangle(StarImage s1, StarImage s2, StarImage s3, int a, int b, int c);
    QPair<double, double> getRatioOfSides();
};

#endif // TRIANGLE_H
