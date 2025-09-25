#include "clickablepixmapitem.h"

ClickablePixmapItem::ClickablePixmapItem(const QPixmap &pixmap, QGraphicsItem *parent)
    : QObject(nullptr), // Устанавливаем родителя QObject как nullptr
    QGraphicsPixmapItem(pixmap, parent) {} // Передаем родителя только в QGraphicsPixmapItem

void ClickablePixmapItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked(event->pos()); // Передаем координаты клика
    }
    QGraphicsPixmapItem::mousePressEvent(event); // Вызываем базовую реализацию
}
