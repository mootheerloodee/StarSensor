#include <QVector>
#include <QImage>
#include <QDebug>
#include "trianglesonimage.h"
#include "marking.h"
#include "triangle.h"

#include "starimage.h"


TrianglesOnImage::TrianglesOnImage(QVector<StarImage> stars) {
    this->stars = stars;
}

QVector<Triangle> TrianglesOnImage::createTriangles() {
    QVector<Triangle> triangles;
    int countOfStars = stars.size(); // Используем размер вектора stars

    if (countOfStars < 3) {
        return triangles;
    } // недостаточно звезд для образования треугольника

    for (int i = 0; i < countOfStars - 2; ++i) { // Исправлено ++i на ++i
        for (int j = i + 1; j < countOfStars - 1; ++j) { // Исправлено i++ на j++
            for (int k = j + 1; k < countOfStars; ++k) { // Исправлено i++ на k++
                // Проверяем, что индексы находятся в допустимом диапазоне
                if (i >= 0 && i < stars.size() &&
                    j >= 0 && j < stars.size() &&
                    k >= 0 && k < stars.size()) {
                    StarImage p1 = stars[i];
                    StarImage p2 = stars[j];
                    StarImage p3 = stars[k];
                    Triangle triangle(p1, p2, p3, i, j, k); // Создаем новый объект Triangle
                    triangles.append(triangle);
                } else {
                    qDebug() << "Ошибка: выход за границы массива stars. i=" << i << ", j=" << j << ", k=" << k << ", stars.size()=" << stars.size();
                }
            }
        }
    }
    return triangles;
}



