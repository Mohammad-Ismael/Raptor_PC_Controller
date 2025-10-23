#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Sidebar;
class QStackedWidget;

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

    Ui::MainWindow *ui;
    Sidebar *sidebar;
    QStackedWidget *stackedWidget;
};
#endif // MAINWINDOW_H
