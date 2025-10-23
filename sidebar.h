#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>

class QVBoxLayout;
class QPushButton;

class Sidebar : public QWidget
{
    Q_OBJECT

public:
    explicit Sidebar(QWidget *parent = nullptr);

signals:
    void categoryChanged(const QString &category);

private slots:
    void handleButtonClick();

private:
    void setupUI();
    void createButtons();

    QVBoxLayout *layout;
};

#endif // SIDEBAR_H
