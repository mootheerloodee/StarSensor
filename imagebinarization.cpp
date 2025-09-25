#include "imagebinarization.h"

#include <QVector>
#include <QImage>
#include <QColor>
#include "arrayofpixels.h"

ImageBinarization::ImageBinarization(QImage image, int height, int width) {
    this->image = image;
    this->height = height;
    this->width = width;
}

QVector<int> ImageBinarization::getArrayOfAvgBrightness(){
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

int ImageBinarization::getThreshold(QVector<int> pixelAverageBrightness){
    int averageBrightness;
    int size = height * width;
    int sum = 0;
    for (int i = 0; i < size; i++){
        sum += pixelAverageBrightness[i];
    }
    averageBrightness = sum / size;
    return averageBrightness;
}

QImage ImageBinarization::getBinImage(){
    QVector<int> arrayOfAvgBrightness = getArrayOfAvgBrightness();
    //int threshold = getThreshold(arrayOfAvgBrightness);
    int threshold = 40;
    QImage binarizationImage = QImage(width, height, QImage::Format_RGB888);
    //проходимся по каждому пикселю и сравниваем его с порогом яркости
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = y * width + x;
            int brightness = arrayOfAvgBrightness[index];
            QColor newColor = (brightness < threshold) ? QColor(0, 0, 0) : QColor(255, 255, 255);
            binarizationImage.setPixel(x, y, newColor.rgb());
        }
    }
    return binarizationImage;
}

