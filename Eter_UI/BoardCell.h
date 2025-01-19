#pragma once
#include <QLabel>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QBuffer>
#include "../models/GameMechanics/Game.h"


class BoardCell : public QLabel {
    Q_OBJECT

public:
    explicit BoardCell(QWidget* parent = nullptr);

signals:
    void cardPlaced(QDropEvent* event, BoardCell* cell, Card::Value cardValue);


protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
};
