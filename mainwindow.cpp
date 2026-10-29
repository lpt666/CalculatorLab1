#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFile>
#include <QKeyEvent>
#include <QPushButton>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFile styleFile(":/styles/calculator.qss");
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setStyleSheet(QString::fromUtf8(styleFile.readAll()));
    }

    connectInputButtons();
    connectOperationButtons();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QPushButton *button = nullptr;
    switch (event->key()) {
    case Qt::Key_0: button = ui->button0; break;
    case Qt::Key_1: button = ui->button1; break;
    case Qt::Key_2: button = ui->button2; break;
    case Qt::Key_3: button = ui->button3; break;
    case Qt::Key_4: button = ui->button4; break;
    case Qt::Key_5: button = ui->button5; break;
    case Qt::Key_6: button = ui->button6; break;
    case Qt::Key_7: button = ui->button7; break;
    case Qt::Key_8: button = ui->button8; break;
    case Qt::Key_9: button = ui->button9; break;
    case Qt::Key_Period:
    case Qt::Key_Comma: button = ui->buttonDecimal; break;
    case Qt::Key_Plus: button = ui->buttonAdd; break;
    case Qt::Key_Minus: button = ui->buttonSubtract; break;
    case Qt::Key_Asterisk: button = ui->buttonMultiply; break;
    case Qt::Key_Slash: button = ui->buttonDivide; break;
    case Qt::Key_Equal:
    case Qt::Key_Enter:
    case Qt::Key_Return: button = ui->buttonEquals; break;
    case Qt::Key_Backspace: button = ui->buttonBackspace; break;
    case Qt::Key_Delete: button = ui->buttonClearEntry; break;
    case Qt::Key_Escape:
    case Qt::Key_C: button = ui->buttonClear; break;
    default:
        QMainWindow::keyPressEvent(event);
        return;
    }

    // Trigger the same clicked signal as a mouse press, including visual feedback.
    button->animateClick();
    event->accept();
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
    if (m_resultDisplayed) {
        ui->expressionLabel->clear();
        m_pendingOperator.clear();
        m_resultDisplayed = false;
        m_startNewEntry = true;
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
    if (m_resultDisplayed) {
        ui->expressionLabel->clear();
        m_pendingOperator.clear();
        m_resultDisplayed = false;
        m_startNewEntry = true;
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
    m_resultDisplayed = false;
    m_errorState = false;
}

void MainWindow::clearAll()
{
    clearEntry();
    ui->expressionLabel->clear();
    m_accumulator = 0.0;
    m_pendingOperator.clear();
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

void MainWindow::connectOperationButtons()
{
    connect(ui->buttonAdd, &QPushButton::clicked, this, [this] { selectOperator("+"); });
    connect(ui->buttonSubtract, &QPushButton::clicked, this, [this] { selectOperator("-"); });
    connect(ui->buttonMultiply, &QPushButton::clicked, this, [this] { selectOperator("*"); });
    connect(ui->buttonDivide, &QPushButton::clicked, this, [this] { selectOperator("/"); });
    connect(ui->buttonEquals, &QPushButton::clicked, this, &MainWindow::calculateResult);
}

void MainWindow::selectOperator(const QString &operation)
{
    if (m_errorState) {
        return;
    }

    const double currentValue = ui->displayEdit->text().toDouble();
    if (m_pendingOperator.isEmpty()) {
        m_accumulator = currentValue;
    } else if (!m_startNewEntry) {
        if (!applyPendingOperation(currentValue)) {
            return;
        }
    }

    m_pendingOperator = operation;
    ui->expressionLabel->setText(formatNumber(m_accumulator) + " "
                                 + displayOperator(operation));
    m_startNewEntry = true;
    m_resultDisplayed = false;
}

void MainWindow::calculateResult()
{
    if (m_errorState || m_pendingOperator.isEmpty() || m_startNewEntry) {
        return;
    }

    const double rightOperand = ui->displayEdit->text().toDouble();
    const QString expression = formatNumber(m_accumulator) + " "
                               + displayOperator(m_pendingOperator) + " "
                               + formatNumber(rightOperand) + " =";
    ui->expressionLabel->setText(expression);

    if (!applyPendingOperation(rightOperand)) {
        return;
    }

    m_pendingOperator.clear();
    m_startNewEntry = true;
    m_resultDisplayed = true;
}

bool MainWindow::applyPendingOperation(double rightOperand)
{
    double result = m_accumulator;
    if (m_pendingOperator == "+") {
        result += rightOperand;
    } else if (m_pendingOperator == "-") {
        result -= rightOperand;
    } else if (m_pendingOperator == "*") {
        result *= rightOperand;
    } else if (m_pendingOperator == "/") {
        if (std::abs(rightOperand) < 1e-15) {
            showError("不能除以零");
            return false;
        }
        result /= rightOperand;
    }

    if (!std::isfinite(result)) {
        showError("结果超出范围");
        return false;
    }

    m_accumulator = result;
    ui->displayEdit->setText(formatNumber(result));
    return true;
}

void MainWindow::showError(const QString &message)
{
    ui->displayEdit->setText(message);
    m_pendingOperator.clear();
    m_startNewEntry = true;
    m_resultDisplayed = false;
    m_errorState = true;
}

QString MainWindow::formatNumber(double value) const
{
    if (std::abs(value) < 1e-15) {
        value = 0.0;
    }
    return QString::number(value, 'g', 15);
}

QString MainWindow::displayOperator(const QString &operation) const
{
    if (operation == "*") {
        return "×";
    }
    if (operation == "/") {
        return "÷";
    }
    if (operation == "-") {
        return "−";
    }
    return operation;
}
