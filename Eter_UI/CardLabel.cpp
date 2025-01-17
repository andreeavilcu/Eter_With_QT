#include "CardLabel.h"
#include <QDebug>

CardLabel::CardLabel(const QString& imagePath, QWidget* parent)
    : QLabel(parent) {
    QPixmap pixmap(imagePath);
    if (pixmap.isNull()) {
        qDebug() << "Eroare: Nu s-a putut încărca imaginea:" << imagePath;
        return;
    }

    setPixmap(pixmap.scaled(100, 150, Qt::KeepAspectRatio));
    setFixedSize(100, 150);
    setStyleSheet("border: none;");
}

void CardLabel::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        QMimeData* mimeData = new QMimeData;

        QPixmap cardPixmap = this->pixmap(Qt::ReturnByValue);
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);
        cardPixmap.save(&buffer, "PNG");

        mimeData->setData("application/x-card", byteArray);  // Trimitem imaginea sub formă de date binare

        QDrag* drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(cardPixmap);

        drag->exec(Qt::MoveAction);
    }
}
