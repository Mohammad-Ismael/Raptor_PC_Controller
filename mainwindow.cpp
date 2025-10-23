#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "sidebar.h"
#include "widgets/networkwidget.h"
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , sidebar(nullptr)
    , stackedWidget(nullptr)
{
    ui->setupUi(this);
    setupUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    setWindowTitle("Raptor PC Controller");
    setMinimumSize(900, 600);
    
    // Create main layout
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Create and setup sidebar
    sidebar = new Sidebar(this);
    mainLayout->addWidget(sidebar);
    
    // Setup content area with stacked widget
    setupContentArea();
    mainLayout->addWidget(stackedWidget, 1); // stretch factor 1
    
    // Connect sidebar signals
    connect(sidebar, &Sidebar::categoryChanged, this, &MainWindow::onCategoryChanged);
    
    // Set the layout to central widget
    ui->centralwidget->setLayout(mainLayout);
}

void MainWindow::setupContentArea()
{
    stackedWidget = new QStackedWidget();
    stackedWidget->setStyleSheet("QStackedWidget { background-color: #ecf0f1; }");
    
    // Add default page
    QWidget *defaultPage = new QWidget();
    defaultPage->setStyleSheet("background-color: #ecf0f1;"); // Set background
    QVBoxLayout *defaultLayout = new QVBoxLayout(defaultPage);
    QLabel *defaultLabel = new QLabel("Select a category from the sidebar");
    defaultLabel->setStyleSheet("font-size: 18px; color: #7f8c8d;");
    defaultLabel->setAlignment(Qt::AlignCenter);
    defaultLayout->addWidget(defaultLabel);
    stackedWidget->addWidget(defaultPage);
    
    // Add network widget
    NetworkWidget *networkWidget = new NetworkWidget();
    stackedWidget->addWidget(networkWidget);
    stackedWidget->setCurrentWidget(defaultPage); // Show default page first
}

void MainWindow::onCategoryChanged(const QString &category)
{
    if (category == "Network") {
        // For now, just show the network widget (index 1)
        // Later we can create proper mapping for all categories
        stackedWidget->setCurrentIndex(1);
    } else {
        // Show default page for other categories (for now)
        stackedWidget->setCurrentIndex(0);
        
        // Update default page message
        QWidget *currentWidget = stackedWidget->currentWidget();
        QLabel *label = currentWidget->findChild<QLabel*>();
        if (label) {
            label->setText(category + " - Coming Soon!\n\nSelect a category from the sidebar");
        }
    }
}