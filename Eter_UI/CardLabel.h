#pragma once

#include <QLabel>
#include <QPixmap>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include "../models/Actions/Card.h"

class CardLabel : public QLabel
{
    Q_OBJECT

public:
    explicit CardLabel(const QString& imagePath, Card::Value cardValue, QWidget* parent);
    
signals:
    void cardMoved(CardLabel* card); 

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    Card::Value m_value;
};
