#include "triangle.h"
#include "starimage.h"

#include <QPoint>
#include <iostream>
#include <algorithm>

#include <QPair>

Triangle::Triangle(StarImage s1, StarImage s2, StarImage s3, int a, int b, int c)
    : star1(s1), star2(s2), star3(s3), i(a), j(b), k(c) {
    QPair<double, double> ratios = getRatioOfSides();
    this->p = ratios.first;
    this->q = ratios.second;
    //this->getRatioOfSides();
}

QPair<double, double> Triangle::getRatioOfSides() {
    double a = std::sqrt((star1.x - star2.x)*(star1.x - star2.x)+(star1.y - star2.y)*(star1.y - star2.y));
    double b = std::sqrt((star1.x - star3.x)*(star1.x - star3.x)+(star1.y - star3.y)*(star1.y - star3.y));
    double c = std::sqrt((star2.x - star3.x)*(star2.x - star3.x)+(star2.y - star3.y)*(star2.y - star3.y));

    double maxSide = std::max({a, b, c});  // Use initializer_list for std::max
    double minSide = std::min({a, b, c});  // Find the minimum side too

    double midSide;  // Declare a variable to store the middle side

    if (maxSide == a) {
        midSide = std::max(b, c);
        this->p = a / midSide;
        this->q = a / minSide;
    } else if (maxSide == b) {
        midSide = std::max(a, c);
        this->p = b / midSide;
        this->q = b / minSide;
    } else {
        midSide = std::max(a, b);
        this->p = c / midSide;
        this->q = c / minSide;
    }

    return qMakePair(this->p, this->q);
}
