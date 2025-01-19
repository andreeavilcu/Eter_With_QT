#pragma once
#include <QLabel>
#include <QDrag>
#include <QMimeData>
#include <QPixmap>
#include <QMouseEvent>
#include <QBuffer>
#include "../models/Actions/Card.h"

class CardLabel : public QLabel {
    Q_OBJECT

public:
    explicit CardLabel(const QString& imagePath, Card::Value cardValue, QWidget* parent);

signals:
    void cardMoved(CardLabel* card);

protected:
    void mousePressEvent(QMouseEvent* event) override;
};