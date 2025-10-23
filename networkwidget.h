#ifndef NETWORKWIDGET_H
#define NETWORKWIDGET_H

#include <QWidget>

class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QListWidget;
class QLabel;
class QTextEdit;

class NetworkWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkWidget(QWidget *parent = nullptr);

private slots:
    void refreshNetworkInfo();
    void pingGoogle();
    void flushDns();
    void releaseRenewIP();
    void showNetworkAdapters();

private:
    void setupUI();
    void createInfoSection();
    void createControlButtons();
    void createNetworkList();

    QVBoxLayout *mainLayout;
    QListWidget *networkList;
    QTextEdit *infoDisplay;
    QLabel *statusLabel;
};

#endif // NETWORKWIDGET_H
