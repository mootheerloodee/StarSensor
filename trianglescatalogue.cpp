#include "trianglescatalogue.h"
#include <QString>
#include <QVector>
#include <QFile>
#include <QDebug>
#include <QLocale>
#include <QPair>
#include <QRegularExpression>
#include <cmath>

#include "triangle.h"
#include "trianglefromcatalogue.h"
#include "starimage.h"

TrianglesCatalogue::TrianglesCatalogue(QString filename) {
    this->catalogueData = readCatalogue(filename);

    if (this->catalogueData.isEmpty()) {
        qDebug() << "Каталог треугольников пуст!";
    } else {
        qDebug() << "Каталог треугольников загружен, количество треугольников:" << this->catalogueData.size();
    }
}

QVector<TriangleFromCatalogue> TrianglesCatalogue::readCatalogue(QString filename) {
    QVector<TriangleFromCatalogue> catalogueData;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Не удалось открыть файл:" << filename;
        return catalogueData; // Возвращаем пустой вектор в случае ошибки
    }

    QTextStream in(&file);
    in.setLocale(QLocale::C); // Устанавливаем локаль для корректного преобразования чисел

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed(); // Читаем строку и удаляем лишние пробелы по краям
        if (line.isEmpty()) {
            continue; // Пропускаем пустые строки
        }

        QStringList values = line.split(" ", Qt::SkipEmptyParts); // Разделяем строку по пробелам
        if (values.size() >= 5) {
            QString pStr = values.at(0).trimmed();
            QString qStr = values.at(1).trimmed();
            QString star1Str = values.at(2).trimmed();
            QString star2Str = values.at(3).trimmed();
            QString star3Str = values.at(4).trimmed();

            bool okP, okQ, okStar1, okStar2, okStar3;

            // Преобразуем значения в соответствующие типы данных
            double p = pStr.toDouble(&okP);
            double q = qStr.toDouble(&okQ);
            int star1 = star1Str.toInt(&okStar1);
            int star2 = star2Str.toInt(&okStar2);
            int star3 = star3Str.toInt(&okStar3);

            // Проверяем успешность преобразования
            if (!okP) {
                qDebug() << "Ошибка преобразования RA в число:" << line << ", RA string: '" << pStr << "'";
            }
            if (!okQ) {
                qDebug() << "Ошибка преобразования Dec в число:" << line << ", Dec string: '" << qStr << "'";
            }

            if (okP && okQ && okStar1 && okStar2 && okStar3) {
                // Создаем объект TriangleFromCatalogue и добавляем его в вектор
                TriangleFromCatalogue triangle(p, q, star1, star2, star3);
                catalogueData.append(triangle);
            } else {
                qDebug() << "Не удалось преобразовать RA или Dec в число:" << line;
            }
        } else {
            qDebug() << "Недостаточно столбцов в строке:" << line;
        }
    }

    file.close();
    return catalogueData;
}

int TrianglesCatalogue::getMaxHIP() {
    int size = catalogueData.size();
    int maxHIP = 0;
    int curMaxHIP = 0;
    for (int i = 0; i < size; ++i) {
        curMaxHIP = std::max({catalogueData[i].star1, catalogueData[i].star2, catalogueData[i].star3});
        if (curMaxHIP > maxHIP) {
            maxHIP = curMaxHIP;
        }
    }
    return maxHIP;
}

QVector<StarImage> TrianglesCatalogue::getHips(QVector<Triangle> triangles, QVector<StarImage> stars, double eps) {
    int n = stars.size();
    int maxHIP = getMaxHIP();
    QVector<QVector<int>> votingMatrix(n, QVector<int>(maxHIP + 1));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < maxHIP; ++j) {
            votingMatrix[i][j] = 0;
        }
    }
    int countOfTriangles = triangles.size();
    int catalogueSize = catalogueData.size();
    for (int i = 0; i < countOfTriangles; ++i) {
        for (int j = 0; j < catalogueSize; ++j) {
            if (std::abs(triangles[i].p - catalogueData[j].p) <= eps && std::abs(triangles[i].q - catalogueData[j].q) <= eps) {
                votingMatrix[triangles[i].i][catalogueData[j].star1]++;
                votingMatrix[triangles[i].j][catalogueData[j].star1]++;
                votingMatrix[triangles[i].k][catalogueData[j].star1]++;

                votingMatrix[triangles[i].i][catalogueData[j].star2]++;
                votingMatrix[triangles[i].j][catalogueData[j].star2]++;
                votingMatrix[triangles[i].k][catalogueData[j].star2]++;

                votingMatrix[triangles[i].i][catalogueData[j].star3]++;
                votingMatrix[triangles[i].j][catalogueData[j].star3]++;
                votingMatrix[triangles[i].k][catalogueData[j].star3]++;
            }
        }
    }
    for (int i = 0; i < n; ++i) {
        int curMax = 0;
        int curHIP = -1;
        for (int j = 0; j <= maxHIP; ++j) {
            if (votingMatrix[i][j] > curMax) {
                curMax = votingMatrix[i][j];
                curHIP = j;
            }
        }
        stars[i].votes = curMax;
        stars[i].hip = curHIP;
    }
    return stars;
}



