#ifndef IMAGEOPENINGCLOSING_H
#define IMAGEOPENINGCLOSING_H

#include <QVector>
#include <QImage>
#include <QColor>

class ImageOpeningClosing
{
private:
    QImage image;
    int height;
    int width;
    int bytesPerPixel;
public:
    ImageOpeningClosing(QImage image, int height, int width);

    QImage dilate(QImage currentimage);
    QImage erode(QImage currentimage);
    QImage opening(QImage currentimage);
    QImage closing(QImage currentimage);
    QImage getOCImage();
};

#endif // IMAGEOPENINGCLOSING_H
