#include "BoardCell.h"
#include <QDebug>
#include <QMimeData>
#include <QPixmap>
#include "CardLabel.h"

BoardCell::BoardCell(QWidget* parent) : QLabel(parent) {
    setAcceptDrops(true);
    setStyleSheet("border: 2px solid black;"); 
}

void BoardCell::dropEvent(QDropEvent* event) {
    if (event->mimeData()->hasFormat("application/x-qt-cardlabel")) {
        CardLabel* sourceCard = qobject_cast<CardLabel*>(event->source());
        if (sourceCard) {
            // Folosim pixmap() în loc să dereferențiem
            setPixmap(sourceCard->pixmap(Qt::ReturnByValue));
            event->acceptProposedAction();
            emit cardPlaced(event, this);
        }
    }
    else {
        event->ignore();
    }
}

void BoardCell::dropEvent(QDropEvent* event) {
    if (event->mimeData()->hasFormat("application/x-card")) {
        QByteArray byteArray = event->mimeData()->data("application/x-card");
        QPixmap pixmap;
        pixmap.loadFromData(byteArray, "PNG");

        // Extragem valoarea cărții din datele MIME
        Card::Value cardValue = static_cast<Card::Value>(event->mimeData()->data("card-value").toInt());

        setPixmap(pixmap.scaled(100, 150, Qt::KeepAspectRatio));
        event->acceptProposedAction();

        emit cardPlaced(event, this, cardValue);

    }
}
//void BoardCell::dropEvent(QDropEvent* event) {
//    if (event->mimeData()->hasFormat("application/x-qt-cardlabel")) {
//        event->acceptProposedAction();
//        emit cardPlaced(event, this);
//    }
//    else {
//        event->ignore();
//    }
//}