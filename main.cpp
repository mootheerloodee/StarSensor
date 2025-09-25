#include <QApplication>
#include <QWidget>

#include "mainwindow.h"

#include <iostream>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
#include "main.moc"
