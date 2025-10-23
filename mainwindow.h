#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Sidebar;
class QStackedWidget;
class QWidget;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onCategoryChanged(const QString &category);

private:
    void setupUI();
    void setupContentArea();
    void showGeneralPage();
    void showNetworkPage();
    void showCleanerPage();
    void showHardwarePage();
    void showPlaceholderPage(const QString &title);

    Ui::MainWindow *ui;
    Sidebar *sidebar;
    QStackedWidget *stackedWidget;
    
    // Track which pages are loaded
    bool networkLoaded;
    bool cleanerLoaded;
    bool hardwareLoaded;
};
#endif // MAINWINDOW_H