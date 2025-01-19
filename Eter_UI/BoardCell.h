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

    void setGridPosition(int row, int col) { m_row = row; m_col = col; }
    int getRow() const { return m_row; }
    int getCol() const { return m_col; }

private:
    int m_row;
    int m_col;

signals:
    void cardPlaced(QDropEvent* event, BoardCell* cell, Card::Value cardValue);


protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
};
