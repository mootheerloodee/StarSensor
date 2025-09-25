#include "marking.h"
#include "imagemedianfilter.h"
#include "arrayofpixels.h"
#include "starimage.h"

#include <QVector>
#include <QImage>
#include <QColor>
#include <iostream>
#include <QPainter>
#include <QFont>
#include <QPair>
#include <QPen>
#include <QDebug>



Marking::Marking(QImage image, int height, int width) {
    this->image = image;
    this->height = height;
    this->width = width;
}
QVector<int> Marking::labelObjects() {
    QVector<int> labels(width * height, 0); // Массив для хранения меток
    int currentLabel = 0; // Идентификатор текущей метки

    // Определяем направления (включая диагонали для соединения компактных звезд)
    const int directions[8][2] = {
        {-1,  0}, {1,  0}, {0, -1}, {0,  1}, // горизонтальные и вертикальные
        {-1, -1}, {1,  1}, {-1,  1}, {1, -1} // диагональные
    };

    // Перебираем каждый пиксель изображения
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Проверяем, является ли пиксель белым и ещё не размечен в labels
            if (image.pixelColor(x, y) == QColor(255, 255, 255) && labels[y * width + x] == 0) {
                // Новый объект найден, увеличиваем счетчик меток
                currentLabel++;

                // Используем стек для обхода в глубину (DFS)
                QVector<QPoint> stack;
                stack.push_back(QPoint(x, y));

                while (!stack.isEmpty()) {
                    QPoint point = stack.takeLast();
                    int px = point.x();
                    int py = point.y();

                    // Помечаем текущий пиксель текущей меткой
                    labels[py * width + px] = currentLabel;

                    // Проверяем соседние пиксели
                    for (const auto& dir : directions) {
                        int nx = px + dir[0];
                        int ny = py + dir[1];

                        // Убедимся, что координаты в пределах изображения
                        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                            // Проверяем, что соседний пиксель белый и ещё не размечен
                            if (image.pixelColor(nx, ny) == QColor(255, 255, 255) && labels[ny * width + nx] == 0) {
                                stack.push_back(QPoint(nx, ny));
                            }
                        }
                    }
                }
            }
        }
    }

    return labels; // Возвращаем массив с метками
}


QPair<QImage, QVector<StarImage>> Marking::getMarkedImage() {
    QVector<StarImage> stars; // Для хранения центров звезд (вещественные координаты)
    QVector<int> labels = labelObjects();
    QImage markedImage = image.copy(); // Создаем копию оригинального изображения
    QPainter painter(&markedImage);
    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setPointSize(40); // Размер шрифта для номера
    painter.setFont(font);

    int maxLabel = *std::max_element(labels.begin(), labels.end());
    QVector<QPointF> centers(maxLabel + 1, QPointF(0.0, 0.0)); // Для хранения центров звезд
    QVector<double> totalIntensity(maxLabel + 1, 0.0); // Для хранения общей интенсивности каждой звезды

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = labels[y * width + x];
            if (index > 0) {
                // Получаем интенсивность пикселя (яркость)
                QColor pixelColor = image.pixelColor(x, y);
                double intensity = pixelColor.redF(); // Используем нормализованную яркость (0.0 - 1.0)

                // Накапливаем взвешенные координаты и общую интенсивность
                centers[index] += QPointF(x, y) * intensity;
                totalIntensity[index] += intensity;

                // Закрашиваем пиксели серым, пропорционально индексу
                int colorValue = (255 * index) / maxLabel;
                markedImage.setPixelColor(x, y, QColor(colorValue, colorValue, colorValue));
            }
        }
    }

    // Вычисляем энергетические центры
    for (int i = 1; i <= maxLabel; ++i) {
        if (totalIntensity[i] > 0) {
            centers[i] /= totalIntensity[i]; // Делим сумму взвешенных координат на общую интенсивность
            StarImage star(centers[i].x(), centers[i].y(), i);
            stars.append(star); // Добавляем центр звезды в список
            painter.drawText(centers[i].toPoint(), QString::number(i)); // Номер звезды
            qDebug() << "координаты звезды" << i << stars[i - 1].x << ", " << stars[i - 1].y;
        }
    }

    painter.end();
    return qMakePair(markedImage, stars);
}

QPair<QImage, QPointF> Marking::getSpecificStar(int index, const QVector<QPointF>& stars) {
    QImage newImage(width, height, QImage::Format_RGB888);
    newImage.fill(Qt::black);

    // Получаем вещественные координаты звезды
    QPointF starCenter = stars[index];
    double x = starCenter.x();
    double y = starCenter.y();

    // Отмечаем пиксель в ближайшей целочисленной позиции (округление)
    int roundedX = static_cast<int>(std::round(x));
    int roundedY = static_cast<int>(std::round(y));

    // Устанавливаем белый пиксель в ближайшей позиции
    if (roundedX >= 0 && roundedX < width && roundedY >= 0 && roundedY < height) {
        newImage.setPixelColor(roundedX, roundedY, QColor(255, 255, 255));
    }

    // Рисуем прямоугольник вокруг звезды
    QPainter painter(&newImage);
    QPen pen(QColor(255, 0, 0), 5); // Красный цвет, ширина линии 5 пикселей
    painter.setPen(pen);

    // Определяем границы прямоугольника
    QRectF rect(QPointF(x - 50.0, y - 50.0), QPointF(x + 50.0, y + 50.0));
    painter.drawRect(rect);

    painter.end();

    // Возвращаем изображение и вещественные координаты звезды
    return qMakePair(newImage, starCenter);
}





