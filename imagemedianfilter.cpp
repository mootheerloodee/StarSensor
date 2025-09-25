#include "imagemedianfilter.h"

#include <QVector>
#include <QImage>
#include <QColor>
#include <algorithm>

ImageMedianFilter::ImageMedianFilter(QImage image, int height, int width) {
    this->image = image;
    this->height = height;
    this->width = width;
}

QVector<int> ImageMedianFilter::getPixelAndNeighbors(int x, int y) {
    QVector<int> pixelAndNeighbors(9);
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            int newX = x + j;
            int newY = y + i;

            // Проверяем границы изображения
            if (newX >= 0 && newX < width && newY >= 0 && newY < height) {
                QColor color(image.pixel(newX, newY));
                int average = (color.red() + color.green() + color.blue()) / 3;
                pixelAndNeighbors[(i + 1) * 3 + (j + 1)] = average; //заполняем массив пикселем и его соседями
            } else {
                pixelAndNeighbors[(i + 1) * 3 + (j + 1)] = 0;
            }
        }
    }

    return pixelAndNeighbors;
}

QImage ImageMedianFilter::getMFImage(){
    QImage medianFilterImage = QImage(width, height, QImage::Format_RGB888);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QVector<int> pixelAndNeighbors = getPixelAndNeighbors(x, y);
            std::sort(pixelAndNeighbors.begin(), pixelAndNeighbors.end()); //сортируем массив, чтобы взять из него средний элемент и присвоить пикселю
            QColor newColor = QColor(pixelAndNeighbors[4], pixelAndNeighbors[4], pixelAndNeighbors[4]);
            medianFilterImage.setPixel(x, y, newColor.rgb());
        }
    }
    return medianFilterImage;
}
