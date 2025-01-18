#include "BoardCell.h"
#include <QDebug>
#include <QMimeData>
#include <QPixmap>
#include "CardLabel.h"

BoardCell::BoardCell(QWidget* parent) : QLabel(parent) {
    setAcceptDrops(true);
    setStyleSheet("border: 2px solid black;"); 
}

void BoardCell::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasFormat("application/x-qt-cardlabel")) {
        event->acceptProposedAction();
    }
    else {
        event->ignore();
    }
}
void BoardCell::dragMoveEvent(QDragMoveEvent* event) {
    event->accept();
}
void BoardCell::dropEvent(QDropEvent* event) {
    if (event->mimeData()->hasFormat("application/x-qt-cardlabel")) {
        event->acceptProposedAction();
        emit cardPlaced(event, this);
    }
    else {
        event->ignore();
    }
}