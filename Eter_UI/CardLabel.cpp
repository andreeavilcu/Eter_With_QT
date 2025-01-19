#include "CardLabel.h"
#include <QDebug>
#include <QBuffer>

CardLabel::CardLabel(const QString& imagePath,
    Card::Value cardValue,
    QWidget* parent)
    : QLabel(parent)
{
    // Încarcă imaginea
    QPixmap loadedPixmap(imagePath);
    if (loadedPixmap.isNull()) {
        qDebug() << "Eroare: Nu s-a putut încărca imaginea:" << imagePath;
        return;
    }

    // Extrage numele cărții din calea imaginii
    QFileInfo fileInfo(imagePath);
    QString cardName = fileInfo.baseName();

    // Setează proprietățile
    setProperty("cardName", cardName);
    setProperty("cardValue", static_cast<int>(cardValue));

    // Setează pixmap-ul scalat
    setPixmap(loadedPixmap.scaled(100, 150, Qt::KeepAspectRatio,
        Qt::SmoothTransformation));

    // Setează dimensiunea fixă și stilul
    setFixedSize(100, 150);
    setStyleSheet("border: none;");

    // Activează acceptarea drag & drop
    setAcceptDrops(true);
}

void CardLabel::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        // Creăm obiectul drag o singură dată
        QDrag* drag = new QDrag(this);
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

        // Setăm datele MIME o singură dată
        mimeData->setData("application/x-card", byteArray);

        drag->setMimeData(mimeData);
        drag->setPixmap(cardPixmap);
        drag->setHotSpot(QPoint(cardPixmap.width() / 2, cardPixmap.height() / 2));

        if (drag->exec(Qt::MoveAction) == Qt::MoveAction) {
            emit cardMoved(this);
        }
    }

    QLabel::mousePressEvent(event);
}