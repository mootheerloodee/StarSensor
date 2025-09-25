#ifndef IMAGEMEDIANFILTER_H
#define IMAGEMEDIANFILTER_H
#include <QVector>
#include <QImage>
#include <QColor>
#include <algorithm>

class ImageMedianFilter
{
private:
    QImage image;
    int height;
    int width;
public:
    ImageMedianFilter(QImage image, int height, int width);

    QVector<int> getPixelAndNeighbors(int x, int y);
    QImage getMFImage();
};

#endif // IMAGEMEDIANFILTER_H
