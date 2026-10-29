#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connectInputButtons();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectInputButtons()
{
    const QList<QPushButton *> digitButtons{
        ui->button0, ui->button1, ui->button2, ui->button3, ui->button4,
        ui->button5, ui->button6, ui->button7, ui->button8, ui->button9
    };

    for (QPushButton *button : digitButtons) {
        connect(button, &QPushButton::clicked, this, [this, button] {
            inputDigit(button->text());
        });
    }

    connect(ui->buttonDecimal, &QPushButton::clicked, this, &MainWindow::inputDecimal);
    connect(ui->buttonBackspace, &QPushButton::clicked, this, &MainWindow::backspace);
    connect(ui->buttonClearEntry, &QPushButton::clicked, this, &MainWindow::clearEntry);
    connect(ui->buttonClear, &QPushButton::clicked, this, &MainWindow::clearAll);
    connect(ui->buttonSign, &QPushButton::clicked, this, &MainWindow::toggleSign);
}

void MainWindow::inputDigit(const QString &digit)
{
    if (m_errorState) {
        clearAll();
    }

    QString text = ui->displayEdit->text();
    if (m_startNewEntry || text == "0") {
        text = digit;
    } else if (text.length() < ui->displayEdit->maxLength()) {
        text += digit;
    }

    ui->displayEdit->setText(text);
    m_startNewEntry = false;
}

void MainWindow::inputDecimal()
{
    if (m_errorState) {
        clearAll();
    }

    QString text = ui->displayEdit->text();
    if (m_startNewEntry) {
        text = "0.";
    } else if (!text.contains('.') && text.length() < ui->displayEdit->maxLength()) {
        text += '.';
    }

    ui->displayEdit->setText(text);
    m_startNewEntry = false;
}

void MainWindow::backspace()
{
    if (m_errorState) {
        clearAll();
        return;
    }
    if (m_startNewEntry) {
        return;
    }

    QString text = ui->displayEdit->text();
    text.chop(1);
    if (text.isEmpty() || text == "-") {
        text = "0";
        m_startNewEntry = true;
    }
    ui->displayEdit->setText(text);
}

void MainWindow::clearEntry()
{
    ui->displayEdit->setText("0");
    m_startNewEntry = true;
    m_errorState = false;
}

void MainWindow::clearAll()
{
    clearEntry();
    ui->expressionLabel->clear();
}

void MainWindow::toggleSign()
{
    if (m_errorState) {
        clearAll();
        return;
    }

    QString text = ui->displayEdit->text();
    if (text == "0" || text == "0.") {
        return;
    }

    if (text.startsWith('-')) {
        text.removeFirst();
    } else {
        text.prepend('-');
    }
    ui->displayEdit->setText(text);
    m_startNewEntry = false;
}
