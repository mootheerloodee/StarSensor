#include "pixelandneighbors.h"
#include <QVector>
#include <QImage>

PixelAndNeighbors::PixelAndNeighbors(QImage binarizationImage, int height, int width) {
    this->binarizationImage = binarizationImage;
    this->height = height;
    this->width = width;
}

//получение массива пикселя и его соседей
QVector<int> PixelAndNeighbors::getPixelAndNeighbors(int x, int y) {
    QVector<int> pixelAndNeighbors(9);

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            int newX = x + j;
            int newY = y + i;

            // Проверяем границы изображения
            if (newX >= 0 && newX < width && newY >= 0 && newY < height) {
                QColor color(binarizationImage.pixel(newX, newY));
                int average = (color.red() + color.green() + color.blue()) / 3;
                pixelAndNeighbors[(i + 1) * 3 + (j + 1)] = average;
            } else {
                pixelAndNeighbors[(i + 1) * 3 + (j + 1)] = 0;
            }
        }
    }

    return pixelAndNeighbors;
}
