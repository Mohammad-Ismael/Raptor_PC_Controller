#ifndef HARDWAREINFO_H
#define HARDWAREINFO_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QListWidget>
#include <QTextEdit>
#include <QProcess>
#include <QTimer>
#include <QGridLayout>

class HardwareInfo : public QWidget
{
    Q_OBJECT

public:
    explicit HardwareInfo(QWidget *parent = nullptr);
    ~HardwareInfo();

private slots:
    void updateHardwareInfo();

private:
    void setupUI();
    void fetchHardwareData();
    QString formatBytes(quint64 bytes);
    QString executeCommand(const QString &command, const QStringList &arguments = QStringList());
    
    // Hardware fetching functions
    void fetchCPUInfo();
    void fetchGPUInfo();
    void fetchMotherboardInfo();
    void fetchRAMInfo();
    void fetchStorageInfo();
    void fetchNetworkInfo();
    void fetchUSBInfo();

    // UI elements
    QVBoxLayout *mainLayout;
    QScrollArea *scrollArea;
    QFrame *contentFrame;
    QTextEdit *infoDisplay;

    // Hardware data storage
    QString cpuInfo;
    QString gpuInfo;
    QString motherboardInfo;
    QString ramInfo;
    QString storageInfo;
    QString networkInfo;
    QString usbInfo;

    QTimer *hardwareTimer;
};

#endif // HARDWAREINFO_H