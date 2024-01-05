#include "widget.h"
#include "ui_widget.h"

#include <QAxObject>
#include <QFileDialog>
#include <QDebug>
#include <QHBoxLayout>

#include "qcustomplot.h"

#include <QDateTime>
#include <QString>
#include <QMap>

#include <QSharedPointer>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    mainHLayout = new QHBoxLayout(this);

    if ( readExcel() )
    {
        test();
    }

}

Widget::~Widget()
{
    delete ui;
}


bool Widget::readExcel()
{
    // 打开文件对话框，选择文件
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open Excel", QDir::currentPath(), "Excel Files (*.xls *.xlsx)");

    if (fileName.isEmpty())
    {
        return false;
    }
    vlIP_Time.clear();


    // 创建连接到Excel的对象
    QAxObject* excel = new QAxObject("Excel.Application");

    // 打开工作簿
    QAxObject* workbooks = excel->querySubObject("WorkBooks");

    //打开文件
    QAxObject* workbook = workbooks->querySubObject("Open(QString, QVariant)", fileName, 0);

    // 获取表格对象集合
    QAxObject* worksheets = workbook->querySubObject("Worksheets");

    // 计算工作表数量
    int worksheetCount = worksheets->dynamicCall("Count()").toInt();

    // 遍历工作表集合
    for(int i = 1; i <= worksheetCount; i++)
    {
        qDebug() << "\n =============" << "《 sheet" << i << "》============== \n\n";

        QList<IP_TIME> qlTmp;

        // 获取工作表
        QAxObject* worksheet = worksheets->querySubObject("Item(int)", i);

        // 获取行数
        QAxObject* usedRange = worksheet->querySubObject("UsedRange");
        int rowCount = usedRange->querySubObject("Rows")->property("Count").toInt();

        // 获取列数
        int columnCount = usedRange->querySubObject("Columns")->property("Count").toInt();

        qDebug() << "rowCount: " << rowCount << ", columnCount: " << columnCount << "\n";


        // 遍历工作表的所有行
        for(int row = 1; row <= rowCount; row++)
        {
            // 遍历工作表的所有列
            //固定：第一、二列是时间，第三列是IP
            QString qsTime;
            for(int column = 1; column <= columnCount; column++)
            {
                // 读取单元格内容
                QAxObject* cell = worksheet->querySubObject("Cells(int,int)", row, column);

                QString cellValue;
                if (column == 1)
                {
                    QDateTime cellDateTime = cell->dynamicCall("Value()").toDateTime();
                    cellValue = cellDateTime.toString("yyyy/MM/dd");
                }
                else
                {
                    cellValue = cell->dynamicCall("Value()").toString();
                }

                if (cellValue.isEmpty() || cellValue == "IP" || cellValue == "时间")
                {
                    qDebug() << "cellValue: " << cellValue;
                    continue;
                }


                if (column == 3)
                {
                    if (cellValue.isEmpty())
                    {
                        qDebug() << "Row:" << row << "Column:" << column << "value :" << cellValue;
                        continue;
                    }
                    qsTime = qsTime.left(qsTime.size() - 10);

                    IP_TIME ipTime;
                    ipTime.qsTime = qsTime;
                    ipTime.qsIP = cellValue;
                    qlTmp.append(ipTime);
                }
                else
                {

                    qsTime += cellValue;
                }

                // 输出单元格内容
                qDebug() << "Row:" << row << "Column:" << column << "value :" << cellValue;
            }

        }//end for(int row = 1; row <= rowCount; row++)

        vlIP_Time.append(qlTmp);

    }

    // 关闭工作簿并关闭Excel应用
    workbook->dynamicCall("Close()");
    excel->dynamicCall("Quit()");
    delete excel;

    return true;
}

void Widget::getSameIP(const QList<IP_TIME>& ql, const QList<IP_TIME>& ql2, QList<QString>& common)
{
    QSet<QString> commonSet;  // 使用QSet进行去重
    QHash<QString, IP_TIME> hash;

    // 将其中一个列表转换为QHash
    for (int i = 0; i < ql.size(); ++i) {
        hash.insert(ql[i].qsIP, ql[i]);
    }

    // 在另一列表中查询
    for (int j = 0; j < ql2.size(); ++j) {
        if (hash.contains(ql2[j].qsIP)){
            // 将共同元素的IP添加到QSet中
            commonSet.insert(ql2[j].qsIP);  // 注意这里只添加ql2[j].qsIP
        }
    }

    // 转换QSet为QList
    common = commonSet.values();

}

void Widget::test()
{
    qDebug() << "\n test2 \n";
    plot = new QCustomPlot();
    mainHLayout->addWidget(plot);

    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    //dateTicker->setDateTimeFormat("hh:mm:ss");
    dateTicker->setDateTimeFormat("yyyy/MM/dd hh:mm:ss");
    plot->xAxis->setTicker(dateTicker);
    plot->xAxis->setLabel("Time");
    plot->yAxis->setLabel("IP Address");

    QList<QString> qlLabels;
    //只显示重复IP
    for (int i = 0; i < vlIP_Time.size(); i++)
    {
        if (i + 1 >= vlIP_Time.size())
        {
            break;
        }

        QList<IP_TIME> ql = vlIP_Time[i];
        QList<IP_TIME> ql2 = vlIP_Time[i + 1];


        getSameIP(ql, ql2, qlLabels);

    }
    // 使用 'qSort' 函数对 QList 进行排序
    std::sort(qlLabels.begin(), qlLabels.end());
    qDebug() << "\n qlLabels size: " << qlLabels.size() << "\n";
    qDebug() << qlLabels << "\n";

    QVector<double> ticks;
    QVector<QString> labels;
    int i = 0;
    foreach (const QString &ip, qlLabels)
    {
        ticks << i;
        labels << ip;
        //qDebug() << "ip: " << ip;
        ++i;
    }

    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    plot->yAxis->setTicker(textTicker);

    for (int i = 0; i < vlIP_Time.size(); i++)
    {
        plot->addGraph();
        plot->graph(i)->setLineStyle(QCPGraph::lsNone);
        plot->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc));
        if (i == 0)
        {
            plot->graph(0)->setPen(QPen(Qt::blue, 3));
            plot->graph(0)->setName("Sheet 1");
        }
        else if (i == 1)
        {
            plot->graph(1)->setPen(QPen(Qt::red, 1));
            plot->graph(1)->setName("Sheet 2");
        }
    }

    //添加数据
    qDebug() << "\n add data \n";



    for (int i = 0; i < vlIP_Time.size(); i++)
    {

        QList<IP_TIME> qlIP_TIME = vlIP_Time[i];

        QString qsOldIP = qlIP_TIME.first().qsIP;
        QString qsOldTime;

        for (int j = 0; j < qlIP_TIME.size(); j++)
        {
            QString qsIP = qlIP_TIME[j].qsIP;
            QString qsTime = qlIP_TIME[j].qsTime;

            //只显示重复IP
            if (qlLabels.indexOf(qsIP) == -1)
            {
                continue;
            }

            QDateTime dateTime = QDateTime::fromString(qsTime, "yyyy/MM/dd HH:mm:ss");
            double time = dateTime.toMSecsSinceEpoch()/1000.0;

            qDebug() << "qlTime: " << qsTime << ",qsIP: " << qsIP << ", index: " << qlLabels.indexOf(qsIP);

            if (qsIP != qsOldIP)
            {

                plot->graph(i)->addData(time, qlLabels.indexOf(qsOldIP));
            }
            qsOldIP = qsIP;
            qsOldTime = qsTime;

            plot->graph(i)->addData(time, qlLabels.indexOf(qsIP));

        }

    }

    plot->rescaleAxes();
    plot->replot();

    plot->show();
}

void Widget::resizeEvent(QResizeEvent* event)
{
    //plot->resize(this->size());
    QWidget::resizeEvent(event);
}
