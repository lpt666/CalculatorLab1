#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    void connectInputButtons();
    void inputDigit(const QString &digit);
    void inputDecimal();
    void backspace();
    void clearEntry();
    void clearAll();
    void toggleSign();

    Ui::MainWindow *ui;
    bool m_startNewEntry = true;
    bool m_errorState = false;
};
#endif // MAINWINDOW_H
