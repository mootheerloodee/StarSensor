#include "starcatalogue.h"
#include "triangle.h"
#include "trianglesonimage.h"
#include <QVector>
#include <QDebug>
#include <QFile>


StarCatalogue::StarCatalogue(QString filename) {
    this->filename = filename;
    this->catalogueData = readCatalogue();

    // Проверка на наличие данных
    if (this->catalogueData.isEmpty()) {
        qDebug() << "Каталог звезд пуст!";
    } else {
        qDebug() << "Каталог звезд загружен, количество звезд:" << this->catalogueData.size();
    }

}


QVector<QVector<QString>> StarCatalogue::readCatalogue(){
    QVector<QVector<QString>> catalogueData;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Не удалось открыть файл:" << filename;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList values = line.split(";"); // Разделяем строку по запятой

        QVector<QString> row; // Создаем новый вектор для строки
        for (const QString& value : values) {
            // Пример: предположим, что у нас есть имя и значение, которые мы хотим сохранить
            QString curValue = value.trimmed(); // Убираем пробелы
            // Добавляем объект в строку
            row.append(curValue); // Создайте объект и добавьте в строку
        }
        catalogueData.append(row); // Добавьте строку в основной QVector
    }
    file.close();
    return catalogueData;
}

QVector<QVector<QString>> StarCatalogue::getStarsInView(double alpha0, double delta0) {
    QVector<QVector<QString>> starsInView;
    double halfAngle = 10.0; // Половина угла обзора (20 градусов)

    // Конвертируем половину угла в радианы для вычислений
    double halfAngleRad = qDegreesToRadians(halfAngle);

    // Пропускаем заголовок (первая строка)
    for (int i = 1; i < catalogueData.size(); ++i) {
        double ra = catalogueData[i][7].toDouble(); // RA - 8 столбец (индекс 7)
        double dec = catalogueData[i][8].toDouble(); // Dec - 9 столбец (индекс 8)

        // Проверяем, попадает ли звезда в область обзора
        if (ra >= (alpha0 - halfAngleRad) && ra <= (alpha0 + halfAngleRad) &&
            dec >= (delta0 - halfAngleRad) && dec <= (delta0 + halfAngleRad)) {
            double x = cos(dec) / sin(dec) * sin(ra - alpha0) / (sin(delta0) + cos(dec) / sin(dec) * cos(delta0) * cos(ra - alpha0));
            double y = (cos(delta0) - cos(dec) / sin(dec) * sin(delta0) * cos(ra - alpha0)) / (sin(delta0) + cos(dec) / sin(dec) * cos(delta0) * cos(ra - alpha0));
            catalogueData[i].resize(14);
            catalogueData[i][12] = QString::number(x);
            catalogueData[i][13] = QString::number(y);
            starsInView.append(catalogueData[i]); // Добавляем звезду в список
        }
    }

    return starsInView;
}

QVector<QVector<QVector<QString>>> StarCatalogue::getStarsNeighbors(){
    QVector<QVector<QVector<QString>>> starsNeighbors;
    // Пропускаем заголовок (первая строка)
    for (int i = 1; i < catalogueData.size(); i += 40) {
        double ra = catalogueData[i][7].toDouble(); // RA - 8 столбец (индекс 7)
        double dec = catalogueData[i][8].toDouble(); // Dec - 9 столбец (индекс 8)
        // Получаем соседние звезды для текущей звезды
        QVector<QVector<QString>> neighbors = getStarsInView(ra, dec);
        if (neighbors.size() > 40) {
            qDebug() << "Слишком много соседей для звезды:" << catalogueData[i][0];
        }
        starsNeighbors.append(neighbors); // Сохраняем соседей для текущей звезды
    }
    return starsNeighbors;
}

QVector<QPoint> StarCatalogue::getArrOfStars(QVector<QVector<QString>> starsNeighbors){
    QVector<QPoint> stars;
    for (int i = 0; i < starsNeighbors.size(); ++i){
        QPoint curStar;
        curStar.setX(starsNeighbors[i][12].toInt());
        curStar.setY(starsNeighbors[i][13].toInt());
        stars.append(curStar);
    }
    return stars;
}

QVector<QVector<Triangle>> StarCatalogue::getTrianglesfromCatalogue(){
    QVector<QVector<QVector<QString>>> starsNeighbors = getStarsNeighbors();
    QVector<QVector<Triangle>> triangles;
    for (int i = 0; i < starsNeighbors.size(); ++i){
        QVector<QPoint> stars = getArrOfStars(starsNeighbors[i]);
        TrianglesOnImage trianglesForCurStar(stars);
        QVector<Triangle> curTriangles = trianglesForCurStar.createTriangles();
        triangles.append(curTriangles);
    }
    return triangles;
}

