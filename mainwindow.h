#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QComboBox>
#include <QTextEdit>

class ClickablePixmapItem;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override; // Добавьте это объявление

private slots:
    void loadImage(); // Загрузка изображения
    void onImageClicked(const QPointF &pos); // Обработка клика по изображению
    void onImageTypeChanged(int index); // Изменение типа отображаемого изображения (обработка)
    void onIdentifiedImageTypeChanged(int index); // Изменение типа отображаемого изображения (идентификация)

private:
    void displayImage(const QImage &image, const QString &type); // Отображение изображения в QGraphicsView
    void adjustImageViewSizes(); // Автоматическое масштабирование изображений
    void updateStarInformation(); // Обновление информации о звездах

    // Элементы UI
    QGraphicsScene *imageScene; // Сцена для исходного изображения
    QGraphicsScene *processedImageScene; // Сцена для обработанного изображения
    QGraphicsScene *identifiedImageScene; // Сцена для идентифицированных звезд

    QGraphicsView *imageView; // Виджет для исходного изображения
    QGraphicsView *processedImageView; // Виджет для обработанного изображения
    QGraphicsView *identifiedImageView; // Виджет для идентифицированных звезд

    QTextEdit *starInfoText; // Поле для вывода информации о звезде

    QComboBox *imageTypeComboBox; // Выбор типа изображения на вкладке "Обработка"
    QComboBox *identifiedImageTypeComboBox; // Выбор типа изображения на вкладке "Идентификация"

    // Хранилище изображений
    QImage originalImage; // Исходное изображение
    QImage binarizedImage; // Бинаризованное изображение
    QImage medianFilterImage; // Изображение после медианного фильтра
    QImage openingClosingImage; // Изображение после размыкания-замыкания
    QImage markedImage; // Размеченное изображение
    QImage imageWithTriangles; // Изображение с треугольниками
    QImage identifiedImage; // Изображение с идентифицированными звездами
    QImage kNNImage; // Изображение с неверно идентифицированными звездами
    QImage reidentifiedImage; //Изображение с верно идентифицированными звездами
};

#endif // MAINWINDOW_H
