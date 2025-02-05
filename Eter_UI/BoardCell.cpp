#include "BoardCell.h"
#include <QDebug>
#include <QMimeData>
#include <QPixmap>
#include "CardLabel.h"

BoardCell::BoardCell(QWidget* parent) : QLabel(parent) {
    setAcceptDrops(true);
    setStyleSheet("border: 2px solid black;");

}


void BoardCell::setGridPosition(int row, int col) {
    this->row = row;
    this->col = col;
}

int BoardCell::getRow() const {
    return row;
}

int BoardCell::getCol() const {
    return col;
}

void BoardCell::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasFormat("application/x-qt-cardlabel")) {
        setStyleSheet("border: none; background-color: rgba(255,255,255,0.5);");
        event->acceptProposedAction();
    }
    else {
        event->ignore();
    }
}

void BoardCell::dragLeaveEvent(QDragLeaveEvent* event) {
    setStyleSheet("border: none; background: transparent;");
    QLabel::dragLeaveEvent(event);
}

void BoardCell::dragMoveEvent(QDragMoveEvent* event) {
    event->accept();
}

void BoardCell::dropEvent(QDropEvent* event) {
    if (event->mimeData()->hasFormat("application/x-qt-cardlabel")) {
        setStyleSheet("border: none; background: transparent;");
        event->acceptProposedAction();
        emit cardPlaced(event, this);
    }
    else {
        event->ignore();
    }
}
