#include "mainwindow.h"
#include <QFileDialog>
#include <QImage>
#include <QPainter>
#include <QPen>
#include <QPolygonF>
#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTabWidget>
#include <QComboBox>
#include <QTextEdit>
#include <QLabel>
#include "clickablepixmapitem.h"

// классы обработки изображений
#include "imagebinarization.h"
#include "imagemedianfilter.h"
#include "imageopeningclosing.h"
#include "marking.h"
#include "trianglesonimage.h"
#include "triangle.h"
#include "trianglescatalogue.h"
#include "knn.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    imageScene(new QGraphicsScene(this)),
    processedImageScene(new QGraphicsScene(this)),
    identifiedImageScene(new QGraphicsScene(this)),
    starInfoText(nullptr), // Инициализация
    imageView(nullptr), // Инициализация
    processedImageView(nullptr), // Инициализация
    identifiedImageView(nullptr), // Инициализация
    imageTypeComboBox(nullptr), // Инициализация
    identifiedImageTypeComboBox(nullptr) // Инициализация
{
    resize(1200, 800);

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Заголовок
    QLabel *titleLabel = new QLabel("Звездный датчик", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setObjectName("titleLabel");
    mainLayout->addWidget(titleLabel);

    // Кнопка загрузки изображения
    QPushButton *loadImageButton = new QPushButton("Загрузить изображение", this);
    loadImageButton->setObjectName("loadImageButton");
    loadImageButton->setFixedWidth(400); // Фиксированная ширина
    loadImageButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mainLayout->addWidget(loadImageButton, 0, Qt::AlignCenter);

    // Вкладки
    QTabWidget *tabWidget = new QTabWidget(this);
    tabWidget->setObjectName("tabWidget");
    tabWidget->setTabPosition(QTabWidget::North); // Вкладки сверху
    tabWidget->setDocumentMode(true); // Современный вид
    tabWidget->setMovable(false); // Запрет перетаскивания вкладок
    mainLayout->addWidget(tabWidget);

    // Вкладка "Главное меню"
    QWidget *mainMenuTab = new QWidget();
    tabWidget->addTab(mainMenuTab, "Главное меню");

    QHBoxLayout *mainMenuLayout = new QHBoxLayout(mainMenuTab);
    mainMenuLayout->setContentsMargins(0, 0, 0, 0); // Убираем отступы
    mainMenuLayout->setSpacing(10); // Устанавливаем межэлементные отступы

    imageView = new QGraphicsView(this);
    imageView->setScene(imageScene);
    imageView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    imageView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    imageView->setAlignment(Qt::AlignCenter);

    // Установите политику растягивания для imageView
    QSizePolicy sizePolicy = imageView->sizePolicy();
    sizePolicy.setHorizontalStretch(1); // Растягивание по горизонтали
    sizePolicy.setVerticalStretch(1); // Растягивание по вертикали
    imageView->setSizePolicy(sizePolicy);

    mainMenuLayout->addWidget(imageView); // Добавляем imageView в левую часть

    starInfoText = new QTextEdit(this);
    starInfoText->setReadOnly(true);
    starInfoText->setFixedWidth(200); // Устанавливаем фиксированную ширину для панели
    starInfoText->setObjectName("starInfoText");
    mainMenuLayout->addWidget(starInfoText); // Добавляем starInfoText в правую часть

    // Подключение события клика по изображению
    ClickablePixmapItem *pixmapItem = new ClickablePixmapItem(QPixmap::fromImage(originalImage));
    imageScene->addItem(pixmapItem);
    connect(pixmapItem, &ClickablePixmapItem::clicked, this, &MainWindow::onImageClicked);

    // Вкладка "Обработка изображения"
    QWidget *processingTab = new QWidget();
    QVBoxLayout *processingLayout = new QVBoxLayout(processingTab);
    processingLayout->setContentsMargins(0, 0, 0, 0); // Убираем отступы
    processingLayout->setSpacing(10); // Устанавливаем межэлементные отступы

    processedImageView = new QGraphicsView(this);
    processedImageView->setScene(processedImageScene);
    processingLayout->addWidget(processedImageView);

    imageTypeComboBox = new QComboBox(this);
    imageTypeComboBox->addItem("Загруженное изображение");
    imageTypeComboBox->addItem("Бинаризованное изображение");
    imageTypeComboBox->addItem("Медианный фильтр");
    imageTypeComboBox->addItem("Размыкание-замыкание");
    imageTypeComboBox->addItem("Разметка");
    imageTypeComboBox->setObjectName("imageTypeComboBox");
    processingLayout->addWidget(imageTypeComboBox);

    tabWidget->addTab(processingTab, "Обработка");

    // Вкладка "Идентификация звезд"
    QWidget *identificationTab = new QWidget();
    QVBoxLayout *identificationLayout = new QVBoxLayout(identificationTab);
    identificationLayout->setContentsMargins(0, 0, 0, 0); // Убираем отступы
    identificationLayout->setSpacing(10); // Устанавливаем межэлементные отступы

    identifiedImageView = new QGraphicsView(this);
    identifiedImageView->setScene(identifiedImageScene);
    identificationLayout->addWidget(identifiedImageView);

    identifiedImageTypeComboBox = new QComboBox(this);
    identifiedImageTypeComboBox->addItem("Загруженное изображение");
    identifiedImageTypeComboBox->addItem("Полученные треугольники");
    identifiedImageTypeComboBox->addItem("Первая идентификация");
    identifiedImageTypeComboBox->addItem("Неверно идентифицированные звезды");
    identifiedImageTypeComboBox->addItem("Верно идентифицированные звезды");
    identifiedImageTypeComboBox->setObjectName("identifiedImageTypeComboBox");
    identificationLayout->addWidget(identifiedImageTypeComboBox);

    tabWidget->addTab(identificationTab, "Идентификация");

    setCentralWidget(centralWidget);

    connect(loadImageButton, &QPushButton::clicked, this, &MainWindow::loadImage);
    connect(imageTypeComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::onImageTypeChanged);
    connect(identifiedImageTypeComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::onIdentifiedImageTypeChanged);
}

MainWindow::~MainWindow() {
    delete imageScene;
    delete processedImageScene;
    delete identifiedImageScene;
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    adjustImageViewSizes();
}

void MainWindow::adjustImageViewSizes() {
    if (!imageScene->sceneRect().isNull()) {
        imageView->fitInView(imageScene->sceneRect(), Qt::KeepAspectRatio);
    }
    if (!processedImageScene->sceneRect().isNull()) {
        processedImageView->fitInView(processedImageScene->sceneRect(), Qt::KeepAspectRatio);
    }
    if (!identifiedImageScene->sceneRect().isNull()) {
        identifiedImageView->fitInView(identifiedImageScene->sceneRect(), Qt::KeepAspectRatio);
    }
}

void MainWindow::loadImage() {
    QString filePath = QFileDialog::getOpenFileName(this, "Выберите изображение", "", "Images (*.png *.jpg *.bmp)");
    if (!filePath.isEmpty()) {
        QImage image(filePath);
        if (!image.isNull()) {
            originalImage = image;

            // Очистка сцен
            imageScene->clear();
            processedImageScene->clear();
            identifiedImageScene->clear();

            // Сброс комбобоксов
            imageTypeComboBox->setCurrentIndex(0); // "Загруженное изображение"
            identifiedImageTypeComboBox->setCurrentIndex(0); // "Загруженное изображение"

            // Создаем кликабельный элемент
            ClickablePixmapItem *pixmapItem = new ClickablePixmapItem(QPixmap::fromImage(originalImage));
            imageScene->addItem(pixmapItem);

            // Подключаем сигнал clicked к слоту onImageClicked
            connect(pixmapItem, &ClickablePixmapItem::clicked, this, &MainWindow::onImageClicked);

            // Отображаем исходное изображение
            displayImage(originalImage, "original");
            displayImage(originalImage, "originalIP");
            displayImage(originalImage, "originalIS");

            qDebug() << "фото загружено";

            // Продолжаем обработку изображения...
            ImageBinarization imageBinarization(image, image.height(), image.width());
            binarizedImage = imageBinarization.getBinImage();

            ImageMedianFilter imageMedianFilter(binarizedImage, image.height(), image.width());
            medianFilterImage = imageMedianFilter.getMFImage();

            ImageOpeningClosing imageOpeningClosing(medianFilterImage, image.height(), image.width());
            openingClosingImage = imageOpeningClosing.getOCImage();

            Marking marking(openingClosingImage, image.height(), image.width());
            QPair<QImage, QVector<StarImage>> markingData = marking.getMarkedImage();
            markedImage = markingData.first;
            QVector<StarImage> stars = markingData.second;

            TrianglesOnImage trianglesOnImage(stars);
            QVector<Triangle> triangles = trianglesOnImage.createTriangles();
            imageWithTriangles = openingClosingImage.copy();
            QPainter painter(&imageWithTriangles);
            QPen pen;
            pen.setColor(QColor(255, 255, 255)); // Белый цвет для треугольников
            pen.setWidth(2); // Толщина линий
            painter.setPen(pen);

            for (const Triangle& triangle : triangles) {
                QPolygonF polygon;
                QPointF p1(triangle.star1.x, triangle.star1.y);
                QPointF p2(triangle.star2.x, triangle.star2.y);
                QPointF p3(triangle.star3.x, triangle.star3.y);
                polygon << p1 << p2 << p3;
                painter.drawPolygon(polygon);
            }

            qDebug() << "треугольники отрисованы";

            // Первичная идентификация
            TrianglesCatalogue trianglesCatalogue("/Users/penk/Desktop/catalog_4.5.txt");

            double eps = 0.01;
            stars = trianglesCatalogue.getHips(triangles, stars, eps);

            for (int i = 0; i < stars.size(); ++i) {
                qDebug() << "для звезды номер" << i << "HIP" << stars[i].hip << stars[i].votes;
            }

            // Создание изображений с подписями HIP
            identifiedImage = openingClosingImage.copy();
            {
            QPainter painter1(&identifiedImage);
            painter1.setPen(QPen(Qt::white, 2));
            painter1.setFont(QFont("Arial", 30)); // Указание размера шрифта
            QFontMetrics fm(painter1.font()); // Метрики для расчета размера текста

            for (const StarImage& star : stars) {
                QPointF pos(star.x, star.y);
                QString text = QString("HIP%1").arg(star.hip);

                int textWidth = fm.horizontalAdvance(text);
                int textHeight = fm.height();

                // Проверка правой границы
                if (pos.x() + textWidth > identifiedImage.width()) {
                    pos.setX(pos.x() - textWidth); // Смещаем влево
                }
                // Проверка верхней границы
                if (pos.y() + textHeight > identifiedImage.height()) {
                    pos.setY(pos.y() - textHeight); // Смещаем вниз
                }

                painter1.drawText(pos.x(), pos.y(), text);
            }
            }

            qDebug() << "первичная идентификация проведена";

            KNN kNN("/Users/penk/Desktop/учеба/нир 6 сем/StarSensor/catalogue.csv");
            double maxAngle = qDegreesToRadians(20); // Максимальный угол обзора
            QVector<StarImage> incorrectlyIdentifiedStars = kNN.getIncorrectlyIdentifiedStars(stars, maxAngle);
            //qDebug() << refinedStars.size() << "звезд определены неверно";

            // Создаем изображение с выделенными неверными звездами
            kNNImage = openingClosingImage.copy();

            {
            QPainter painter2(&kNNImage);
            painter2.setPen(QPen(Qt::red, 2));
            painter2.setFont(QFont("Arial", 30)); // Указание размера шрифта
            QFontMetrics fm(painter2.font());

            for (const StarImage& star : incorrectlyIdentifiedStars) {
                QPointF pos(star.x, star.y);
                QString text = QString("HIP%1").arg(star.hip);

                int textWidth = fm.horizontalAdvance(text);
                int textHeight = fm.height();

                // Проверка правой границы
                if (pos.x() + textWidth > kNNImage.width()) {
                    pos.setX(pos.x() - textWidth); // Смещаем влево
                }
                // Проверка верхней границы
                if (pos.y() + textHeight > kNNImage.height()) {
                    pos.setY(pos.y() - textHeight); // Смещаем вниз
                }

                painter2.drawText(pos.x(), pos.y(), text);
            }
            }

            qDebug() << "неверно идентифицированные звезды найдены";

            QVector<StarImage> reidentifiedStars = kNN.improveIdentification(stars, incorrectlyIdentifiedStars);
            // Создание изображения с выделением неверно идентифицированных звезд
            reidentifiedImage = openingClosingImage.copy();

            {
            QPainter painter3(&reidentifiedImage);
            painter3.setPen(QPen(Qt::white, 2));
            painter3.setFont(QFont("Arial", 30)); // Указание размера шрифта
            QFontMetrics fm(painter3.font());

            for (const StarImage& star : reidentifiedStars) {
                QPointF pos(star.x, star.y);
                QString text = QString("HIP%1").arg(star.hip);

                int textWidth = fm.horizontalAdvance(text);
                int textHeight = fm.height();

                // Проверка правой границы
                if (pos.x() + textWidth > reidentifiedImage.width()) {
                    pos.setX(pos.x() - textWidth); // Смещаем влево
                }
                // Проверка верхней границы
                if (pos.y() + textHeight > reidentifiedImage.height()) {
                    pos.setY(pos.y() - textHeight); // Смещаем вниз
                }

                painter3.drawText(pos.x(), pos.y(), text);
            }
            }



            qDebug() << "метод повышения точности проведен";

            updateStarInformation();
        }
    }
}


void MainWindow::onImageClicked(const QPointF &pos) {
    qDebug() << "Клик по изображению:" << pos;

}

void MainWindow::onImageTypeChanged(int index) {
    switch (index) {
    case 0: // Загруженное изображение
        displayImage(originalImage, "originalIP");
        break;
    case 1: // Бинаризованное изображение
        displayImage(binarizedImage, "binarized");
        break;
    case 2: // Медианный фильтр
        displayImage(medianFilterImage, "median");
        break;
    case 3: // Размыкание-замыкание
        displayImage(openingClosingImage, "openingClosing");
        break;
    case 4: // Разметка треугольников
        displayImage(markedImage, "marked");
        break;
    }
}

void MainWindow::onIdentifiedImageTypeChanged(int index) {
    switch (index) {
    case 0: // Загруженное изображение
        displayImage(originalImage, "originalIS");
        break;
    case 1: // Полученные треугольники
        displayImage(imageWithTriangles, "triangles");
        break;
    case 2: // Первая идентификация
        displayImage(identifiedImage, "identified");
        break;
    case 3: // Неверно идентифицированные звезды
        displayImage(kNNImage, "kNN");
        break;
    case 4: // Верно идентифицированные звезды
        displayImage(reidentifiedImage, "reidentified");
        break;
    }
}

void MainWindow::displayImage(const QImage &image, const QString &type) {
    if (image.isNull()) {
        qCritical() << "Ошибка: Передано пустое изображение.";
        return;
    }

    if (type == "original") {
        imageScene->clear();
        QPixmap pixmap = QPixmap::fromImage(image);
        if (pixmap.isNull()) {
            qCritical() << "Ошибка: Не удалось создать QPixmap из изображения.";
            return;
        }
        imageScene->addPixmap(pixmap);
        imageView->fitInView(imageScene->sceneRect(), Qt::KeepAspectRatio);
    } else if (type == "originalIP") {
        processedImageScene->clear();
        QPixmap pixmap = QPixmap::fromImage(image);
        if (pixmap.isNull()) {
            qCritical() << "Ошибка: Не удалось создать QPixmap из изображения.";
            return;
        }
        processedImageScene->addPixmap(pixmap);
        processedImageView->fitInView(processedImageScene->sceneRect(), Qt::KeepAspectRatio);
    } else if (type == "binarized") {
        processedImageScene->clear();
        QPixmap pixmap = QPixmap::fromImage(image);
        if (pixmap.isNull()) {
            qCritical() << "Ошибка: Не удалось создать QPixmap из изображения.";
            return;
        }
        processedImageScene->addPixmap(pixmap);
        processedImageView->fitInView(processedImageScene->sceneRect(), Qt::KeepAspectRatio);
    } else if (type == "median") {
        processedImageScene->clear();
        QPixmap pixmap = QPixmap::fromImage(image);
        if (pixmap.isNull()) {
            qCritical() << "Ошибка: Не удалось создать QPixmap из изображения.";
            return;
        }
        processedImageScene->addPixmap(pixmap);
        processedImageView->fitInView(processedImageScene->sceneRect(), Qt::KeepAspectRatio);
    } else if (type == "openingClosing") {
        processedImageScene->clear();
        QPixmap pixmap = QPixmap::fromImage(image);
        if (pixmap.isNull()) {
            qCritical() << "Ошибка: Не удалось создать QPixmap из изображения.";
            return;
        }
        processedImageScene->addPixmap(pixmap);
        processedImageView->fitInView(processedImageScene->sceneRect(), Qt::KeepAspectRatio);
    } else if (type == "marked") {
        processedImageScene->clear();
        QPixmap pixmap = QPixmap::fromImage(image);
        if (pixmap.isNull()) {
            qCritical() << "Ошибка: Не удалось создать QPixmap из изображения.";
            return;
        }
        processedImageScene->addPixmap(pixmap);
        processedImageView->fitInView(processedImageScene->sceneRect(), Qt::KeepAspectRatio);
    } else if (type == "originalIS") {
        identifiedImageScene->clear();
        QPixmap pixmap = QPixmap::fromImage(image);
        if (pixmap.isNull()) {
            qCritical() << "Ошибка: Не удалось создать QPixmap из изображения.";
            return;
        }
        identifiedImageScene->addPixmap(pixmap);
        identifiedImageView->fitInView(identifiedImageScene->sceneRect(), Qt::KeepAspectRatio);
    } else if (type == "triangles") {
        identifiedImageScene->clear();
        QPixmap pixmap = QPixmap::fromImage(image);
        if (pixmap.isNull()) {
            qCritical() << "Ошибка: Не удалось создать QPixmap из изображения.";
            return;
        }
        identifiedImageScene->addPixmap(pixmap);
        identifiedImageView->fitInView(identifiedImageScene->sceneRect(), Qt::KeepAspectRatio);
    } else if (type == "identified") {
        identifiedImageScene->clear();
        QPixmap pixmap = QPixmap::fromImage(image);
        if (pixmap.isNull()) {
            qCritical() << "Ошибка: Не удалось создать QPixmap из изображения.";
            return;
        }
        identifiedImageScene->addPixmap(pixmap);
        identifiedImageView->fitInView(identifiedImageScene->sceneRect(), Qt::KeepAspectRatio);
    } else if (type == "kNN") {
        identifiedImageScene->clear();
        QPixmap pixmap = QPixmap::fromImage(image);
        if (pixmap.isNull()) {
            qCritical() << "Ошибка: Не удалось создать QPixmap из изображения.";
            return;
        }
        identifiedImageScene->addPixmap(pixmap);
        identifiedImageView->fitInView(identifiedImageScene->sceneRect(), Qt::KeepAspectRatio);
    } else if (type == "reidentified") {
        identifiedImageScene->clear();
        QPixmap pixmap = QPixmap::fromImage(image);
        if (pixmap.isNull()) {
            qCritical() << "Ошибка: Не удалось создать QPixmap из изображения.";
            return;
        }
        identifiedImageScene->addPixmap(pixmap);
        identifiedImageView->fitInView(identifiedImageScene->sceneRect(), Qt::KeepAspectRatio);
    } else {
        qWarning() << "Неизвестный тип изображения:" << type;
    }
}


void MainWindow::updateStarInformation() {
    // Здесь можно добавить логику для обновления информации о звездах
    // Например:
    // starInfoText->setText(QString("Количество звезд: %1").arg(stars.size()));
}
