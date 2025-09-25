#ifndef PIXELANDNEIGHBORS_H
#define PIXELANDNEIGHBORS_H

#include <QImage>
#include <QVector>
#include <QColor>

class PixelAndNeighbors {
private:
    QImage binarizationImage;
    int height;
    int width;

public:
    PixelAndNeighbors(QImage binarizationImage, int height, int width);

    QVector<int> getPixelAndNeighbors(int x, int y);
};


#endif // PIXELANDNEIGHBORS_H
