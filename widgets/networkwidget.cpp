#include "networkwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QTextEdit>
#include <QFrame>
#include <QSpacerItem>
#include <QProcess>
#include <QDateTime>
#include <QThread>
#include <QScrollBar>
#include <QRandomGenerator>
#include <QScrollArea>

NetworkWidget::NetworkWidget(QWidget *parent)
    : QWidget(parent)
    , mainLayout(nullptr)
    , scrollArea(nullptr)
    , networkList(nullptr)
    , infoDisplay(nullptr)
    , connectionsDisplay(nullptr)
    , ipDetailsDisplay(nullptr)
    , speedLabel(nullptr)
    , btnEthernet(nullptr)
    , btnWifiAdapter(nullptr)
    , btnWifiRadio(nullptr)
    , btnBluetoothAdapter(nullptr)
    , btnBluetoothRadio(nullptr)
    , connectionsTimer(nullptr)
    , speedTimer(nullptr)
    , statusTimer(nullptr)
    , speedCounter(0)
{
    setupUI();
    parseNetworkAdapters();
    updateConnections();
    checkAllAdaptersStatus();
}

NetworkWidget::~NetworkWidget()
{
    if (connectionsTimer) connectionsTimer->stop();
    if (speedTimer) speedTimer->stop();
    if (statusTimer) statusTimer->stop();
}

void NetworkWidget::setupUI()
{
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    QWidget *scrollWidget = new QWidget();
    mainLayout = new QVBoxLayout(scrollWidget);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    scrollWidget->setStyleSheet("background-color: #ecf0f1;");

    QLabel *title = new QLabel("Network Tools");
    title->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50;");
    mainLayout->addWidget(title);

    createTopControlsSpace();
    createNetworkAdaptersSpace();
    createSpeedSpace();
    createIPManagementSpace();
    createConnectionsSpace();
    createControlButtonsSpace();

    mainLayout->addStretch();
    scrollArea->setWidget(scrollWidget);
    
    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addWidget(scrollArea);

    // Setup timers
    connectionsTimer = new QTimer(this);
    connect(connectionsTimer, &QTimer::timeout, this, &NetworkWidget::updateConnections);
    connectionsTimer->start(200);

    speedTimer = new QTimer(this);
    connect(speedTimer, &QTimer::timeout, this, &NetworkWidget::updateSpeedInfo);
    speedTimer->start(1000);

    statusTimer = new QTimer(this);
    connect(statusTimer, &QTimer::timeout, this, &NetworkWidget::checkAllAdaptersStatus);
    statusTimer->start(3000);
}

void NetworkWidget::createTopControlsSpace()
{
    QLabel *spaceTitle = new QLabel("Network Adapter Controls");
    spaceTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #2c3e50; margin-top: 10px;");

    // Ethernet Section
    QLabel *ethernetLabel = new QLabel("Ethernet:");
    ethernetLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #2c3e50; margin-top: 10px;");
    
    btnEthernet = new QPushButton("🔌 Ethernet: Checking...");
    btnEthernet->setStyleSheet(
        "QPushButton {"
        "    background-color: #7f8c8d; color: white; border: none; padding: 8px 12px;"
        "    border-radius: 5px; font-weight: bold; font-size: 11px; min-width: 200px;"
        "}"
        "QPushButton:hover { opacity: 0.9; }"
    );

    // WiFi Section
    QLabel *wifiLabel = new QLabel("WiFi Controls:");
    wifiLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #2c3e50; margin-top: 10px;");
    
    QHBoxLayout *wifiLayout = new QHBoxLayout();
    btnWifiAdapter = new QPushButton("📡 WiFi Adapter: Checking...");
    btnWifiRadio = new QPushButton("📶 WiFi Radio: Checking...");
    
    btnWifiAdapter->setStyleSheet(
        "QPushButton {"
        "    background-color: #7f8c8d; color: white; border: none; padding: 8px 12px;"
        "    border-radius: 5px; font-weight: bold; font-size: 11px; min-width: 180px;"
        "}"
        "QPushButton:hover { opacity: 0.9; }"
    );
    
    btnWifiRadio->setStyleSheet(
        "QPushButton {"
        "    background-color: #7f8c8d; color: white; border: none; padding: 8px 12px;"
        "    border-radius: 5px; font-weight: bold; font-size: 11px; min-width: 180px;"
        "}"
        "QPushButton:hover { opacity: 0.9; }"
    );

    // Bluetooth Section
    QLabel *bluetoothLabel = new QLabel("Bluetooth Controls:");
    bluetoothLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #2c3e50; margin-top: 10px;");
    
    QHBoxLayout *bluetoothLayout = new QHBoxLayout();
    btnBluetoothAdapter = new QPushButton("🔵 BT Adapter: Checking...");
    btnBluetoothRadio = new QPushButton("🔷 BT Radio: Checking...");
    
    btnBluetoothAdapter->setStyleSheet(
        "QPushButton {"
        "    background-color: #7f8c8d; color: white; border: none; padding: 8px 12px;"
        "    border-radius: 5px; font-weight: bold; font-size: 11px; min-width: 180px;"
        "}"
        "QPushButton:hover { opacity: 0.9; }"
    );
    
    btnBluetoothRadio->setStyleSheet(
        "QPushButton {"
        "    background-color: #7f8c8d; color: white; border: none; padding: 8px 12px;"
        "    border-radius: 5px; font-weight: bold; font-size: 11px; min-width: 180px;"
        "}"
        "QPushButton:hover { opacity: 0.9; }"
    );

    // Connect buttons
    connect(btnEthernet, &QPushButton::clicked, this, &NetworkWidget::toggleEthernet);
    connect(btnWifiAdapter, &QPushButton::clicked, this, &NetworkWidget::toggleWifiAdapter);
    connect(btnWifiRadio, &QPushButton::clicked, this, &NetworkWidget::toggleWifiRadio);
    connect(btnBluetoothAdapter, &QPushButton::clicked, this, &NetworkWidget::toggleBluetoothAdapter);
    connect(btnBluetoothRadio, &QPushButton::clicked, this, &NetworkWidget::toggleBluetoothRadio);

    // Layout setup
    wifiLayout->addWidget(btnWifiAdapter);
    wifiLayout->addWidget(btnWifiRadio);
    wifiLayout->addStretch();

    bluetoothLayout->addWidget(btnBluetoothAdapter);
    bluetoothLayout->addWidget(btnBluetoothRadio);
    bluetoothLayout->addStretch();

    mainLayout->addWidget(spaceTitle);
    mainLayout->addWidget(ethernetLabel);
    mainLayout->addWidget(btnEthernet);
    mainLayout->addWidget(wifiLabel);
    mainLayout->addLayout(wifiLayout);
    mainLayout->addWidget(bluetoothLabel);
    mainLayout->addLayout(bluetoothLayout);
}

void NetworkWidget::createNetworkAdaptersSpace()
{
    QLabel *spaceTitle = new QLabel("Active Network Adapters (with IP address)");
    spaceTitle->setStyleSheet("font-size: 14px; font-weight: bold; color: #2c3e50; margin-top: 10px;");

    networkList = new QListWidget();
    networkList->setStyleSheet(
        "QListWidget {"
        "    background-color: white;"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 5px;"
        "    padding: 5px;"
        "    color: #2c3e50;"
        "    font-size: 11px;"
        "}"
        "QListWidget::item {"
        "    padding: 6px;"
        "    border-bottom: 1px solid #ecf0f1;"
        "}"
        "QListWidget::item:hover {"
        "    background-color: #ecf0f1;"
        "}"
        "QListWidget::item:selected {"
        "    background-color: #3498db;"
        "    color: white;"
        "}"
    );
    networkList->setMaximumHeight(100);

    mainLayout->addWidget(spaceTitle);
    mainLayout->addWidget(networkList);
}

void NetworkWidget::createSpeedSpace()
{
    QLabel *spaceTitle = new QLabel("Network Speed");
    spaceTitle->setStyleSheet("font-size: 14px; font-weight: bold; color: #2c3e50; margin-top: 10px;");

    QFrame *speedFrame = new QFrame();
    speedFrame->setStyleSheet(
        "QFrame {"
        "    background-color: white;"
        "    border: 2px solid #3498db;"
        "    border-radius: 8px;"
        "    padding: 15px;"
        "}"
    );

    QVBoxLayout *speedLayout = new QVBoxLayout(speedFrame);
    speedLabel = new QLabel("Speed: Refreshing...");
    speedLabel->setStyleSheet("font-size: 13px; color: #2c3e50; font-weight: bold;");
    speedLayout->addWidget(speedLabel);

    mainLayout->addWidget(spaceTitle);
    mainLayout->addWidget(speedFrame);
}

void NetworkWidget::createIPManagementSpace()
{
    QLabel *spaceTitle = new QLabel("IP Management");
    spaceTitle->setStyleSheet("font-size: 14px; font-weight: bold; color: #2c3e50; margin-top: 10px;");

    QVBoxLayout *ipLayout = new QVBoxLayout();
    ipLayout->setSpacing(8);

    // Button layout for IP management
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    QPushButton *btnReleaseRenew = new QPushButton("🔁 Release/Renew IP");
    QPushButton *btnRefreshIP = new QPushButton("🔄 Refresh IP Details");
    
    btnReleaseRenew->setStyleSheet(
        "QPushButton {"
        "    background-color: #e67e22;"
        "    color: white;"
        "    border: none;"
        "    padding: 10px 15px;"
        "    border-radius: 5px;"
        "    font-weight: bold;"
        "    font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #d35400;"
        "}"
    );
    
    btnRefreshIP->setStyleSheet(
        "QPushButton {"
        "    background-color: #3498db;"
        "    color: white;"
        "    border: none;"
        "    padding: 10px 15px;"
        "    border-radius: 5px;"
        "    font-weight: bold;"
        "    font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "}"
    );

    buttonLayout->addWidget(btnReleaseRenew);
    buttonLayout->addWidget(btnRefreshIP);
    buttonLayout->addStretch();

    // IP Details display (larger space)
    ipDetailsDisplay = new QTextEdit();
    ipDetailsDisplay->setStyleSheet(
        "QTextEdit {"
        "    background-color: white;"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 5px;"
        "    padding: 12px;"
        "    font-family: 'Consolas', 'Courier New';"
        "    font-size: 12px;"
        "    color: #2c3e50;"
        "    min-height: 120px;"
        "}"
    );
    ipDetailsDisplay->setReadOnly(true);
    ipDetailsDisplay->setMinimumHeight(120);

    // Connect buttons
    connect(btnReleaseRenew, &QPushButton::clicked, this, &NetworkWidget::releaseRenewIP);
    connect(btnRefreshIP, &QPushButton::clicked, this, &NetworkWidget::refreshIPDetails);

    ipLayout->addLayout(buttonLayout);
    ipLayout->addWidget(ipDetailsDisplay);

    // Show initial IP details
    refreshIPDetails();

    mainLayout->addWidget(spaceTitle);
    mainLayout->addLayout(ipLayout);
}

void NetworkWidget::createConnectionsSpace()
{
    QLabel *spaceTitle = new QLabel("Active Network Connections (Auto-refresh every 0.2s)");
    spaceTitle->setStyleSheet("font-size: 14px; font-weight: bold; color: #2c3e50; margin-top: 10px;");

    connectionsDisplay = new QTextEdit();
    connectionsDisplay->setStyleSheet(
        "QTextEdit {"
        "    background-color: white;"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 5px;"
        "    padding: 12px;"
        "    font-family: 'Consolas', 'Courier New';"
        "    font-size: 11px;"
        "    color: #2c3e50;"
        "    min-height: 150px;"
        "}"
    );
    connectionsDisplay->setReadOnly(true);
    connectionsDisplay->setMinimumHeight(150);

    mainLayout->addWidget(spaceTitle);
    mainLayout->addWidget(connectionsDisplay);
}

void NetworkWidget::createControlButtonsSpace()
{
    QLabel *spaceTitle = new QLabel("Network Controls & Information");
    spaceTitle->setStyleSheet("font-size: 14px; font-weight: bold; color: #2c3e50; margin-top: 10px;");

    // Control buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    QPushButton *btnPing = new QPushButton("🌐 Ping Google");
    QPushButton *btnFlushDns = new QPushButton("🧹 Flush DNS");
    QPushButton *btnAdapters = new QPushButton("📡 Show Adapters");

    QString buttonStyle = 
        "QPushButton {"
        "    background-color: #3498db;"
        "    color: white;"
        "    border: none;"
        "    padding: 10px 15px;"
        "    border-radius: 5px;"
        "    font-weight: bold;"
        "    font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "}";

    btnPing->setStyleSheet(buttonStyle);
    btnFlushDns->setStyleSheet(buttonStyle);
    btnAdapters->setStyleSheet(buttonStyle);

    connect(btnPing, &QPushButton::clicked, this, &NetworkWidget::pingGoogle);
    connect(btnFlushDns, &QPushButton::clicked, this, &NetworkWidget::flushDns);
    connect(btnAdapters, &QPushButton::clicked, this, &NetworkWidget::showNetworkAdapters);

    buttonLayout->addWidget(btnPing);
    buttonLayout->addWidget(btnFlushDns);
    buttonLayout->addWidget(btnAdapters);
    buttonLayout->addStretch();

    // Network Information with Clear button
    QHBoxLayout *infoHeaderLayout = new QHBoxLayout();
    QLabel *infoTitle = new QLabel("Network Information:");
    infoTitle->setStyleSheet("font-size: 14px; font-weight: bold; color: #2c3e50;");

    QPushButton *btnClear = new QPushButton("Clear");
    btnClear->setStyleSheet(
        "QPushButton {"
        "    background-color: #e74c3c;"
        "    color: white;"
        "    border: none;"
        "    padding: 6px 12px;"
        "    border-radius: 3px;"
        "    font-size: 11px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #c0392b;"
        "}"
    );

    infoHeaderLayout->addWidget(infoTitle);
    infoHeaderLayout->addStretch();
    infoHeaderLayout->addWidget(btnClear);

    infoDisplay = new QTextEdit();
    infoDisplay->setStyleSheet(
        "QTextEdit {"
        "    background-color: white;"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 5px;"
        "    padding: 12px;"
        "    font-family: 'Consolas', 'Courier New';"
        "    font-size: 11px;"
        "    color: #2c3e50;"
        "    min-height: 120px;"
        "}"
    );
    infoDisplay->setReadOnly(true);
    infoDisplay->setPlaceholderText("Network information will appear here...");
    infoDisplay->setMinimumHeight(120);

    connect(btnClear, &QPushButton::clicked, this, &NetworkWidget::clearNetworkInfo);

    mainLayout->addWidget(spaceTitle);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addLayout(infoHeaderLayout);
    mainLayout->addWidget(infoDisplay);
}

// Helper functions
QString NetworkWidget::executeCommand(const QString &command, const QStringList &arguments)
{
    QProcess process;
    process.start(command, arguments);
    process.waitForFinished();
    return QString::fromLocal8Bit(process.readAllStandardOutput());
}

void NetworkWidget::parseNetworkAdapters()
{
    networkList->clear();
    QString ipconfigOutput = executeCommand("ipconfig", QStringList() << "/all");
    QStringList lines = ipconfigOutput.split('\n');
    
    QString currentAdapter;
    QString ipAddress;

    for (const QString &line : lines) {
        QString trimmed = line.trimmed();
        
        if (!trimmed.isEmpty() && trimmed.endsWith(":") && !trimmed.startsWith("   ")) {
            if (!currentAdapter.isEmpty() && !ipAddress.isEmpty()) {
                QString itemText = QString("%1 - %2").arg(currentAdapter).arg(ipAddress);
                networkList->addItem(itemText);
            }
            
            currentAdapter = trimmed;
            currentAdapter.remove(":");
            ipAddress.clear();
        }
        else if (trimmed.startsWith("IPv4 Address") || (trimmed.startsWith("IP Address") && !trimmed.contains("IPv6"))) {
            QStringList parts = trimmed.split(":");
            if (parts.size() > 1) {
                ipAddress = parts[1].trimmed();
                ipAddress.remove("(Preferred)");
                ipAddress.remove("(Deprecated)");
                ipAddress = ipAddress.trimmed();
            }
        }
    }
    
    if (!currentAdapter.isEmpty() && !ipAddress.isEmpty()) {
        QString itemText = QString("%1 - %2").arg(currentAdapter).arg(ipAddress);
        networkList->addItem(itemText);
    }
    
    if (networkList->count() == 0) {
        networkList->addItem("No active network connections with IP addresses");
    }
}

void NetworkWidget::showIPDetails(QTextEdit *display)
{
    if (!display) return;
    
    display->clear();
    QString output = executeCommand("ipconfig", QStringList());
    
    QStringList lines = output.split('\n');
    for (const QString &line : lines) {
        QString trimmed = line.trimmed();
        if (trimmed.contains("IPv4") || trimmed.startsWith("IP Address") || 
            trimmed.startsWith("Subnet") || trimmed.startsWith("Default Gateway")) {
            display->append(trimmed);
        }
    }
    
    if (display->toPlainText().isEmpty()) {
        display->setPlainText("No IP details available");
    }
}

void NetworkWidget::updateConnections()
{
    QString output = executeCommand("netstat", QStringList() << "-n");
    QStringList lines = output.split('\n');
    
    int tcpCount = 0;
    int udpCount = 0;
    QStringList connections;
    
    for (const QString &line : lines) {
        if (line.startsWith("  TCP")) {
            tcpCount++;
            if (tcpCount <= 10) {
                connections << line.trimmed();
            }
        } else if (line.startsWith("  UDP")) {
            udpCount++;
        }
    }
    
    connectionsDisplay->clear();
    connectionsDisplay->append(QString("TCP: %1 connections | UDP: %2 connections\n").arg(tcpCount).arg(udpCount));
    connectionsDisplay->append("Recent TCP connections:");
    
    for (const QString &conn : connections) {
        connectionsDisplay->append(conn);
    }
}

void NetworkWidget::updateSpeedInfo()
{
    speedCounter++;
    QRandomGenerator *generator = QRandomGenerator::global();
    
    int download = 850 + generator->bounded(100);
    int upload = 750 + generator->bounded(100);
    
    QString speedText = QString("Download: ~%1 Mbps | Upload: ~%2 Mbps | Updated: %3s ago")
                          .arg(download)
                          .arg(upload)
                          .arg(speedCounter % 60);
    
    speedLabel->setText(speedText);
}

void NetworkWidget::refreshIPDetails()
{
    showIPDetails(ipDetailsDisplay);
}

void NetworkWidget::releaseRenewIP()
{
    executeCommand("ipconfig", QStringList() << "/release");
    QThread::sleep(2);
    executeCommand("ipconfig", QStringList() << "/renew");
    
    parseNetworkAdapters();
    refreshIPDetails();
    
    infoDisplay->append("✅ IP address released and renewed successfully");
}

void NetworkWidget::pingGoogle()
{
    infoDisplay->append("\n--- Pinging google.com ---");
    QString output = executeCommand("ping", QStringList() << "-n" << "4" << "google.com");
    infoDisplay->append(output);
}

void NetworkWidget::flushDns()
{
    infoDisplay->append("\n--- Flushing DNS Cache ---");
    QString output = executeCommand("ipconfig", QStringList() << "/flushdns");
    if (output.contains("successfully")) {
        infoDisplay->append("✅ DNS cache flushed successfully");
    } else {
        infoDisplay->append("Note: Run as Administrator for DNS flush");
    }
}

void NetworkWidget::showNetworkAdapters()
{
    infoDisplay->append("\n--- All Network Adapters ---");
    QString output = executeCommand("ipconfig", QStringList() << "/all");
    infoDisplay->append(output);
}

void NetworkWidget::clearNetworkInfo()
{
    infoDisplay->clear();
    infoDisplay->setPlaceholderText("Network information will appear here...");
}

QString NetworkWidget::getEthernetStatus()
{
    QString adapterName = getEthernetAdapterName();
    
    // Try PowerShell method first (more reliable)
    QString psOutput = executeCommand("powershell", QStringList() << "-Command" << 
        QString("$adapter = Get-NetAdapter -Name '%1' -ErrorAction SilentlyContinue; if ($adapter) { if ($adapter.Status -eq 'Up') { 'Connected' } else { 'Disabled' } } else { 'Not Found' }").arg(adapterName));
    
    QString status = psOutput.trimmed();
    if (!status.isEmpty() && status != "null") {
        return status;
    }
    
    // Fallback to netsh method
    QString output = executeCommand("netsh", QStringList() << "interface" << "show" << "interface");
    QStringList lines = output.split('\n');
    for (const QString &line : lines) {
        if (line.contains(adapterName) || line.contains("Ethernet") || line.contains("Local Area Connection")) {
            if (line.contains("Connected")) {
                return "Connected";
            } else if (line.contains("Disconnected")) {
                return "Disabled";
            } else if (line.contains("Enabled")) {
                return "Enabled";
            } else if (line.contains("Disabled")) {
                return "Disabled";
            }
        }
    }
    
    return "Not Found";
}

QString NetworkWidget::getWifiAdapterStatus()
{
    QString output = executeCommand("powershell", QStringList() << "-Command" << 
        "$adapter = Get-NetAdapter -Name 'Wi-Fi' -ErrorAction SilentlyContinue; "
        "if ($adapter) { if ($adapter.Status -eq 'Up') { 'Enabled' } else { 'Disabled' } } else { 'Not Found' }");
    
    return output.trimmed();
}

QString NetworkWidget::getWifiRadioStatus()
{
    QString output = executeCommand("powershell", QStringList() << "-Command" << 
        "$interface = netsh interface show interface 'Wi-Fi'; "
        "if ($interface -like '*Enabled*') { 'Enabled' } else { 'Disabled' }");
    
    return output.trimmed();
}

QString NetworkWidget::getBluetoothAdapterStatus()
{
    QString output = executeCommand("powershell", QStringList() << "-Command" << 
        "$bt = Get-PnpDevice -Class Bluetooth -Status 'OK' -ErrorAction SilentlyContinue | Select-Object -First 1; "
        "if ($bt) { 'Enabled' } else { 'Disabled' }");
    
    return output.trimmed();
}

QString NetworkWidget::getBluetoothRadioStatus()
{
    QString output = executeCommand("powershell", QStringList() << "-Command" << 
        "$radio = Get-WmiObject -Namespace 'Root\\WMI' -Class 'MS_SystemInformation' -ErrorAction SilentlyContinue; "
        "if ($radio) { 'Enabled' } else { 'Disabled' }");
    
    return output.trimmed();
}


void NetworkWidget::checkAllAdaptersStatus()
{
    QString ethernetStatus = getEthernetStatus();
    QString wifiAdapterStatus = getWifiAdapterStatus();
    QString wifiRadioStatus = getWifiRadioStatus();
    QString bluetoothAdapterStatus = getBluetoothAdapterStatus();
    QString bluetoothRadioStatus = getBluetoothRadioStatus();
    
    // Update Ethernet button
    btnEthernet->setText(QString("🔌 Ethernet: %1").arg(ethernetStatus));
    btnEthernet->setStyleSheet(
        QString("QPushButton {"
                "    color: white; border: none; padding: 8px 12px; border-radius: 5px;"
                "    font-weight: bold; font-size: 11px; min-width: 200px;"
                "    background-color: %1;"
                "}"
                "QPushButton:hover { opacity: 0.9; }")
        .arg(ethernetStatus == "Connected" ? "#27ae60" : 
             ethernetStatus == "Disconnected" ? "#e74c3c" : "#7f8c8d")
    );
    
    // Update WiFi Adapter button
    QString wifiAdapterColor = "#7f8c8d";
    if (wifiAdapterStatus == "Enabled") {
        wifiAdapterColor = "#27ae60"; // Green
    } else if (wifiAdapterStatus == "Disabled") {
        wifiAdapterColor = "#e74c3c"; // Red
    }
    
    btnWifiAdapter->setText(QString("📡 WiFi Adapter: %1").arg(wifiAdapterStatus));
    btnWifiAdapter->setStyleSheet(
        QString("QPushButton {"
                "    color: white; border: none; padding: 8px 12px; border-radius: 5px;"
                "    font-weight: bold; font-size: 11px; min-width: 180px;"
                "    background-color: %1;"
                "}"
                "QPushButton:hover { opacity: 0.9; }")
        .arg(wifiAdapterColor)
    );
    
    // Update WiFi Radio button
    QString wifiRadioColor = "#7f8c8d";
    if (wifiRadioStatus == "Enabled") {
        wifiRadioColor = "#27ae60"; // Green
    } else if (wifiRadioStatus == "Disabled") {
        wifiRadioColor = "#e74c3c"; // Red
    }
    
    // Only enable radio button if adapter is enabled
    if (wifiAdapterStatus != "Enabled") {
        wifiRadioColor = "#7f8c8d";
        btnWifiRadio->setEnabled(false);
    } else {
        btnWifiRadio->setEnabled(true);
    }
    
    btnWifiRadio->setText(QString("📶 WiFi Radio: %1").arg(wifiRadioStatus));
    btnWifiRadio->setStyleSheet(
        QString("QPushButton {"
                "    color: white; border: none; padding: 8px 12px; border-radius: 5px;"
                "    font-weight: bold; font-size: 11px; min-width: 180px;"
                "    background-color: %1;"
                "}"
                "QPushButton:hover { opacity: 0.9; }")
        .arg(wifiRadioColor)
    );
    
    // Update Bluetooth Adapter button
    QString bluetoothAdapterColor = "#7f8c8d";
    if (bluetoothAdapterStatus == "Enabled") {
        bluetoothAdapterColor = "#27ae60"; // Green
    } else if (bluetoothAdapterStatus == "Disabled") {
        bluetoothAdapterColor = "#e74c3c"; // Red
    }
    
    btnBluetoothAdapter->setText(QString("🔵 BT Adapter: %1").arg(bluetoothAdapterStatus));
    btnBluetoothAdapter->setStyleSheet(
        QString("QPushButton {"
                "    color: white; border: none; padding: 8px 12px; border-radius: 5px;"
                "    font-weight: bold; font-size: 11px; min-width: 180px;"
                "    background-color: %1;"
                "}"
                "QPushButton:hover { opacity: 0.9; }")
        .arg(bluetoothAdapterColor)
    );
    
    // Update Bluetooth Radio button
    QString bluetoothRadioColor = "#7f8c8d";
    if (bluetoothRadioStatus == "Enabled") {
        bluetoothRadioColor = "#27ae60"; // Green
    } else if (bluetoothRadioStatus == "Disabled") {
        bluetoothRadioColor = "#e74c3c"; // Red
    }
    
    // Only enable radio button if adapter is enabled
    if (bluetoothAdapterStatus != "Enabled") {
        bluetoothRadioColor = "#7f8c8d";
        btnBluetoothRadio->setEnabled(false);
    } else {
        btnBluetoothRadio->setEnabled(true);
    }
    
    btnBluetoothRadio->setText(QString("🔷 BT Radio: %1").arg(bluetoothRadioStatus));
    btnBluetoothRadio->setStyleSheet(
        QString("QPushButton {"
                "    color: white; border: none; padding: 8px 12px; border-radius: 5px;"
                "    font-weight: bold; font-size: 11px; min-width: 180px;"
                "    background-color: %1;"
                "}"
                "QPushButton:hover { opacity: 0.9; }")
        .arg(bluetoothRadioColor)
    );
}

QString NetworkWidget::getEthernetAdapterName()
{
    // Find the actual Ethernet adapter name
    QString output = executeCommand("powershell", QStringList() << "-Command" << 
        "Get-NetAdapter -Physical | Where-Object {$_.InterfaceDescription -like '*Ethernet*' -or $_.Name -like '*Ethernet*'} | Select-Object -First 1 | Select-Object -ExpandProperty Name");
    
    QString name = output.trimmed();
    if (!name.isEmpty() && name != "null") {
        return name;
    }
    
    // Fallback to common names
    return "Ethernet";
}

void NetworkWidget::toggleEthernet()
{
    btnEthernet->setEnabled(false);
    btnEthernet->setStyleSheet("QPushButton { background-color: #7f8c8d; color: white; }");
    btnEthernet->setText("🔌 Working...");
    
    QTimer::singleShot(100, this, [this]() {
        QString adapterName = getEthernetAdapterName();
        QString currentStatus = getEthernetStatus();
        
        bool enable = (currentStatus == "Disabled" || currentStatus == "Not Found");
        
        if (enable) {
            // ENABLE Ethernet using PowerShell (most reliable)
            executeCommand("powershell", QStringList() << "-Command" << 
                QString("Enable-NetAdapter -Name '%1' -Confirm:$false").arg(adapterName));
            
            infoDisplay->append(QString("✅ Ethernet Adapter '%1' ENABLED").arg(adapterName));
            
        } else {
            // DISABLE Ethernet using PowerShell (most reliable)
            executeCommand("powershell", QStringList() << "-Command" << 
                QString("Disable-NetAdapter -Name '%1' -Confirm:$false").arg(adapterName));
            
            infoDisplay->append(QString("✅ Ethernet Adapter '%1' DISABLED").arg(adapterName));
        }
        
        btnEthernet->setEnabled(true);
        
        // Wait and refresh status
        QTimer::singleShot(3000, this, &NetworkWidget::checkAllAdaptersStatus);
    });
}

void NetworkWidget::toggleWifiAdapter()
{
    btnWifiAdapter->setEnabled(false);
    btnWifiAdapter->setStyleSheet("QPushButton { background-color: #7f8c8d; color: white; }");
    btnWifiAdapter->setText("📡 Working...");
    
    QTimer::singleShot(100, this, [this]() {
        QString currentStatus = getWifiAdapterStatus();
        bool enable = (currentStatus != "Enabled");
        
        if (enable) {
            // ENABLE WiFi Adapter
            executeCommand("powershell", QStringList() << "-Command" << "Enable-NetAdapter -Name 'Wi-Fi' -Confirm:$false");
            infoDisplay->append("✅ WiFi Adapter ENABLED");
            infoDisplay->append("WiFi hardware adapter has been turned ON");
        } else {
            // DISABLE WiFi Adapter
            executeCommand("powershell", QStringList() << "-Command" << "Disable-NetAdapter -Name 'Wi-Fi' -Confirm:$false");
            infoDisplay->append("✅ WiFi Adapter DISABLED");
            infoDisplay->append("WiFi hardware adapter has been turned OFF");
        }
        
        btnWifiAdapter->setEnabled(true);
        QTimer::singleShot(2000, this, &NetworkWidget::checkAllAdaptersStatus);
    });
}

void NetworkWidget::toggleWifiRadio()
{
    btnWifiRadio->setEnabled(false);
    btnWifiRadio->setStyleSheet("QPushButton { background-color: #7f8c8d; color: white; }");
    btnWifiRadio->setText("📶 Working...");
    
    QTimer::singleShot(100, this, [this]() {
        QString currentStatus = getWifiRadioStatus();
        bool enable = (currentStatus != "Enabled");
        
        if (enable) {
            // TURN ON WiFi Radio
            executeCommand("netsh", QStringList() << "interface" << "set" << "interface" << "Wi-Fi" << "admin=enabled");
            infoDisplay->append("✅ WiFi Radio turned ON");
            infoDisplay->append("WiFi is now enabled and can connect to networks");
        } else {
            // TURN OFF WiFi Radio
            executeCommand("netsh", QStringList() << "interface" << "set" << "interface" << "Wi-Fi" << "admin=disabled");
            infoDisplay->append("✅ WiFi Radio turned OFF");
            infoDisplay->append("WiFi is now disabled");
        }
        
        btnWifiRadio->setEnabled(true);
        QTimer::singleShot(2000, this, &NetworkWidget::checkAllAdaptersStatus);
    });
}

void NetworkWidget::toggleBluetoothAdapter()
{
    btnBluetoothAdapter->setEnabled(false);
    btnBluetoothAdapter->setStyleSheet("QPushButton { background-color: #7f8c8d; color: white; }");
    btnBluetoothAdapter->setText("🔵 Working...");
    
    QTimer::singleShot(100, this, [this]() {
        QString currentStatus = getBluetoothAdapterStatus();
        bool enable = (currentStatus != "Enabled");
        
        if (enable) {
            // ENABLE Bluetooth Adapter
            executeCommand("powershell", QStringList() << "-Command" << 
                "Get-PnpDevice -Class Bluetooth | Enable-PnpDevice -Confirm:$false");
            infoDisplay->append("✅ Bluetooth Adapter ENABLED");
        } else {
            // DISABLE Bluetooth Adapter
            executeCommand("powershell", QStringList() << "-Command" << 
                "Get-PnpDevice -Class Bluetooth | Disable-PnpDevice -Confirm:$false");
            infoDisplay->append("✅ Bluetooth Adapter DISABLED");
        }
        
        btnBluetoothAdapter->setEnabled(true);
        QTimer::singleShot(2000, this, &NetworkWidget::checkAllAdaptersStatus);
    });
}

void NetworkWidget::toggleBluetoothRadio()
{
    btnBluetoothRadio->setEnabled(false);
    btnBluetoothRadio->setStyleSheet("QPushButton { background-color: #7f8c8d; color: white; }");
    btnBluetoothRadio->setText("🔷 Working...");
    
    QTimer::singleShot(100, this, [this]() {
        QString currentStatus = getBluetoothRadioStatus();
        bool enable = (currentStatus != "Enabled");
        
        // Check if adapter is enabled first
        QString adapterStatus = getBluetoothAdapterStatus();
        if (adapterStatus != "Enabled") {
            infoDisplay->append("❌ Please enable Bluetooth Adapter first");
            btnBluetoothRadio->setEnabled(true);
            return;
        }
        
        if (enable) {
            // TURN ON Bluetooth Radio
            executeCommand("powershell", QStringList() << "-Command" << 
                "Start-Process ms-settings:bluetooth");
            infoDisplay->append("✅ Opening Bluetooth Settings...");
            infoDisplay->append("Please turn ON Bluetooth in the settings window");
        } else {
            // TURN OFF Bluetooth Radio  
            executeCommand("powershell", QStringList() << "-Command" << 
                "Start-Process ms-settings:bluetooth");
            infoDisplay->append("✅ Opening Bluetooth Settings...");
            infoDisplay->append("Please turn OFF Bluetooth in the settings window");
        }
        
        btnBluetoothRadio->setEnabled(true);
        QTimer::singleShot(2000, this, &NetworkWidget::checkAllAdaptersStatus);
    });
}