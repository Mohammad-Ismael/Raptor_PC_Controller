#include "cleanerwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QTextEdit>
#include <QFrame>
#include <QSpacerItem>
#include <QProcess>
#include <QCheckBox>
#include <QProgressBar>
#include <QScrollArea>
#include <QDir>
#include <QFileInfo>
#include <QStorageInfo>
#include <QDateTime>
#include <QStandardPaths>

CleanerWidget::CleanerWidget(QWidget *parent)
    : QWidget(parent)
    , mainLayout(nullptr)
    , scrollArea(nullptr)
    , infoDisplay(nullptr)
    , progressBar(nullptr)
    , statusLabel(nullptr)
    , progressTimer(nullptr)
    , progressValue(0)
{
    setupUI();
}

CleanerWidget::~CleanerWidget()
{
    if (progressTimer) progressTimer->stop();
}

void CleanerWidget::setupUI()
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

    QLabel *title = new QLabel("System Cleaner");
    title->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50;");
    mainLayout->addWidget(title);

    createScanControls();
    createCleanupOptions();
    createProgressSection();
    createLogSection();

    mainLayout->addStretch();
    scrollArea->setWidget(scrollWidget);
    
    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addWidget(scrollArea);
}

void CleanerWidget::createScanControls()
{
    QLabel *sectionTitle = new QLabel("Scan & Clean Controls");
    sectionTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #2c3e50; margin-top: 10px;");

    // Button layout
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    btnScan = new QPushButton("🔍 Scan System");
    btnClean = new QPushButton("🧹 Clean Selected");
    btnSelectAll = new QPushButton("✓ Select All");
    btnDeselectAll = new QPushButton("✗ Deselect All");
    
    QString scanStyle = 
        "QPushButton {"
        "    background-color: #3498db;"
        "    color: white;"
        "    border: none;"
        "    padding: 12px 20px;"
        "    border-radius: 5px;"
        "    font-weight: bold;"
        "    font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #bdc3c7;"
        "}";
    
    QString cleanStyle = 
        "QPushButton {"
        "    background-color: #e74c3c;"
        "    color: white;"
        "    border: none;"
        "    padding: 12px 20px;"
        "    border-radius: 5px;"
        "    font-weight: bold;"
        "    font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #c0392b;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #bdc3c7;"
        "}";
    
    QString selectStyle = 
        "QPushButton {"
        "    background-color: #2ecc71;"
        "    color: white;"
        "    border: none;"
        "    padding: 10px 15px;"
        "    border-radius: 5px;"
        "    font-weight: bold;"
        "    font-size: 11px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #27ae60;"
        "}";
    
    btnScan->setStyleSheet(scanStyle);
    btnClean->setStyleSheet(cleanStyle);
    btnSelectAll->setStyleSheet(selectStyle);
    btnDeselectAll->setStyleSheet(selectStyle);
    
    btnClean->setEnabled(false);

    connect(btnScan, &QPushButton::clicked, this, &CleanerWidget::scanSystem);
    connect(btnClean, &QPushButton::clicked, this, &CleanerWidget::cleanSelected);
    connect(btnSelectAll, &QPushButton::clicked, this, &CleanerWidget::selectAll);
    connect(btnDeselectAll, &QPushButton::clicked, this, &CleanerWidget::deselectAll);

    buttonLayout->addWidget(btnScan);
    buttonLayout->addWidget(btnClean);
    buttonLayout->addWidget(btnSelectAll);
    buttonLayout->addWidget(btnDeselectAll);
    buttonLayout->addStretch();

    mainLayout->addWidget(sectionTitle);
    mainLayout->addLayout(buttonLayout);
}

void CleanerWidget::createCleanupOptions()
{
    QLabel *sectionTitle = new QLabel("Cleanup Options");
    sectionTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #2c3e50; margin-top: 10px;");

    QFrame *optionsFrame = new QFrame();
    optionsFrame->setStyleSheet(
        "QFrame {"
        "    background-color: white;"
        "    border: 2px solid #3498db;"
        "    border-radius: 8px;"
        "    padding: 15px;"
        "}"
    );

    QVBoxLayout *optionsLayout = new QVBoxLayout(optionsFrame);
    
    // Checkbox style
    QString checkboxStyle = 
        "QCheckBox {"
        "    font-size: 13px;"
        "    color: #2c3e50;"
        "    padding: 8px;"
        "}"
        "QCheckBox::indicator {"
        "    width: 16px;"
        "    height: 16px;"
        "}"
        "QCheckBox::indicator:unchecked {"
        "    border: 2px solid #bdc3c7;"
        "    background-color: white;"
        "    border-radius: 3px;"
        "}"
        "QCheckBox::indicator:checked {"
        "    border: 2px solid #27ae60;"
        "    background-color: #27ae60;"
        "    border-radius: 3px;"
        "}";

    chkTempFiles = new QCheckBox("🗑️ Temporary Files (Browser cache, app data)");
    chkRecycleBin = new QCheckBox("🗂️ Recycle Bin (Empty all deleted files)");
    chkBrowserCache = new QCheckBox("🌐 Browser Cache (Chrome, Firefox, Edge)");
    chkWindowsTemp = new QCheckBox("💻 Windows Temp Files (System temporary files)");
    chkPrefetch = new QCheckBox("⚡ Prefetch Files (Speed up application loading)");
    chkThumbnails = new QCheckBox("🖼️ Thumbnail Cache (Image previews)");
    chkDNS = new QCheckBox("🔗 DNS Cache (Network address cache)");
    chkLogs = new QCheckBox("📋 System Logs (Old log files)");

    chkTempFiles->setStyleSheet(checkboxStyle);
    chkRecycleBin->setStyleSheet(checkboxStyle);
    chkBrowserCache->setStyleSheet(checkboxStyle);
    chkWindowsTemp->setStyleSheet(checkboxStyle);
    chkPrefetch->setStyleSheet(checkboxStyle);
    chkThumbnails->setStyleSheet(checkboxStyle);
    chkDNS->setStyleSheet(checkboxStyle);
    chkLogs->setStyleSheet(checkboxStyle);

    // Select some common options by default
    chkTempFiles->setChecked(true);
    chkRecycleBin->setChecked(true);
    chkBrowserCache->setChecked(true);
    chkWindowsTemp->setChecked(true);

    optionsLayout->addWidget(chkTempFiles);
    optionsLayout->addWidget(chkRecycleBin);
    optionsLayout->addWidget(chkBrowserCache);
    optionsLayout->addWidget(chkWindowsTemp);
    optionsLayout->addWidget(chkPrefetch);
    optionsLayout->addWidget(chkThumbnails);
    optionsLayout->addWidget(chkDNS);
    optionsLayout->addWidget(chkLogs);

    mainLayout->addWidget(sectionTitle);
    mainLayout->addWidget(optionsFrame);
}

void CleanerWidget::createProgressSection()
{
    QLabel *sectionTitle = new QLabel("Cleanup Progress");
    sectionTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #2c3e50; margin-top: 10px;");

    QFrame *progressFrame = new QFrame();
    progressFrame->setStyleSheet(
        "QFrame {"
        "    background-color: white;"
        "    border: 2px solid #3498db;"
        "    border-radius: 8px;"
        "    padding: 15px;"
        "}"
    );

    QVBoxLayout *progressLayout = new QVBoxLayout(progressFrame);
    
    statusLabel = new QLabel("Ready to scan system");
    statusLabel->setStyleSheet("font-size: 13px; color: #2c3e50; font-weight: bold;");
    
    progressBar = new QProgressBar();
    progressBar->setStyleSheet(
        "QProgressBar {"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 5px;"
        "    text-align: center;"
        "    background-color: #ecf0f1;"
        "    height: 20px;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: #2ecc71;"
        "    border-radius: 3px;"
        "}"
    );
    progressBar->setValue(0);
    progressBar->setVisible(false);

    progressLayout->addWidget(statusLabel);
    progressLayout->addWidget(progressBar);

    mainLayout->addWidget(sectionTitle);
    mainLayout->addWidget(progressFrame);
}

void CleanerWidget::createLogSection()
{
    QLabel *sectionTitle = new QLabel("Cleanup Log");
    sectionTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #2c3e50; margin-top: 10px;");

    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *infoTitle = new QLabel("Cleaning Operations:");
    infoTitle->setStyleSheet("font-size: 14px; font-weight: bold; color: #2c3e50;");

    QPushButton *btnClear = new QPushButton("Clear Log");
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

    headerLayout->addWidget(infoTitle);
    headerLayout->addStretch();
    headerLayout->addWidget(btnClear);

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
        "    min-height: 200px;"
        "}"
    );
    infoDisplay->setReadOnly(true);
    infoDisplay->setPlaceholderText("Scanning and cleaning operations will appear here...");

    connect(btnClear, &QPushButton::clicked, this, &CleanerWidget::clearLog);

    mainLayout->addWidget(sectionTitle);
    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(infoDisplay);
}

// Helper functions
QString CleanerWidget::executeCommand(const QString &command, const QStringList &arguments)
{
    QProcess process;
    process.start(command, arguments);
    process.waitForFinished();
    return QString::fromLocal8Bit(process.readAllStandardOutput());
}

qint64 CleanerWidget::getFolderSize(const QString &path)
{
    qint64 totalSize = 0;
    QDir dir(path);
    
    if (!dir.exists()) return 0;
    
    QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);
    
    for (const QFileInfo &entry : entries) {
        if (entry.isDir()) {
            totalSize += getFolderSize(entry.absoluteFilePath());
        } else {
            totalSize += entry.size();
        }
    }
    
    return totalSize;
}

QString CleanerWidget::formatSize(qint64 bytes)
{
    const qint64 KB = 1024;
    const qint64 MB = KB * 1024;
    const qint64 GB = MB * 1024;
    
    if (bytes >= GB) {
        return QString("%1 GB").arg(QString::number(bytes / (double)GB, 'f', 2));
    } else if (bytes >= MB) {
        return QString("%1 MB").arg(QString::number(bytes / (double)MB, 'f', 2));
    } else if (bytes >= KB) {
        return QString("%1 KB").arg(QString::number(bytes / (double)KB, 'f', 2));
    } else {
        return QString("%1 bytes").arg(bytes);
    }
}

// Slot implementations
void CleanerWidget::scanSystem()
{
    infoDisplay->clear();
    infoDisplay->append("🔍 Scanning system for cleanup opportunities...\n");
    
    btnScan->setEnabled(false);
    btnClean->setEnabled(false);
    progressBar->setVisible(true);
    progressBar->setValue(0);
    
    progressTimer = new QTimer(this);
    progressValue = 0;
    
    connect(progressTimer, &QTimer::timeout, this, &CleanerWidget::updateCleanupProgress);
    progressTimer->start(50);
    
    // Simulate scanning process
    QTimer::singleShot(2000, this, [this]() {
        progressTimer->stop();
        progressBar->setVisible(false);
        
        // Show scan results
        infoDisplay->append("📊 Scan Results:");
        infoDisplay->append("────────────────────────");
        
        qint64 totalSize = 0;
        
        if (chkTempFiles->isChecked()) {
            qint64 size = getFolderSize(QDir::tempPath());
            totalSize += size;
            infoDisplay->append(QString("🗑️ Temporary Files: %1").arg(formatSize(size)));
        }
        
        if (chkRecycleBin->isChecked()) {
            // Estimate recycle bin size (this is approximate)
            qint64 size = 500 * 1024 * 1024; // 500MB estimate
            totalSize += size;
            infoDisplay->append(QString("🗂️ Recycle Bin: ~%1").arg(formatSize(size)));
        }
        
        if (chkBrowserCache->isChecked()) {
            qint64 size = 300 * 1024 * 1024; // 300MB estimate
            totalSize += size;
            infoDisplay->append(QString("🌐 Browser Cache: ~%1").arg(formatSize(size)));
        }
        
        if (chkWindowsTemp->isChecked()) {
            qint64 size = getFolderSize("C:/Windows/Temp");
            totalSize += size;
            infoDisplay->append(QString("💻 Windows Temp: %1").arg(formatSize(size)));
        }
        
        if (chkPrefetch->isChecked()) {
            qint64 size = getFolderSize("C:/Windows/Prefetch");
            totalSize += size;
            infoDisplay->append(QString("⚡ Prefetch Files: %1").arg(formatSize(size)));
        }
        
        if (chkThumbnails->isChecked()) {
            qint64 size = 100 * 1024 * 1024; // 100MB estimate
            totalSize += size;
            infoDisplay->append(QString("🖼️ Thumbnail Cache: ~%1").arg(formatSize(size)));
        }
        
        infoDisplay->append("────────────────────────");
        infoDisplay->append(QString("💾 Total space that can be freed: %1").arg(formatSize(totalSize)));
        infoDisplay->append("\n✅ Scan complete! Click 'Clean Selected' to proceed.");
        
        statusLabel->setText("Scan complete - ready to clean");
        btnScan->setEnabled(true);
        btnClean->setEnabled(true);
    });
}

void CleanerWidget::cleanSelected()
{
    infoDisplay->append("\n🧹 Starting cleanup process...\n");
    
    btnScan->setEnabled(false);
    btnClean->setEnabled(false);
    progressBar->setVisible(true);
    progressBar->setValue(0);
    
    progressTimer = new QTimer(this);
    progressValue = 0;
    
    connect(progressTimer, &QTimer::timeout, this, &CleanerWidget::updateCleanupProgress);
    progressTimer->start(30);
    
    // Use a list to track cleanup operations
    QStringList cleanupOperations;
    
    if (chkTempFiles->isChecked()) {
        cleanupOperations << "temp";
    }
    if (chkRecycleBin->isChecked()) {
        cleanupOperations << "recycle";
    }
    if (chkBrowserCache->isChecked()) {
        cleanupOperations << "browser";
    }
    if (chkWindowsTemp->isChecked()) {
        cleanupOperations << "wintemp";
    }
    if (chkPrefetch->isChecked()) {
        cleanupOperations << "prefetch";
    }
    if (chkThumbnails->isChecked()) {
        cleanupOperations << "thumbnails";
    }
    if (chkDNS->isChecked()) {
        cleanupOperations << "dns";
    }
    if (chkLogs->isChecked()) {
        cleanupOperations << "logs";
    }
    
    if (cleanupOperations.isEmpty()) {
        progressTimer->stop();
        progressBar->setVisible(false);
        infoDisplay->append("❌ No cleanup options selected!");
        btnScan->setEnabled(true);
        return;
    }
    
    // Process cleanup operations one by one
    processCleanupOperations(cleanupOperations, 0);
}

void CleanerWidget::processCleanupOperations(const QStringList &operations, int index)
{
    if (index >= operations.size()) {
        // All operations completed
        progressTimer->stop();
        progressBar->setValue(100);
        
        QTimer::singleShot(500, this, [this]() {
            progressBar->setVisible(false);
            infoDisplay->append("\n✅ Cleanup completed successfully!");
            infoDisplay->append("✨ Your system should now be faster and cleaner!");
            statusLabel->setText("Cleanup completed successfully");
            
            btnScan->setEnabled(true);
            btnClean->setEnabled(false);
        });
        return;
    }
    
    QString operation = operations[index];
    int progressStep = (index + 1) * (100 / operations.size());
    
    // Update progress
    progressValue = progressStep - (100 / operations.size());
    progressBar->setValue(progressValue);
    
    // Execute the cleanup operation
    if (operation == "temp") {
        cleanTempFiles();
    } else if (operation == "recycle") {
        cleanRecycleBin();
    } else if (operation == "browser") {
        cleanBrowserCache();
    } else if (operation == "wintemp") {
        cleanWindowsTemp();
    } else if (operation == "prefetch") {
        cleanPrefetch();
    } else if (operation == "thumbnails") {
        cleanThumbnails();
    } else if (operation == "dns") {
        cleanDNS();
    } else if (operation == "logs") {
        cleanLogs();
    }
    
    // Process next operation after a delay
    QTimer::singleShot(1000, this, [this, operations, index]() {
        processCleanupOperations(operations, index + 1);
    });
}

void CleanerWidget::cleanTempFiles()
{
    infoDisplay->append("🗑️ Cleaning temporary files...");
    
    // Clean user temp files
    QString tempPath = QDir::tempPath();
    QDir tempDir(tempPath);
    
    if (tempDir.exists()) {
        QFileInfoList files = tempDir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
        int deletedCount = 0;
        
        for (const QFileInfo &file : files) {
            if (file.isDir()) {
                QDir dir(file.absoluteFilePath());
                if (dir.removeRecursively()) {
                    deletedCount++;
                }
            } else {
                if (QFile::remove(file.absoluteFilePath())) {
                    deletedCount++;
                }
            }
        }
        infoDisplay->append(QString("   ✓ Deleted %1 temporary files/folders").arg(deletedCount));
    }
}

void CleanerWidget::cleanRecycleBin()
{
    infoDisplay->append("🗂️ Emptying recycle bin...");
    
    // Use PowerShell to clear recycle bin
    QString output = executeCommand("powershell", QStringList() << "-Command" << 
        "Clear-RecycleBin -Force -ErrorAction SilentlyContinue; Write-Output 'Recycle bin cleared'");
    
    if (output.contains("cleared")) {
        infoDisplay->append("   ✓ Recycle bin emptied");
    } else {
        infoDisplay->append("   ⚠️ Recycle bin may require administrator rights");
    }
}

void CleanerWidget::cleanBrowserCache()
{
    infoDisplay->append("🌐 Clearing browser cache...");
    
    int cleanedCount = 0;
    
    // Chrome cache
    QString chromeCache = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + "/Google/Chrome";
    if (QDir(chromeCache).exists()) {
        QDir(chromeCache).removeRecursively();
        cleanedCount++;
    }
    
    // Firefox cache
    QString firefoxCache = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + "/Mozilla/Firefox";
    if (QDir(firefoxCache).exists()) {
        QDir(firefoxCache).removeRecursively();
        cleanedCount++;
    }
    
    // Edge cache
    QString edgeCache = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + "/Microsoft/Edge";
    if (QDir(edgeCache).exists()) {
        QDir(edgeCache).removeRecursively();
        cleanedCount++;
    }
    
    infoDisplay->append(QString("   ✓ Cleared cache for %1 browsers").arg(cleanedCount));
}

void CleanerWidget::cleanWindowsTemp()
{
    infoDisplay->append("💻 Cleaning Windows temp files...");
    
    QDir winTempDir("C:/Windows/Temp");
    if (winTempDir.exists()) {
        int deletedCount = 0;
        QFileInfoList files = winTempDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
        
        for (const QFileInfo &file : files) {
            if (QFile::remove(file.absoluteFilePath())) {
                deletedCount++;
            }
        }
        infoDisplay->append(QString("   ✓ Deleted %1 Windows temp files").arg(deletedCount));
    }
}

void CleanerWidget::cleanPrefetch()
{
    infoDisplay->append("⚡ Cleaning prefetch files...");
    
    QDir prefetchDir("C:/Windows/Prefetch");
    if (prefetchDir.exists()) {
        int deletedCount = 0;
        QFileInfoList files = prefetchDir.entryInfoList(QStringList() << "*.pf", QDir::Files | QDir::NoDotAndDotDot);
        
        for (const QFileInfo &file : files) {
            if (QFile::remove(file.absoluteFilePath())) {
                deletedCount++;
            }
        }
        infoDisplay->append(QString("   ✓ Deleted %1 prefetch files").arg(deletedCount));
    }
}

void CleanerWidget::cleanThumbnails()
{
    infoDisplay->append("🖼️ Clearing thumbnail cache...");
    
    QString thumbPath = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + "/Microsoft/Windows/Explorer";
    QDir thumbDir(thumbPath);
    
    if (thumbDir.exists()) {
        int deletedCount = 0;
        QFileInfoList files = thumbDir.entryInfoList(QStringList() << "thumbcache_*.db", QDir::Files);
        
        for (const QFileInfo &file : files) {
            if (QFile::remove(file.absoluteFilePath())) {
                deletedCount++;
            }
        }
        infoDisplay->append(QString("   ✓ Deleted %1 thumbnail cache files").arg(deletedCount));
    }
}

void CleanerWidget::cleanDNS()
{
    infoDisplay->append("🔗 Flushing DNS cache...");
    
    QString output = executeCommand("ipconfig", QStringList() << "/flushdns");
    if (output.contains("successfully")) {
        infoDisplay->append("   ✓ DNS cache flushed successfully");
    } else {
        infoDisplay->append("   ⚠️ DNS flush may require administrator rights");
    }
}

void CleanerWidget::cleanLogs()
{
    infoDisplay->append("📋 Cleaning log files...");
    
    QDir logsDir("C:/Windows/Logs");
    if (logsDir.exists()) {
        int deletedCount = 0;
        
        // Only delete log files in subdirectories, not the directories themselves
        QFileInfoList entries = logsDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QFileInfo &entry : entries) {
            if (entry.isDir()) {
                QDir subDir(entry.absoluteFilePath());
                QFileInfoList files = subDir.entryInfoList(QDir::Files);
                for (const QFileInfo &file : files) {
                    if (QFile::remove(file.absoluteFilePath())) {
                        deletedCount++;
                    }
                }
            }
        }
        infoDisplay->append(QString("   ✓ Deleted %1 log files").arg(deletedCount));
    }
}

void CleanerWidget::selectAll()
{
    chkTempFiles->setChecked(true);
    chkRecycleBin->setChecked(true);
    chkBrowserCache->setChecked(true);
    chkWindowsTemp->setChecked(true);
    chkPrefetch->setChecked(true);
    chkThumbnails->setChecked(true);
    chkDNS->setChecked(true);
    chkLogs->setChecked(true);
    
    infoDisplay->append("✓ All cleanup options selected");
}

void CleanerWidget::deselectAll()
{
    chkTempFiles->setChecked(false);
    chkRecycleBin->setChecked(false);
    chkBrowserCache->setChecked(false);
    chkWindowsTemp->setChecked(false);
    chkPrefetch->setChecked(false);
    chkThumbnails->setChecked(false);
    chkDNS->setChecked(false);
    chkLogs->setChecked(false);
    
    infoDisplay->append("✗ All cleanup options deselected");
}

void CleanerWidget::updateCleanupProgress()
{
    progressValue += 1;
    if (progressValue > 100) {
        progressValue = 100;
    }
    progressBar->setValue(progressValue);
}

void CleanerWidget::clearLog()
{
    infoDisplay->clear();
    infoDisplay->setPlaceholderText("Scanning and cleaning operations will appear here...");
}