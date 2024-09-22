#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QDebug>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void Reload();
    void Update(QString name, QString subject, QString mark);

private slots:
    void on_pushButton_clicked();

    void on_tableWidget_cellChanged(int row, int column);

private:
    Ui::MainWindow *ui;

    bool userChange = false;
    QString address;

    void ReadConfig();
};
#endif // MAINWINDOW_H
