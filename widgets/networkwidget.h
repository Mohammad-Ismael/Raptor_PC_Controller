#ifndef NETWORKWIDGET_H
#define NETWORKWIDGET_H

#include <QWidget>
#include <QTimer>

class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QListWidget;
class QLabel;
class QTextEdit;
class QFrame;
class QScrollArea;

class NetworkWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkWidget(QWidget *parent = nullptr);
    ~NetworkWidget();

private slots:
    void pingGoogle();
    void flushDns();
    void showNetworkAdapters();
    void releaseRenewIP();
    void updateConnections();
    void updateSpeedInfo();
    void clearNetworkInfo();
    void refreshIPDetails();
    void checkAllAdaptersStatus();
    
    // Toggle functions
    void toggleEthernet();
    void toggleWifiAdapter();
    void toggleWifiRadio();
    void toggleBluetoothAdapter();
    void toggleBluetoothRadio();

    // Remove the async task finished slots
    void onEthernetTaskFinished() {}
    void onWifiAdapterTaskFinished() {}
    void onWifiRadioTaskFinished() {}
    void onBluetoothAdapterTaskFinished() {}
    void onBluetoothRadioTaskFinished() {}

private:
    void setupUI();
    void createTopControlsSpace();
    void createNetworkAdaptersSpace();
    void createSpeedSpace();
    void createIPManagementSpace();
    void createConnectionsSpace();
    void createControlButtonsSpace();
    void showIPDetails(QTextEdit *display);
    QString executeCommand(const QString &command, const QStringList &arguments = QStringList());
    void parseNetworkAdapters();
    
    // Status checking functions
    QString getEthernetStatus();
    QString getWifiAdapterStatus();
    QString getWifiRadioStatus();
    QString getBluetoothAdapterStatus();
    QString getBluetoothRadioStatus();
    
    QVBoxLayout *mainLayout;
    QScrollArea *scrollArea;
    QListWidget *networkList;
    QTextEdit *infoDisplay;
    QTextEdit *connectionsDisplay;
    QTextEdit *ipDetailsDisplay;
    QLabel *speedLabel;
    
    // Control buttons
    QPushButton *btnEthernet;
    QPushButton *btnWifiAdapter;
    QPushButton *btnWifiRadio;
    QPushButton *btnBluetoothAdapter;
    QPushButton *btnBluetoothRadio;
    
    // Remove the async watcher pointers
    QTimer *connectionsTimer;
    QTimer *speedTimer;
    QTimer *statusTimer;
    int speedCounter;
};

#endif // NETWORKWIDGET_H