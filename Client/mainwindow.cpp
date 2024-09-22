#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ReadConfig();
    ui->setupUi(this);
    Reload();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Reload()
{
    MainWindow::userChange = false;
    QNetworkAccessManager manager;

    QUrl url("http://" + address.toUtf8() + "/select");
    QNetworkRequest request(url);

    QNetworkReply *reply = manager.get(request);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    std::vector<QString> names;
    std::unordered_map<QString, std::vector<QChar>> mp;

    if (reply->error() == QNetworkReply::NoError)
    {
        QString response = reply->readAll();

        QString temp = "";
        QString name = "";
        for (auto &x : response)
        {
            if (x == '\xa')
            {
                if (temp[0] == 'N')
                {
                    name = "";
                    bool g = false;
                    for (auto &l : temp)
                    {
                        if (g)
                        {
                            name += l;
                        }
                        else if (l == ' ')
                        {
                            g = true;
                        }
                    }
                    temp = "";
                    names.push_back(name);
                    continue;
                }
                mp[name].push_back(temp[temp.size()-1]);
                temp = "";
                continue;
            }

            temp += x;
        }

    }
    else
    {
        qDebug() << "Error :" << reply->errorString();
    }

    int countRows = 0;
    for (auto &x : mp)
    {
        ui->tableWidget->insertRow(countRows);

        int countCols = 0;
        QTableWidgetItem *item = new QTableWidgetItem(x.first);

        ui->tableWidget->setItem(countRows, countCols, item);
        ++countCols;

        for(int i = 0; i < x.second.size(); ++i)
        {
            QTableWidgetItem *item = new QTableWidgetItem(x.second[i]);
            ui->tableWidget->setItem(countRows, countCols, item);
            ++countCols;
        }
    }

    for(int i = 0; i < mp.size(); ++i)
    {
        QTableWidgetItem *cellItem = ui->tableWidget->item(i,0);

        if (cellItem) {
            cellItem->setFlags(cellItem->flags() & ~Qt::ItemIsEditable);
        }
    }
    MainWindow::userChange = true;
}

void MainWindow::Update(QString name, QString subject, QString mark)
{
    QNetworkAccessManager manager;

    QUrl url("http://" + address.toUtf8() + "/update?name=" + name.toUtf8() + "&subject=" + subject.toUtf8() + "&mark=" + mark.toUtf8());
    QNetworkRequest request(url);

    QNetworkReply *reply = manager.get(request);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

}


void MainWindow::on_pushButton_clicked()
{
    while(ui->tableWidget->rowCount()>0)
    {
        ui->tableWidget->removeRow(0);
    }

    Reload();
}


void MainWindow::on_tableWidget_cellChanged(int row, int column)
{
    if (MainWindow::userChange)
    {
        QString name = ui->tableWidget->item(row,0)->text();

        QTableWidgetItem *headerItem = ui->tableWidget->horizontalHeaderItem(column);
        QString subject = headerItem->text();

        QString mark = ui->tableWidget->item(row,column)->text();

        Update(name, subject, mark);
    }
}

void MainWindow::ReadConfig()
{
    QFile file("config.txt");

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        this->address = stream.readLine();
        qDebug() << address;
    }
    else
    {
        qDebug() << "Error with config!";
    }
}

