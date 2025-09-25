#ifndef IMAGEBINARIZATION_H
#define IMAGEBINARIZATION_H

#include <QVector>
#include <QImage>
#include <QColor>

class ImageBinarization {
private:
    QImage image;
    int height;
    int width;

public:
    ImageBinarization(QImage image, int height, int width);

    QVector<int> getArrayOfAvgBrightness();
    int getThreshold(QVector<int> arrayOfAvgBrightness);
    QImage getBinImage();
    //QVector<int> getPixelAndNeighbors(int x, int y);
};
#endif // IMAGEBINARIZATION_H
