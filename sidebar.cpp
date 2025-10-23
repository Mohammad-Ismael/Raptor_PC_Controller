#include "sidebar.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

Sidebar::Sidebar(QWidget *parent)
    : QWidget(parent)
    , layout(nullptr)
{
    setupUI();
}

void Sidebar::setupUI()
{
    // Set fixed width and style
    setFixedWidth(200);
    setStyleSheet(
        "Sidebar {"
        "    background-color: #2c3e50;"
        "}"
        "QPushButton {"
        "    background-color: transparent;"
        "    color: white;"
        "    border: none;"
        "    text-align: left;"
        "    padding: 12px 15px;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #34495e;"
        "}"
        );

    layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    // Add title
    QLabel *title = new QLabel("RAPTOR");
    title->setStyleSheet(
        "color: #1abc9c;"
        "font-size: 20px;"
        "font-weight: bold;"
        "padding: 20px;"
        "background-color: #34495e;"
        );
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    createButtons();

    // Add spacer at the bottom
    layout->addStretch();
}

void Sidebar::createButtons()
{
    QStringList categories = {
        "General",
        "WiFi",
        "Apps",
        "Cleaner",
        "Network",
        "Hardware",
        "Options"
    };

    for (const QString &category : categories) {
        QPushButton *button = new QPushButton(category);
        button->setProperty("category", category);
        connect(button, &QPushButton::clicked, this, &Sidebar::handleButtonClick);
        layout->addWidget(button);
    }
}

void Sidebar::handleButtonClick()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        QString category = button->property("category").toString();
        emit categoryChanged(category);
    }
}
