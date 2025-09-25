#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QFileDialog>
#include <QImage>
#include <QLabel>
#include <QVector>
#include <QVBoxLayout>//Подключает класс вертикального компоновщика, который управляет расположением виджетов в вертикальной колонке.
#include <QDebug>
#include <cmath>
#include <algorithm> //для сортировки вектора
//заголовочный файлы
#include "imagebinarization.h"
#include "imagemedianfilter.h"
#include "imageopeningclosing.h"

#include "mainwindow.h"

#include <iostream>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
#include "main.moc"
