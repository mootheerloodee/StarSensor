#include "imageopeningclosing.h"

#include <QVector>
#include <QImage>
#include <QColor>

ImageOpeningClosing::ImageOpeningClosing(QImage image, int height, int width) {
    this->image = image;
    this->height = height;
    this->width = width;
}

QImage ImageOpeningClosing::dilate(QImage currentimage) {
    QImage dilateImage(width, height, QImage::Format_RGB888);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QColor color = currentimage.pixelColor(x, y);
            // Если текущий пиксель белый
            if (color.red() == 255) {
                dilateImage.setPixelColor(x, y, QColor(255, 255, 255));
            } else {
                // Проверяем соседние пиксели
                bool isDilated = false;
                //беру окно размером 5*5
                for (int dy = -2; dy <= 2; ++dy) {
                    for (int dx = -2; dx <= 2; ++dx) {
                        if (dx == 0 && dy == 0) continue; // Пропускаем текущий пиксель
                        int newX = x + dx;
                        int newY = y + dy;
                        if (newX >= 0 && newX < width && newY >= 0 && newY < height) {
                            QColor neighborColor = currentimage.pixelColor(newX, newY);
                            if (neighborColor.red() == 255) {
                                isDilated = true;
                                break;
                            }
                        }
                    }
                    if (isDilated) break;
                }
                dilateImage.setPixelColor(x, y, isDilated ? QColor(255, 255, 255) : QColor(0, 0, 0)); //заполняем все окно белым или черным цветом
            }
        }
    }
    return dilateImage;
}

QImage ImageOpeningClosing::erode(QImage currentimage) {
    QImage erodedImage(width, height, QImage::Format_RGB888);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QColor color = currentimage.pixelColor(x, y);
            // Если текущий пиксель черный
            if (color.red() == 0) {
                erodedImage.setPixelColor(x, y, QColor(0, 0, 0));
            } else {
                // Проверяем соседние пиксели
                bool isEroded = true;
                for (int dy = -2; dy <= 2; ++dy) {
                    for (int dx = -2; dx <= 2; ++dx) {
                        if (dx == 0 && dy == 0) continue; // Пропускаем текущий пиксель
                        int newX = x + dx;
                        int newY = y + dy;
                        if (newX >= 0 && newX < width && newY >= 0 && newY < height) {
                            QColor neighborColor = currentimage.pixelColor(newX, newY);
                            if (neighborColor.red() == 0) {
                                isEroded = false;
                                break;
                            }
                        }
                    }
                    if (!isEroded) break;
                }
                erodedImage.setPixelColor(x, y, isEroded ? QColor(255, 255, 255) : QColor(0, 0, 0)); //заполняем все окно белым или черным цветом
            }
        }
    }
    return erodedImage;
}

QImage ImageOpeningClosing::opening(QImage currentimage) {
    QImage openingImage(width, height, QImage::Format_RGB888);
    //последовательное применение эрозии и дилатации
    openingImage = erode(currentimage);
    openingImage = dilate(openingImage);
    //openingImage = erode(openingImage);
    return openingImage;
}

QImage ImageOpeningClosing::closing(QImage currentimage) {
    QImage closingImage(width, height, QImage::Format_RGB888);
    closingImage = dilate(currentimage);
    closingImage = erode(closingImage);
    //closingImage = dilate(closingImage);
    return closingImage;
}

QImage ImageOpeningClosing::getOCImage() {
    QImage openingClosingImage(width, height, QImage::Format_RGB888);
    //сначала размыкание потом замыкание
    openingClosingImage = opening(image);
    //openingClosingImage = closing(openingClosingImage);
    return openingClosingImage;
}
