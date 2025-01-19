#include "CardLabel.h"
#include <QDebug>
#include "qfileinfo.h""

CardLabel::CardLabel(const QString& imagePath, Card::Value cardValue, QWidget* parent)
    : QLabel(parent) {
    QPixmap pixmap(imagePath);
    if (pixmap.isNull()) {
        qDebug() << "Eroare: Nu s-a putut încărca imaginea:" << imagePath;
        return;
    }

    // Setează proprietatea cardName
    setProperty("cardName", QFileInfo(imagePath).baseName());

    setProperty("cardValue", static_cast<int>(cardValue));
    setPixmap(pixmap.scaled(100, 150, Qt::KeepAspectRatio));
    setFixedSize(100, 150);
    setStyleSheet("border: none;");
}


void CardLabel::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        QMimeData* mimeData = new QMimeData;

        // Salvăm valoarea cărții în datele MIME
        QByteArray valueData;
        valueData.setNum(property("cardValue").toInt());
        mimeData->setData("card-value", valueData);

        // Salvăm imaginea cărții
        QPixmap cardPixmap = this->pixmap(Qt::ReturnByValue);
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);
        cardPixmap.save(&buffer, "PNG");
        mimeData->setData("application/x-card", byteArray);
        mimeData->setData("application/x-card", byteArray);

        QDrag* drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(cardPixmap);
        drag->setHotSpot(QPoint(cardPixmap.width() / 2, cardPixmap.height() / 2));

        drag->setHotSpot(QPoint(cardPixmap.width() / 2, cardPixmap.height() / 2));

        if (drag->exec(Qt::MoveAction) == Qt::MoveAction) {
            emit cardMoved(this);
        }
    }
}