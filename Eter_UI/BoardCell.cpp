#include "BoardCell.h"

BoardCell::BoardCell(QWidget* parent) : QLabel(parent) {
    setFixedSize(100, 100);
    setStyleSheet("border: 2px solid white; background-color: rgba(255, 255, 255, 0.1);");
    setAlignment(Qt::AlignCenter);
    setAcceptDrops(true);
}

void BoardCell::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasFormat("application/x-card")) {  // Verificăm dacă formatul este corect
        event->acceptProposedAction();
    }
}

void BoardCell::dropEvent(QDropEvent* event) {
    if (event->mimeData()->hasFormat("application/x-card")) {
        QByteArray byteArray = event->mimeData()->data("application/x-card");
        QPixmap pixmap;
        pixmap.loadFromData(byteArray, "PNG");

        QLabel* card = new QLabel(this);
        card->setPixmap(pixmap.scaled(100, 150, Qt::KeepAspectRatio));
        card->setGeometry(0, 0, 100, 150);
        card->show();

        event->acceptProposedAction();
    }
}
