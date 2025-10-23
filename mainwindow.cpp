#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "sidebar.h"
#include "widgets/networkwidget.h"
#include "widgets/cleanerwidget.h" 
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , sidebar(nullptr)
    , stackedWidget(nullptr)
    , networkLoaded(false)
    , cleanerLoaded(false)
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
    
    // Show General page by default
    showGeneralPage();
}

void MainWindow::setupContentArea()
{
    stackedWidget = new QStackedWidget();
    stackedWidget->setStyleSheet("QStackedWidget { background-color: #ecf0f1; }");
    
    // Add placeholder pages - they will be replaced when needed
    for (int i = 0; i < 7; i++) {
        QWidget *placeholder = new QWidget();
        placeholder->setStyleSheet("background-color: #ecf0f1;");
        QVBoxLayout *placeholderLayout = new QVBoxLayout(placeholder);
        QLabel *placeholderLabel = new QLabel("Loading...");
        placeholderLabel->setStyleSheet("font-size: 18px; color: #7f8c8d;");
        placeholderLabel->setAlignment(Qt::AlignCenter);
        placeholderLayout->addWidget(placeholderLabel);
        stackedWidget->addWidget(placeholder);
    }
}

void MainWindow::onCategoryChanged(const QString &category)
{
    if (category == "General") {
        showGeneralPage();
    } else if (category == "Network") {
        showNetworkPage();
    } else if (category == "Cleaner") {
        showCleanerPage();
    } else {
        showPlaceholderPage(category);
    }
}

void MainWindow::showGeneralPage()
{
    // Create or show General page
    if (stackedWidget->count() > 0) {
        QWidget *generalPage = new QWidget();
        generalPage->setStyleSheet("background-color: #ecf0f1;");
        QVBoxLayout *generalLayout = new QVBoxLayout(generalPage);
        
        QLabel *welcomeLabel = new QLabel("Welcome to Raptor PC Controller");
        welcomeLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50; margin-bottom: 20px;");
        welcomeLabel->setAlignment(Qt::AlignCenter);
        
        QLabel *descriptionLabel = new QLabel(
            "This tool helps you manage your PC's network settings, clean up system files, "
            "and optimize performance.\n\n"
            "Select a category from the sidebar to get started."
        );
        descriptionLabel->setStyleSheet("font-size: 14px; color: #7f8c8d; text-align: center; line-height: 1.5;");
        descriptionLabel->setAlignment(Qt::AlignCenter);
        descriptionLabel->setWordWrap(true);
        
        generalLayout->addStretch();
        generalLayout->addWidget(welcomeLabel);
        generalLayout->addWidget(descriptionLabel);
        generalLayout->addStretch();
        
        if (stackedWidget->count() > 0) {
            delete stackedWidget->widget(0); // Remove old widget
            stackedWidget->insertWidget(0, generalPage);
        }
        stackedWidget->setCurrentIndex(0);
    }
}

void MainWindow::showNetworkPage()
{
    if (!networkLoaded) {
        // Load Network widget only when needed
        NetworkWidget *networkWidget = new NetworkWidget();
        if (stackedWidget->count() > 4) {
            delete stackedWidget->widget(4); // Remove old widget
            stackedWidget->insertWidget(4, networkWidget);
        }
        networkLoaded = true;
    }
    stackedWidget->setCurrentIndex(4);
}

void MainWindow::showCleanerPage()
{
    if (!cleanerLoaded) {
        // Load Cleaner widget only when needed
        CleanerWidget *cleanerWidget = new CleanerWidget();
        if (stackedWidget->count() > 3) {
            delete stackedWidget->widget(3); // Remove old widget
            stackedWidget->insertWidget(3, cleanerWidget);
        }
        cleanerLoaded = true;
    }
    stackedWidget->setCurrentIndex(3);
}

void MainWindow::showPlaceholderPage(const QString &title)
{
    // Show placeholder for other categories
    QWidget *placeholder = new QWidget();
    placeholder->setStyleSheet("background-color: #ecf0f1;");
    QVBoxLayout *placeholderLayout = new QVBoxLayout(placeholder);
    
    QLabel *titleLabel = new QLabel(title + " - Coming Soon!");
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50; margin-bottom: 20px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    
    QLabel *descriptionLabel = new QLabel(
        "This feature is under development and will be available in a future update.\n\n"
        "Check back soon for new functionality!"
    );
    descriptionLabel->setStyleSheet("font-size: 14px; color: #7f8c8d; text-align: center;");
    descriptionLabel->setAlignment(Qt::AlignCenter);
    descriptionLabel->setWordWrap(true);
    
    placeholderLayout->addStretch();
    placeholderLayout->addWidget(titleLabel);
    placeholderLayout->addWidget(descriptionLabel);
    placeholderLayout->addStretch();
    
    // Map categories to indices
    QMap<QString, int> categoryIndices = {
        {"WiFi", 1},
        {"Apps", 2},
        {"Hardware", 5},
        {"Options", 6}
    };
    
    if (categoryIndices.contains(title)) {
        int index = categoryIndices[title];
        if (stackedWidget->count() > index) {
            delete stackedWidget->widget(index);
            stackedWidget->insertWidget(index, placeholder);
        }
        stackedWidget->setCurrentIndex(index);
    }
}