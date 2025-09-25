#ifndef CLICKABLEPIXMAPITEM_H
#define CLICKABLEPIXMAPITEM_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>

class ClickablePixmapItem : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    explicit ClickablePixmapItem(const QPixmap &pixmap, QGraphicsItem *parent = nullptr);

signals:
    void clicked(const QPointF &pos); // Сигнал для передачи координат клика

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // CLICKABLEPIXMAPITEM_H
