#include "BoardCell.h"
#include <QDebug>
#include <QMimeData>
#include <QPixmap>

BoardCell::BoardCell(QWidget* parent) : QLabel(parent) {
    setAcceptDrops(true);
    setStyleSheet("border: 2px solid black;"); 
}

void BoardCell::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasFormat("application/x-card")) {
        event->acceptProposedAction();
    }
}

void BoardCell::dropEvent(QDropEvent* event) {
    if (event->mimeData()->hasFormat("application/x-card")) {
        QByteArray byteArray = event->mimeData()->data("application/x-card");
        QPixmap pixmap;
        pixmap.loadFromData(byteArray, "PNG");

        setPixmap(pixmap.scaled(100, 150, Qt::KeepAspectRatio));
        event->acceptProposedAction();

        emit cardPlaced(event, this);

    }
}
