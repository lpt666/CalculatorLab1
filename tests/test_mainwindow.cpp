#include "mainwindow.h"

#include <QLineEdit>
#include <QPushButton>
#include <QtTest>

class CalculatorTest : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();
    void inputEditing();
    void basicArithmetic();
    void continuousOperation();
    void operatorReplacement();
    void divisionByZeroRecovery();
    void resultStartsNewEntry();
    void keyboardAndMixedInput();

private:
    void click(const char *objectName);
    QString displayText() const;

    MainWindow *m_window = nullptr;
};

void CalculatorTest::init()
{
    m_window = new MainWindow;
    m_window->show();
    QTest::qWait(10);
}

void CalculatorTest::cleanup()
{
    delete m_window;
    m_window = nullptr;
}

void CalculatorTest::click(const char *objectName)
{
    QPushButton *button = m_window->findChild<QPushButton *>(objectName);
    QVERIFY2(button, objectName);
    QTest::mouseClick(button, Qt::LeftButton);
}

QString CalculatorTest::displayText() const
{
    QLineEdit *display = m_window->findChild<QLineEdit *>("displayEdit");
    return display ? display->text() : QString();
}

void CalculatorTest::inputEditing()
{
    click("button1");
    click("buttonDecimal");
    click("button2");
    click("buttonDecimal");
    click("button3");
    QCOMPARE(displayText(), QString("1.23"));

    click("buttonBackspace");
    QCOMPARE(displayText(), QString("1.2"));
    click("buttonClearEntry");
    QCOMPARE(displayText(), QString("0"));
}

void CalculatorTest::basicArithmetic()
{
    click("button1");
    click("button2");
    click("buttonDecimal");
    click("button5");
    click("buttonAdd");
    click("button3");
    click("buttonDecimal");
    click("button2");
    click("buttonEquals");
    QCOMPARE(displayText(), QString("15.7"));
}

void CalculatorTest::continuousOperation()
{
    click("button2");
    click("buttonAdd");
    click("button3");
    click("buttonAdd");
    click("button4");
    click("buttonEquals");
    QCOMPARE(displayText(), QString("9"));
}

void CalculatorTest::operatorReplacement()
{
    click("button8");
    click("buttonAdd");
    click("buttonMultiply");
    click("button2");
    click("buttonEquals");
    QCOMPARE(displayText(), QString("16"));
}

void CalculatorTest::divisionByZeroRecovery()
{
    click("button9");
    click("buttonDivide");
    click("button0");
    click("buttonEquals");
    QCOMPARE(displayText(), QString::fromUtf8("不能除以零"));

    click("button7");
    QCOMPARE(displayText(), QString("7"));
}

void CalculatorTest::resultStartsNewEntry()
{
    click("button5");
    click("buttonAdd");
    click("button5");
    click("buttonEquals");
    QCOMPARE(displayText(), QString("10"));
    click("button3");
    QCOMPARE(displayText(), QString("3"));
}

void CalculatorTest::keyboardAndMixedInput()
{
    QTest::keyClicks(m_window, "12.5+3.2");
    QTest::keyClick(m_window, Qt::Key_Return);
    QCOMPARE(displayText(), QString("15.7"));

    QTest::keyClick(m_window, Qt::Key_Escape);
    click("button8");
    QTest::keyClick(m_window, Qt::Key_Plus);
    click("button2");
    QTest::keyClick(m_window, Qt::Key_Return);
    QCOMPARE(displayText(), QString("10"));
}

QTEST_MAIN(CalculatorTest)
#include "test_mainwindow.moc"
