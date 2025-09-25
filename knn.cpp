#include "knn.h"
#include "starimage.h"

#include <QPoint>
#include <algorithm>
#include <cmath>
#include <QVector>
#include <QDebug>
#include <QString>
#include <QFile>
#include <QLocale>

KNN::KNN(QString filename) {
    this->catalogue = readCatalogue(filename);
}


double degreesToRadians(double degrees) {
    return degrees * M_PI / 180.0;
}

double hoursToRadians(double hours) {
    return hours * M_PI / 12.0;
}

QVector<Star> KNN::readCatalogue(QString filename) {
    QVector<Star> catalogueData;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Не удалось открыть файл:" << filename;
        return catalogueData; // Возвращаем пустой вектор в случае ошибки
    }
    QTextStream in(&file);
    in.setLocale(QLocale::C);
    bool firstLine = true;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList values = line.split(";");
        if (values.size() >= 12) {
            if (firstLine) {
                firstLine = false;
                continue;
            }
            QString HIP = values.at(1).trimmed();
            QString raStr = values.at(7).trimmed();
            QString decStr = values.at(8).trimmed();
            //QString Mag = values.at(13).trimmed();

            // Агрессивная очистка: удаляем все пробельные символы, кавычки и заменяем запятые на точки
            raStr = raStr.remove('"').replace(",", ".").simplified();
            decStr = decStr.remove('"').replace(",", ".").simplified();

            bool okRA, okDec;
            double RA = raStr.toDouble(&okRA);
            double Dec = decStr.toDouble(&okDec);
            int hipValue = HIP.toInt();

            if (!okRA) {
                qDebug() << "Ошибка преобразования RA в число:" << line << ", RA string: '" << raStr << "'";
            }
            if (!okDec) {
                qDebug() << "Ошибка преобразования Dec в число:" << line << ", Dec string: '" << decStr << "'";
            }

            if (okRA && okDec) {
                // **Создаем объект Star и добавляем его в catalogueData**
                double RaRad = hoursToRadians(RA);
                double DecRad = degreesToRadians(Dec);
                Star star(hipValue, RaRad, DecRad);
                catalogueData.append(star);
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

//считает угол между двумя векторами
double calculateAngles(Star star1, Star star2) {
    // Преобразуем RA и DEC в декартовы координаты
    double x1 = cos(star1.dec) * cos(star1.ra);
    double y1 = cos(star1.dec) * sin(star1.ra);
    double z1 = sin(star1.dec);

    double x2 = cos(star2.dec) * cos(star2.ra);
    double y2 = cos(star2.dec) * sin(star2.ra);
    double z2 = sin(star2.dec);

    // Вычисляем скалярное произведение
    double dotProduct = x1 * x2 + y1 * y2 + z1 * z2;
    //double lenProduct = std::sqrt(x1 * x1 + y1 * y1 + z1 * z1) * std::sqrt(x2 * x2 + y2 * y2 + z2 * z2);
    //double res = dotProduct / lenProduct;

    // Нормализуем результат (ограничиваем диапазон [-1, 1] для acos)
    dotProduct = std::max(-1.0, std::min(1.0, dotProduct));

    // Вычисляем угол в радианах
    return acos(dotProduct);
}

//проверяем два рпотивоположных угла при заданной звезде star1
bool checkAngles(Star star1, Star star2, Star star3, double maxAngle) {
    double angle1 = calculateAngles(star1, star2);
    double angle2 = calculateAngles(star1, star3);

    // Проверяем, что оба угла меньше или равны maxAngle
    return (angle1 <= maxAngle && angle2 <= maxAngle);
}

//находит звезду с данным HIP в каталоге
Star KNN::findStar(StarImage star) {
    for (int i = 0; i < catalogue.size(); ++i) {
        if (star.hip == catalogue[i].hip) {
            return catalogue[i];
        }
    }
    return Star{-1, 0, 0}; // Возвращаем пустую звезду, если не найдено
}

QVector<StarImage> KNN::getIncorrectlyIdentifiedStars(QVector<StarImage> stars, double maxAngleRad) {
    int n = stars.size();
    QVector<bool> isIncorrect(n, false); // Флаги для пометки неверных звезд

    for (int indexOfCurStar = 0; indexOfCurStar < n; ++indexOfCurStar) {
        int count = 0;
        Star star1 = findStar(stars[indexOfCurStar]);
        if (star1.hip == -1) continue; // Пропускаем звёзды, которых нет в каталоге

        bool hasCorrectTriangle = false; // Флаг для проверки наличия хотя бы одного правильного треугольника

        for (int indexOfNeib1 = 0; indexOfNeib1 < n; ++indexOfNeib1) {
            if (indexOfNeib1 == indexOfCurStar) continue; // Пропускаем текущую звезду
            Star star2 = findStar(stars[indexOfNeib1]);
            if (star2.hip == -1) continue;

            for (int indexOfNeib2 = 0; indexOfNeib2 < n; ++indexOfNeib2) {
                if (indexOfNeib2 == indexOfCurStar || indexOfNeib2 == indexOfNeib1) continue;
                Star star3 = findStar(stars[indexOfNeib2]);
                if (star3.hip == -1) continue;

                // Проверяем углы при звезде star1
                if (checkAngles(star1, star2, star3, maxAngleRad)) {
                    hasCorrectTriangle = true; // Нашли хотя бы один правильный треугольник
                    break;
                }
            }
            if (hasCorrectTriangle) {
                break; // Если нашли правильный треугольник, переходим к следующей звезде
            }
        }

        if (!hasCorrectTriangle) {
            isIncorrect[indexOfCurStar] = true; // Помечаем звезду как неверно идентифицированную
        }
    }
    // Создаем новый массив с неверно идентифицированными звездами
    QVector<StarImage> incorrectlyIdentifiedStars;
    for (int i = 0; i < n; ++i) {
        if (isIncorrect[i]) {
            incorrectlyIdentifiedStars.append(stars[i]);
        }
    }

    return incorrectlyIdentifiedStars;
}

double sphericalDistance(double ra1, double dec1, double ra2, double dec2) {
    double dlon = ra2 - ra1;
    double dlat = dec2 - dec1;
    double a = std::sin(dlat/2)*std::sin(dlat/2) +
               std::cos(dec1)*std::cos(dec2)*
                   std::sin(dlon/2)*std::sin(dlon/2);
    return 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
}

QVector<NeighborImage> findNeighbors(const StarImage& currentStar,
                                     const QVector<StarImage>& stars,
                                     const QSet<int>& incorrectIndexes) {
    QVector<NeighborImage> neighbors;
    for (const auto& other : stars) {
        if (other.i == currentStar.i || incorrectIndexes.contains(other.i)) continue;
        double dx = other.x - currentStar.x;
        double dy = other.y - currentStar.y;
        double distance = std::sqrt(dx*dx + dy*dy);
        neighbors.append({distance, other});
    }
    std::sort(neighbors.begin(), neighbors.end(),
              [](const NeighborImage& a, const NeighborImage& b) {
                  return a.distance < b.distance;
              });
    return neighbors;
}

QVector<StarImage> KNN::improveIdentification(
    const QVector<StarImage>& stars,
    const QVector<StarImage>& incorrectlyIdentifiedStars
    ) {
    QVector<StarImage> reidentifiedStars = stars;
    QSet<int> incorrectIndexes;
    for (const auto& s : incorrectlyIdentifiedStars) {
        incorrectIndexes.insert(s.i);
    }

    QMap<int, int> indexMap;
    for (int idx = 0; idx < reidentifiedStars.size(); ++idx) {
        indexMap[reidentifiedStars[idx].i] = idx;
    }

    for (const auto& incorrectStar : incorrectlyIdentifiedStars) {
        int currentI = incorrectStar.i;
        StarImage& currentStar = reidentifiedStars[indexMap[currentI]];

        // Находим 3 ближайших соседа на снимке
        auto neighbors = findNeighbors(currentStar, stars, incorrectIndexes);
        if (neighbors.size() < 3) {
            currentStar.hip = 0;
            continue;
        }

        // Координаты соседей на снимке и их HIP
        QVector<StarImage> neighborStars;
        for (int i = 0; i < 3; ++i) {
            neighborStars.append(neighbors[i].star);
        }

        // Карта HIP соседей в каталоге
        QHash<int, Star> catalogNeighbors;
        for (const auto& neighbor : neighborStars) {
            for (const auto& catStar : catalogue) {
                if (catStar.hip == neighbor.hip) {
                    catalogNeighbors[neighbor.hip] = catStar;
                    break;
                }
            }
        }

        // Для каждой звезды из каталога, кроме текущей
        QVector<std::pair<double, int>> candidates;
        for (const auto& catStar : catalogue) {
            if (catStar.hip == currentStar.hip) continue;

            double totalError = 0;
            bool valid = true;
            for (int i = 0; i < 3; ++i) {
                StarImage neighborImage = neighborStars[i];
                auto it = catalogNeighbors.find(neighborImage.hip);
                if (it == catalogNeighbors.end()) {
                    valid = false;
                    break;
                }

                Star neighborCat = it.value();
                double sphericalDist = sphericalDistance(
                    catStar.ra, catStar.dec,
                    neighborCat.ra, neighborCat.dec
                    );

                // Расстояние на снимке между текущей звездой и соседом
                double imageDist = neighbors[i].distance;

                // Преобразование сферического расстояния в пиксели
                double pixelSphericalDist = sphericalDist / 0.000368758;

                totalError += std::abs(pixelSphericalDist - imageDist);
            }

            if (valid) {
                candidates.append({totalError, catStar.hip});
            }
        }

        // Сортируем по минимальной ошибке
        std::sort(candidates.begin(), candidates.end(),
                  [](const std::pair<double, int>& a, const std::pair<double, int>& b) {
                      return a.first < b.first;
                  });

        // Порог для ошибки
        const double threshold = 10.0; // Пиксели

        if (!candidates.isEmpty() && candidates[0].first < threshold) {
            currentStar.hip = candidates[0].second;
        } else {
            currentStar.hip = 0;
        }
    }

    return reidentifiedStars;
}
