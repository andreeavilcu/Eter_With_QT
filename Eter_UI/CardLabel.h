#pragma once

#include <QLabel>
#include <QPixmap>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QEnterEvent> // Include pentru gestionarea hover-ului
#include <QToolTip> // Include pentru afișarea tooltipurilor
#include "../models/Actions/Card.h"

class CardLabel : public QLabel
{
    Q_OBJECT

public:
    explicit CardLabel(const QString& imagePath,
        Card::Value cardValue,
        QWidget* parent = nullptr);

    void setDescription(const QString& description); // Setează descrierea cardului
    bool isWizardCard() const { return isWizard; }
    size_t getWizardIndex() const { return wizardIndex; }
    void setWizardCard(bool value, size_t index, const QString& name) {
        isWizard = value;
        wizardIndex = index;
        wizardName = name;
    }
signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override; // Eveniment pentru hover

private:
    Card::Value m_value;
    QString m_description; // Stochează descrierea cardului

    bool isWizard = false;
    size_t wizardIndex = 0;
    QString wizardName;
};
