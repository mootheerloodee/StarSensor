#ifndef ARRAYOFPIXELS_H
#define ARRAYOFPIXELS_H
#include <QVector>
#include <QImage>

class ArrayOfPixels
{
private:
    QImage image;
    int height;
    int width;

public:
    ArrayOfPixels(QImage image, int height, int width);

    QVector<int> getArrOfBrightness();
    QVector<int> getArrOfPixels();

};

#endif // ARRAYOFPIXELS_H
