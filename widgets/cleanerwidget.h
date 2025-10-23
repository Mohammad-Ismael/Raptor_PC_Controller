#ifndef CLEANERWIDGET_H
#define CLEANERWIDGET_H

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
class QCheckBox;
class QProgressBar;

class CleanerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CleanerWidget(QWidget *parent = nullptr);
    ~CleanerWidget();

private slots:
    void scanSystem();
    void cleanSelected();
    void selectAll();
    void deselectAll();
    void updateCleanupProgress();
    void clearLog();

private:
    void setupUI();
    void createScanControls();
    void createCleanupOptions();
    void createProgressSection();
    void createLogSection();
    
    void processCleanupOperations(const QStringList &operations, int index);
    void cleanTempFiles();
    void cleanRecycleBin();
    void cleanBrowserCache();
    void cleanWindowsTemp();
    void cleanPrefetch();
    void cleanThumbnails();
    void cleanDNS();
    void cleanLogs();

    QString executeCommand(const QString &command, const QStringList &arguments = QStringList());
    qint64 getFolderSize(const QString &path);
    QString formatSize(qint64 bytes);

    QVBoxLayout *mainLayout;
    QScrollArea *scrollArea;
    QTextEdit *infoDisplay;
    QProgressBar *progressBar;
    QLabel *statusLabel;

    // Checkboxes for cleanup options
    QCheckBox *chkTempFiles;
    QCheckBox *chkRecycleBin;
    QCheckBox *chkBrowserCache;
    QCheckBox *chkWindowsTemp;
    QCheckBox *chkPrefetch;
    QCheckBox *chkThumbnails;
    QCheckBox *chkDNS;
    QCheckBox *chkLogs;

    QPushButton *btnScan;
    QPushButton *btnClean;
    QPushButton *btnSelectAll;
    QPushButton *btnDeselectAll;

    QTimer *progressTimer;
    int progressValue;
};

#endif // CLEANERWIDGET_H