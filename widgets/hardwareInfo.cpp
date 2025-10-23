#include "HardwareInfo.h"
#include <QProcess>
#include <QDebug>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QFrame>
#include <QScrollArea>
#include <QGridLayout>
#include <QDateTime>

HardwareInfo::HardwareInfo(QWidget *parent)
    : QWidget(parent)
    , mainLayout(nullptr)
    , scrollArea(nullptr)
    , contentFrame(nullptr)
    , infoDisplay(nullptr)
    , hardwareTimer(nullptr)
{
    setupUI();
    hardwareTimer = new QTimer(this);
    connect(hardwareTimer, &QTimer::timeout, this, &HardwareInfo::updateHardwareInfo);
    hardwareTimer->start(3000); // Update every 3 seconds

    fetchHardwareData();
}

HardwareInfo::~HardwareInfo()
{
    if (hardwareTimer) {
        hardwareTimer->stop();
        delete hardwareTimer;
    }
}

void HardwareInfo::setupUI()
{
    // Main layout
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Set background
    setStyleSheet("background-color: #ecf0f1;");

    // Title
    QLabel *title = new QLabel("Hardware Information");
    title->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50;");
    mainLayout->addWidget(title);

    // Create scroll area for hardware info
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    contentFrame = new QFrame();
    contentFrame->setStyleSheet(
        "QFrame {"
        "    background-color: white;"
        "    border: 2px solid #3498db;"
        "    border-radius: 8px;"
        "    padding: 20px;"
        "}"
    );

    QVBoxLayout *frameLayout = new QVBoxLayout(contentFrame);
    
    // Info display with better formatting
    infoDisplay = new QTextEdit();
    infoDisplay->setStyleSheet(
        "QTextEdit {"
        "    background-color: white;"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 5px;"
        "    padding: 15px;"
        "    font-family: 'Consolas', 'Courier New';"
        "    font-size: 12px;"
        "    color: #2c3e50;"
        "    min-height: 400px;"
        "}"
    );
    infoDisplay->setReadOnly(true);
    infoDisplay->setPlaceholderText("Gathering hardware information...");

    frameLayout->addWidget(infoDisplay);
    scrollArea->setWidget(contentFrame);
    mainLayout->addWidget(scrollArea);

    // Status label
    QLabel *statusLabel = new QLabel("Hardware information updates every 3 seconds");
    statusLabel->setStyleSheet("font-size: 11px; color: #7f8c8d; font-style: italic;");
    mainLayout->addWidget(statusLabel);
}

QString HardwareInfo::executeCommand(const QString &command, const QStringList &arguments)
{
    QProcess process;
    process.start(command, arguments);
    process.waitForFinished(5000); // 5 second timeout
    return QString::fromLocal8Bit(process.readAllStandardOutput());
}

QString HardwareInfo::formatBytes(quint64 bytes)
{
    const quint64 KB = 1024;
    const quint64 MB = KB * 1024;
    const quint64 GB = MB * 1024;
    const quint64 TB = GB * 1024;

    if (bytes >= TB) {
        return QString("%1 TB").arg(QString::number(bytes / (double)TB, 'f', 2));
    } else if (bytes >= GB) {
        return QString("%1 GB").arg(QString::number(bytes / (double)GB, 'f', 2));
    } else if (bytes >= MB) {
        return QString("%1 MB").arg(QString::number(bytes / (double)MB, 'f', 2));
    } else if (bytes >= KB) {
        return QString("%1 KB").arg(QString::number(bytes / (double)KB, 'f', 2));
    } else {
        return QString("%1 bytes").arg(bytes);
    }
}

void HardwareInfo::fetchHardwareData()
{
    fetchCPUInfo();
    fetchGPUInfo();
    fetchMotherboardInfo();
    fetchRAMInfo();
    fetchStorageInfo();
    fetchNetworkInfo();
    fetchUSBInfo();
}

void HardwareInfo::updateHardwareInfo()
{
    fetchHardwareData();

    QString infoText;
    infoText += "🖥️ SYSTEM HARDWARE INFORMATION\n";
    infoText += "══════════════════════════════\n\n";

    // CPU Information
    infoText += "🔹 PROCESSOR (CPU)\n";
    infoText += "──────────────────\n";
    infoText += QString("%1\n\n").arg(cpuInfo.isEmpty() ? "Not available" : cpuInfo);

    // Motherboard Information
    infoText += "🔧 MOTHERBOARD\n";
    infoText += "──────────────\n";
    infoText += QString("%1\n\n").arg(motherboardInfo.isEmpty() ? "Not available" : motherboardInfo);

    // GPU Information
    infoText += "🎮 GRAPHICS CARD (GPU)\n";
    infoText += "─────────────────────\n";
    infoText += QString("%1\n\n").arg(gpuInfo.isEmpty() ? "Not available" : gpuInfo);

    // RAM Information
    infoText += "💾 MEMORY (RAM)\n";
    infoText += "───────────────\n";
    infoText += QString("%1\n\n").arg(ramInfo.isEmpty() ? "Not available" : ramInfo);

    // Storage Information
    infoText += "💿 STORAGE DRIVES\n";
    infoText += "────────────────\n";
    infoText += QString("%1\n\n").arg(storageInfo.isEmpty() ? "Not available" : storageInfo);

    // Network Information
    infoText += "🌐 NETWORK ADAPTERS\n";
    infoText += "──────────────────\n";
    infoText += QString("%1\n\n").arg(networkInfo.isEmpty() ? "Not available" : networkInfo);

    // USB Information
    infoText += "🔌 USB DEVICES\n";
    infoText += "──────────────\n";
    infoText += QString("%1\n\n").arg(usbInfo.isEmpty() ? "Not available" : usbInfo);

    infoText += "⏱️ Last updated: " + QDateTime::currentDateTime().toString("hh:mm:ss AP");
    
    infoDisplay->setText(infoText);
}

void HardwareInfo::fetchCPUInfo()
{
    QString output = executeCommand("wmic", QStringList() << "cpu" << "get" << "Name,NumberOfCores,NumberOfLogicalProcessors,MaxClockSpeed" << "/format:list");
    
    if (!output.isEmpty()) {
        QStringList lines = output.split('\n');
        QString name, cores, logicalProcessors, speed;
        
        for (const QString &line : lines) {
            if (line.startsWith("Name=")) {
                name = line.mid(5).trimmed();
            } else if (line.startsWith("NumberOfCores=")) {
                cores = line.mid(14).trimmed();
            } else if (line.startsWith("NumberOfLogicalProcessors=")) {
                logicalProcessors = line.mid(26).trimmed();
            } else if (line.startsWith("MaxClockSpeed=")) {
                speed = line.mid(14).trimmed();
            }
        }
        
        if (!name.isEmpty()) {
            cpuInfo = name;
            if (!cores.isEmpty() && !logicalProcessors.isEmpty()) {
                cpuInfo += QString("\n   Cores: %1 Physical, %2 Logical").arg(cores).arg(logicalProcessors);
            }
            if (!speed.isEmpty()) {
                double speedGHz = speed.toDouble() / 1000;
                cpuInfo += QString("\n   Clock Speed: %1 GHz").arg(QString::number(speedGHz, 'f', 1));
            }
        }
    }
    
    if (cpuInfo.isEmpty()) {
        cpuInfo = "Unable to retrieve CPU information";
    }
}

void HardwareInfo::fetchGPUInfo()
{
    QString name, driverVersion;
    
    // First get basic GPU info from WMIC
    QString output = executeCommand("wmic", QStringList() << "path" << "win32_videocontroller" << "get" << "Name,DriverVersion" << "/format:list");
    
    if (!output.isEmpty()) {
        QStringList lines = output.split('\n');
        for (const QString &line : lines) {
            if (line.startsWith("Name=")) {
                name = line.mid(5).trimmed();
            } else if (line.startsWith("DriverVersion=")) {
                driverVersion = line.mid(14).trimmed();
            }
        }
    }
    
    if (!name.isEmpty()) {
        gpuInfo = name;
        
        // Use nvidia-smi for accurate GPU information (works for NVIDIA cards)
        QString nvidiaOutput = executeCommand("nvidia-smi", QStringList() << "--query-gpu=name,memory.total" << "--format=csv");
        
        if (!nvidiaOutput.trimmed().isEmpty() && !nvidiaOutput.contains("not found")) {
            QStringList lines = nvidiaOutput.split('\n');
            
            // Skip header line and process data lines
            for (int i = 1; i < lines.size(); ++i) {
                QString line = lines[i].trimmed();
                if (!line.isEmpty()) {
                    // Parse CSV format: "GPU Name, memory.total [MiB]"
                    QStringList parts = line.split(',');
                    if (parts.size() >= 2) {
                        QString gpuName = parts[0].trimmed();
                        QString memoryStr = parts[1].trimmed();
                        
                        // Check if this GPU matches our detected GPU
                        if (gpuName.contains(name, Qt::CaseInsensitive) || name.contains(gpuName, Qt::CaseInsensitive)) {
                            // Extract memory value and convert from MiB to GB
                            QRegularExpression memoryRegex("(\\d+)");
                            QRegularExpressionMatch match = memoryRegex.match(memoryStr);
                            if (match.hasMatch()) {
                                quint64 memoryMiB = match.captured(1).toULongLong();
                                double memoryGB = memoryMiB / 1024.0;
                                gpuInfo = gpuName; // Use the name from nvidia-smi
                                gpuInfo += QString("\n   VRAM: %1 GB").arg(QString::number(memoryGB, 'f', 2));
                                break;
                            }
                        }
                    }
                }
            }
        }
        
        // If nvidia-smi didn't work or we didn't find matching GPU, try alternative methods
        if (!gpuInfo.contains("VRAM:")) {
            // Try using nvidia-smi with simpler query
            QString simpleNvidiaOutput = executeCommand("nvidia-smi", QStringList() << "--query-gpu=memory.total" << "--format=csv,noheader,nounits");
            
            if (!simpleNvidiaOutput.trimmed().isEmpty()) {
                QString memoryLine = simpleNvidiaOutput.trimmed().split('\n').first().trimmed();
                bool ok;
                quint64 memoryMiB = memoryLine.toULongLong(&ok);
                if (ok && memoryMiB > 0) {
                    double memoryGB = memoryMiB / 1024.0;
                    gpuInfo += QString("\n   VRAM: %1 GB").arg(QString::number(memoryGB, 'f', 2));
                }
            } else {
                // Fallback to PowerShell method for non-NVIDIA cards
                QString psCommand = 
                    "Get-CimInstance -ClassName Win32_VideoController | Where-Object { $_.Name -like '*" + name + "*' } | "
                    "Select-Object @{Name='VRAM_GB'; Expression={[math]::Round($_.AdapterRAM / 1GB, 2)}}";
                
                QString psOutput = executeCommand("powershell", QStringList() << "-Command" << psCommand);
                
                if (!psOutput.trimmed().isEmpty()) {
                    QRegularExpression vramRegex("VRAM_GB\\s*:\\s*([0-9.]+)");
                    QRegularExpressionMatch match = vramRegex.match(psOutput);
                    if (match.hasMatch()) {
                        QString vram = match.captured(1) + " GB";
                        gpuInfo += QString("\n   VRAM: %1").arg(vram);
                    }
                }
            }
        }
        
        // Add driver version if available
        if (!driverVersion.isEmpty()) {
            // Try to get driver version from nvidia-smi for more accuracy
            QString driverOutput = executeCommand("nvidia-smi", QStringList() << "--query-gpu=driver_version" << "--format=csv,noheader");
            QString nvidiaDriver = driverOutput.trimmed();
            
            if (!nvidiaDriver.isEmpty() && nvidiaDriver != "N/A") {
                gpuInfo += QString("\n   Driver: %1").arg(nvidiaDriver);
            } else {
                gpuInfo += QString("\n   Driver: %1").arg(driverVersion);
            }
        }
        
        // Additional GPU information from nvidia-smi
        QString additionalInfo = executeCommand("nvidia-smi", QStringList() << "--query-gpu=temperature.gpu,utilization.gpu,power.draw" << "--format=csv,noheader");
        if (!additionalInfo.trimmed().isEmpty() && !additionalInfo.contains("N/A")) {
            QStringList additionalParts = additionalInfo.trimmed().split(',');
            if (additionalParts.size() >= 3) {
                QString temp = additionalParts[0].trimmed();
                QString utilization = additionalParts[1].trimmed();
                QString power = additionalParts[2].trimmed();
                
                if (temp != "N/A" && temp != "[Not Supported]") {
                    gpuInfo += QString("\n   Temperature: %1°C").arg(temp);
                }
                if (utilization != "N/A" && utilization != "[Not Supported]") {
                    gpuInfo += QString("\n   Utilization: %1%").arg(utilization);
                }
                if (power != "N/A" && power != "[Not Supported]" && !power.contains("W]")) {
                    gpuInfo += QString("\n   Power: %1").arg(power.trimmed());
                }
            }
        }
    }
    
    if (gpuInfo.isEmpty()) {
        gpuInfo = "Unable to retrieve GPU information";
    }
}
void HardwareInfo::fetchMotherboardInfo()
{
    QString output = executeCommand("wmic", QStringList() << "baseboard" << "get" << "Product,Manufacturer,Version" << "/format:list");
    
    if (!output.isEmpty()) {
        QStringList lines = output.split('\n');
        QString product, manufacturer, version;
        
        for (const QString &line : lines) {
            if (line.startsWith("Product=")) {
                product = line.mid(8).trimmed();
            } else if (line.startsWith("Manufacturer=")) {
                manufacturer = line.mid(13).trimmed();
            } else if (line.startsWith("Version=")) {
                version = line.mid(8).trimmed();
            }
        }
        
        if (!manufacturer.isEmpty() && !product.isEmpty()) {
            motherboardInfo = QString("%1 %2").arg(manufacturer).arg(product);
            if (!version.isEmpty() && version != "Default string") {
                motherboardInfo += QString("\n   Version: %1").arg(version);
            }
        }
    }
    
    if (motherboardInfo.isEmpty()) {
        motherboardInfo = "Unable to retrieve motherboard information";
    }
}

void HardwareInfo::fetchRAMInfo()
{
    QString output = executeCommand("wmic", QStringList() << "memorychip" << "get" << "Capacity,Speed,Manufacturer" << "/format:list");
    
    if (!output.isEmpty()) {
        QStringList lines = output.split('\n');
        quint64 totalRAM = 0;
        QStringList manufacturers;
        QStringList speeds;
        
        for (const QString &line : lines) {
            if (line.startsWith("Capacity=")) {
                quint64 capacity = line.mid(9).trimmed().toULongLong();
                if (capacity > 0) {
                    totalRAM += capacity;
                }
            } else if (line.startsWith("Manufacturer=")) {
                QString manufacturer = line.mid(13).trimmed();
                if (!manufacturer.isEmpty() && manufacturer != "Unknown" && !manufacturers.contains(manufacturer)) {
                    manufacturers.append(manufacturer);
                }
            } else if (line.startsWith("Speed=")) {
                QString speed = line.mid(6).trimmed();
                if (!speed.isEmpty() && speed != "0" && !speeds.contains(speed)) {
                    speeds.append(speed + " MHz");
                }
            }
        }
        
        if (totalRAM > 0) {
            ramInfo = formatBytes(totalRAM);
            if (!manufacturers.isEmpty()) {
                ramInfo += QString("\n   Manufacturer: %1").arg(manufacturers.join(", "));
            }
            if (!speeds.isEmpty()) {
                ramInfo += QString("\n   Speed: %1").arg(speeds.join(", "));
            }
        }
    }
    
    if (ramInfo.isEmpty()) {
        ramInfo = "Unable to retrieve RAM information";
    }
}

void HardwareInfo::fetchStorageInfo()
{
    QString output = executeCommand("wmic", QStringList() << "diskdrive" << "get" << "Model,Size,MediaType" << "/format:list");
    
    if (!output.isEmpty()) {
        QStringList lines = output.split('\n');
        QStringList drives;
        QString currentModel, currentSize, currentType;
        
        for (const QString &line : lines) {
            if (line.startsWith("Model=")) {
                if (!currentModel.isEmpty() && !currentSize.isEmpty()) {
                    QString driveInfo = currentModel;
                    if (!currentSize.isEmpty() && currentSize != "0") {
                        quint64 sizeBytes = currentSize.toULongLong();
                        driveInfo += QString(" (%1)").arg(formatBytes(sizeBytes));
                    }
                    if (!currentType.isEmpty() && currentType != "Unknown") {
                        driveInfo += QString(" [%1]").arg(currentType);
                    }
                    drives.append("   • " + driveInfo);
                }
                currentModel = line.mid(6).trimmed();
                currentSize.clear();
                currentType.clear();
            } else if (line.startsWith("Size=")) {
                currentSize = line.mid(5).trimmed();
            } else if (line.startsWith("MediaType=")) {
                currentType = line.mid(10).trimmed();
            }
        }
        
        // Add the last drive
        if (!currentModel.isEmpty() && !currentSize.isEmpty()) {
            QString driveInfo = currentModel;
            if (!currentSize.isEmpty() && currentSize != "0") {
                quint64 sizeBytes = currentSize.toULongLong();
                driveInfo += QString(" (%1)").arg(formatBytes(sizeBytes));
            }
            if (!currentType.isEmpty() && currentType != "Unknown") {
                driveInfo += QString(" [%1]").arg(currentType);
            }
            drives.append("   • " + driveInfo);
        }
        
        if (!drives.isEmpty()) {
            storageInfo = drives.join("\n");
        }
    }
    
    if (storageInfo.isEmpty()) {
        storageInfo = "Unable to retrieve storage information";
    }
}

void HardwareInfo::fetchNetworkInfo()
{
    QString output = executeCommand("wmic", QStringList() << "nic" << "where" << "NetEnabled=true" << "get" << "Name,MACAddress,AdapterType" << "/format:list");
    
    if (!output.isEmpty()) {
        QStringList lines = output.split('\n');
        QStringList adapters;
        QString currentName, currentMAC, currentType;
        
        for (const QString &line : lines) {
            if (line.startsWith("Name=")) {
                if (!currentName.isEmpty()) {
                    QString adapterInfo = currentName;
                    if (!currentType.isEmpty()) {
                        adapterInfo += QString(" (%1)").arg(currentType);
                    }
                    if (!currentMAC.isEmpty() && currentMAC.length() > 5) {
                        // Format MAC address
                        QString formattedMAC;
                        for (int i = 0; i < currentMAC.length(); i += 2) {
                            if (!formattedMAC.isEmpty()) formattedMAC += ":";
                            formattedMAC += currentMAC.mid(i, 2);
                        }
                        adapterInfo += QString("\n      MAC: %1").arg(formattedMAC.toUpper());
                    }
                    adapters.append("   • " + adapterInfo);
                }
                currentName = line.mid(5).trimmed();
                currentMAC.clear();
                currentType.clear();
            } else if (line.startsWith("MACAddress=")) {
                currentMAC = line.mid(11).trimmed();
            } else if (line.startsWith("AdapterType=")) {
                currentType = line.mid(12).trimmed();
            }
        }
        
        // Add the last adapter
        if (!currentName.isEmpty()) {
            QString adapterInfo = currentName;
            if (!currentType.isEmpty()) {
                adapterInfo += QString(" (%1)").arg(currentType);
            }
            if (!currentMAC.isEmpty() && currentMAC.length() > 5) {
                // Format MAC address
                QString formattedMAC;
                for (int i = 0; i < currentMAC.length(); i += 2) {
                    if (!formattedMAC.isEmpty()) formattedMAC += ":";
                    formattedMAC += currentMAC.mid(i, 2);
                }
                adapterInfo += QString("\n      MAC: %1").arg(formattedMAC.toUpper());
            }
            adapters.append("   • " + adapterInfo);
        }
        
        if (!adapters.isEmpty()) {
            networkInfo = adapters.join("\n");
        }
    }
    
    if (networkInfo.isEmpty()) {
        networkInfo = "No active network adapters found";
    }
}

void HardwareInfo::fetchUSBInfo()
{
    QString output = executeCommand("wmic", QStringList() << "path" << "Win32_USBControllerDevice" << "get" << "Dependent" << "/format:list");
    
    if (!output.isEmpty()) {
        QStringList lines = output.split('\n');
        int usbDeviceCount = 0;
        
        for (const QString &line : lines) {
            if (line.startsWith("Dependent=")) {
                QString deviceID = line.mid(10).trimmed();
                // Count unique USB devices (excluding hubs and controllers)
                if (deviceID.contains("VID_") && !deviceID.contains("ROOT_HUB")) {
                    usbDeviceCount++;
                }
            }
        }
        
        usbInfo = QString("%1 connected USB devices").arg(usbDeviceCount);
    }
    
    if (usbInfo.isEmpty()) {
        usbInfo = "Unable to retrieve USB device information";
    }
}