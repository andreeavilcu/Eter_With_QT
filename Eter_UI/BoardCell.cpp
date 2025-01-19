#include "BoardCell.h"
#include "Eter_UI.h"
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
        Board* gameBoard = qobject_cast<Eter_UI*>(window())->getGameBoard();

        bool isFirstCard = !gameBoard->m_firstCardPlayed;

        if (!isFirstCard && !gameBoard->checkNeighbours(m_row, m_col)) {
            event->ignore();
            return;
        }

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
