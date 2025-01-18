#include "CardLabel.h"
#include <QDebug>

CardLabel::CardLabel(const QString& imagePath,
    Card::Value cardValue,
    QWidget* parent)
    : QLabel(parent),
    m_value(cardValue)
{
    QPixmap loadedPixmap(imagePath);
    if (loadedPixmap.isNull()) {
        qDebug() << "Eroare: Nu s-a putut încărca imaginea:" << imagePath;
        return;
    }
    setPixmap(loadedPixmap.scaled(100, 150, Qt::KeepAspectRatio,
        Qt::SmoothTransformation));

    setFixedSize(100, 150);
    setStyleSheet("border: none;");
}

void CardLabel::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        QDrag* drag = new QDrag(this);
        QMimeData* mimeData = new QMimeData;

        mimeData->setData("application/x-qt-cardlabel", QByteArray());

        drag->setMimeData(mimeData);

        QPixmap px = this->pixmap();
        if (!px.isNull()) {
            drag->setPixmap(px.scaled(64, 64,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation));
        }

        Qt::DropAction dropAction = drag->exec(Qt::MoveAction | Qt::CopyAction);
        qDebug() << "Drag ended with action:" << dropAction;
    }

    QLabel::mousePressEvent(event);
}
