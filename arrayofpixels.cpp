#include "arrayofpixels.h"
#include <QVector>
#include <QImage>
#include <QColor>

ArrayOfPixels::ArrayOfPixels(QImage image, int width, int height) {
    this->image = image;
    this->width = width;
    this->height = height;
}

QVector<int> ArrayOfPixels::getArrOfBrightness() {
    QVector<int> pixelAverageBrightness(width * height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QColor color = image.pixelColor(x, y);
            int index = (y * width + x);
            int brightness = static_cast<int>(0.2125 *  color.red() + 0.7154 * color.green() + 0.0721 * color.blue()); //яркость текущего пикселя
            pixelAverageBrightness[index] = brightness; //добавляем в массив средних яркостей пикселей
        }
    }
    return pixelAverageBrightness;
}

QVector<int> ArrayOfPixels::getArrOfPixels() {
    QVector<int> pixelData(height * width * 3);
    for(int y = 0; y < height; ++y){
        for(int x = 0; x < width; ++x){
            int index = (y * width + x) * 3;
            QColor color = image.pixelColor(x, y);
            pixelData[index] = color.red();
            pixelData[index + 1] = color.green();
            pixelData[index + 2] = color.blue();
        }
    }
    return pixelData;
}

