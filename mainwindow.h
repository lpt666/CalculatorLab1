#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QKeyEvent;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void connectInputButtons();
    void inputDigit(const QString &digit);
    void inputDecimal();
    void backspace();
    void clearEntry();
    void clearAll();
    void toggleSign();
    void connectOperationButtons();
    void selectOperator(const QString &operation);
    void calculateResult();
    bool applyPendingOperation(double rightOperand);
    void showError(const QString &message);
    QString formatNumber(double value) const;
    QString displayOperator(const QString &operation) const;

    Ui::MainWindow *ui;
    // Completed value on the left side of the pending binary operation.
    double m_accumulator = 0.0;
    QString m_pendingOperator;
    // These flags distinguish a fresh entry, a displayed result and an error.
    bool m_startNewEntry = true;
    bool m_resultDisplayed = false;
    bool m_errorState = false;
};
#endif // MAINWINDOW_H
