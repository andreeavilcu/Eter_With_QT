#pragma once
#include <QLabel>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QBuffer>
#include <QTimer>

class BoardCell : public QLabel {
    Q_OBJECT

public:
    explicit BoardCell(QWidget* parent = nullptr);

    void setGridPosition(int row, int col); // New function
    int getRow() const;                    // New function
    int getCol() const;                    // New function
    

signals:
    void cardPlaced(QDropEvent* event, BoardCell* cell);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;

private:
    int row = -1; // Store row position
    int col = -1; // Store column position
   
};
