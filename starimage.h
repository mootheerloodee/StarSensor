#ifndef STARIMAGE_H
#define STARIMAGE_H

#include "starimage.h"

class StarImage
{
public:
    StarImage(double x, double y, int i);
    double x;
    double y;
    int i;
    int hip;
    int votes;
};

#endif // STARIMAGE_H
