#pragma once
#include <QLabel>
#include <QDrag>
#include <QMimeData>
#include <QPixmap>
#include <QMouseEvent>
#include <QBuffer>

class CardLabel : public QLabel {
    Q_OBJECT

public:
    explicit CardLabel(const QString& imagePath, QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* event) override;
};
