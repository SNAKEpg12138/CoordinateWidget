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

//    if ( readExcel() )
//    {
//        qDebug() << "vec size: " << vec.size() << "\n";
//        QCustomPlot *customPlot = new QCustomPlot();
//        setupGraph2(customPlot, QPen(QColor(237, 28, 36), 5));
//        customPlot->resize(this->size());
//        customPlot->replot();
//        mainHLayout->addWidget(customPlot);

//    }
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
    vec.clear();

    MyMap tmpMap;
    tmpMap.insert("", QStringList());
    //vec.push_back(tmpMap);

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

        MyMap map;

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
                    //qDebug() << "final time: " << qsTime << "\n\n";

                    if (cellValue == "223.104.68.119")
                    {
                        qDebug() << "\n qsTime: " << qsTime << "\n";
                    }

                    if ( map.contains(cellValue) )
                    {
                        map[cellValue].append(qsTime);
                    }
                    else
                    {
                        QStringList qlTime;
                        qlTime << qsTime;
                        map.insert(cellValue, qlTime);
                    }
                }
                else
                {

                    qsTime += cellValue;
                }

                // 输出单元格内容
                qDebug() << "Row:" << row << "Column:" << column << "value :" << cellValue;
            }
        }

        vec.push_back(map);
    }

    //vec.push_back(tmpMap);

    // 关闭工作簿并关闭Excel应用
    workbook->dynamicCall("Close()");
    excel->dynamicCall("Quit()");
    delete excel;

    return true;
}


void Widget::setupGraph(QCustomPlot *customPlot)
{
    // Set the x-axis with increasing time
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    //dateTicker->setDateTimeFormat("hh:mm:ss");
    dateTicker->setDateTimeFormat("yyyy/MM/dd hh:mm:ss");
    customPlot->xAxis->setTicker(dateTicker);
    customPlot->xAxis->setLabel("Time");

    // Set the y-axis with IP addresses
    customPlot->yAxis->setLabel("IP Address");
    QVector<double> ticks;
    QVector<QString> labels;

    //ticks << 0 << 1 << 2 << 3 << 4; // IP addresses are represented by integers here for simplicity
    //labels << " " << "103.116.122.50" << "223.104.68.66" << "103.116.122.118" << " ";

    int nSize = vec.size();
    qDebug() << "\n\n nSize: " << nSize << "\n\n";
    int nTicks = 0;
    for (int i = 0; i < nSize; i++)
    {
        MyMap tmpMap = vec[i];
        QList<QString> keys = tmpMap.keys();
        int nKeySize = keys.size();
        qDebug() << "\n nKey Size: " << nKeySize << "\n";
        for (int j = 0; j < nKeySize; ++j)
        {
            nTicks++;
            qDebug() << "\n nTicks: " << nTicks << "\n";
            ticks << j;
            labels << keys.at(j);
        }

//        ticks << i;
//        QList<QString> keys = vec[i].keys();
//        int nKeySize = keys.size();
//        qDebug() << "\n nKey Size: " << nKeySize << "\n";
//        labels << keys.at(i);
    }

    qDebug() << "\n QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText) \n";
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    customPlot->yAxis->setTicker(textTicker);

    // Make graphs, 需对应IP数量
//    for (int i = 0; i < 5; ++i)
//    {
//        customPlot->addGraph();
//        customPlot->graph(i)->setPen(QPen(QColor(40 * i, 100, 150)));
//        customPlot->graph(i)->setName(labels[i]);
//    }

    qDebug() << "\n Make graphs \n";
    for (int i = 0; i < nSize; i++)
    {
        customPlot->addGraph();
        customPlot->graph(i)->setPen(QPen(QColor(40 * i, 100, 150)));
        customPlot->graph(i)->setName(labels[i]);
    }


//    // Set graph data - in this example we add 100 points of data for each IP
//    for (int i = 0; i < 100; ++i)   //每个IP的时间点
//    {
//        double time = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0 + 24 * 3600 * i; // For example take the current time and add i days
//        for (int j = 0; j < 5; ++j)
//        {
//            customPlot->graph(j)->addData(time, j); // Add data point to the j-th graph with time on x-axis and corresponding IP address on y
//        }
//    }

    qDebug() << "\n Set graph data \n";
    int nIndex = 0;
    for (int i = 0; i < nSize; i++)
    {
        QList<QString> keys = vec[i].keys();
        for (int m = 0; m < keys.count(); ++m)
        {
            nIndex++;
            QStringList qlTime = vec[i].value(keys[m]);
            for (int n = 0; n < qlTime.size(); ++n)
            {
                QDateTime dateTime = QDateTime::fromString(qlTime[n], "yyyy/MM/dd HH:mm:ss");
                double time = dateTime.toMSecsSinceEpoch()/1000.0 + 24 * 3600 * i;
                //qDebug() << "\n nIndex: " << nIndex << "\n";
                customPlot->graph(nIndex)->addData(time, nIndex);
            }
        }
    }

    customPlot->rescaleAxes();
}

void Widget::setupGraph2(QCustomPlot *customPlot, const QPen& pen)
{
    // Set the x-axis with increasing time
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    //dateTicker->setDateTimeFormat("hh:mm:ss");
    dateTicker->setDateTimeFormat("yyyy/MM/dd hh:mm:ss");
    customPlot->xAxis->setTicker(dateTicker);
    customPlot->xAxis->setLabel("Time");

    // Set the y-axis with IP addresses
    customPlot->yAxis->setLabel("IP Address");
    QVector<double> ticks;
    QVector<QString> labels;

    ticks << 0 << 1 << 2 << 3 << 4; // IP addresses are represented by integers here for simplicity
    labels << " " << "103.116.122.50" << "223.104.68.66" << "103.116.122.118" << " ";

    qDebug() << "\n QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText) \n";
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    customPlot->yAxis->setTicker(textTicker);

    // Make graphs, 需对应IP数量
     qDebug() << "\n Make graphs \n";
    for (int i = 0; i < 5; ++i)
    {
        customPlot->addGraph();
        //customPlot->graph(i)->setPen(QPen(QColor(40 * i, 100, 150)));
        customPlot->graph(i)->setPen(pen);
        customPlot->graph(i)->setName(labels[i]);
    }

    qDebug() << "\n Set graph data \n";
    // Set graph data - in this example we add 100 points of data for each IP
    for (int i = 0; i < 100; ++i)   //每个IP的时间点
    {
        double time = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0 + 24 * 3600 * i; // For example take the current time and add i days
        for (int j = 0; j < 5; ++j)
        {
            if (i % 2 == 0 && j % 2 != 0)
            {
                continue;
            }
            customPlot->graph(j)->addData(time, j); // Add data point to the j-th graph with time on x-axis and corresponding IP address on y
        }
    }

    customPlot->rescaleAxes();
}

//int main(int argc, char *argv[])
//{
//    QApplication a(argc, argv);
//    QCustomPlot *customPlot = new QCustomPlot;
//    setupGraph(customPlot);
//    customPlot->replot();
//    customPlot->show();
//    return a.exec();
//}

void Widget::resizeEvent(QResizeEvent* event)
{
    //plot->resize(this->size());
    QWidget::resizeEvent(event);
}

void Widget::test()
{
    qDebug() << "\n test \n";
    plot = new QCustomPlot();
    mainHLayout->addWidget(plot);

    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    //plot->xAxis->setLabel("Time");


    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    //dateTicker->setDateTimeFormat("hh:mm:ss");
    dateTicker->setDateTimeFormat("yyyy/MM/dd hh:mm:ss");
    plot->xAxis->setTicker(dateTicker);
    plot->xAxis->setLabel("Time");

    plot->yAxis->setLabel("IP Address");

    QSet<QString> combinedIpSet;
    for (int i = 0; i < vec.size(); i++)
    {
        if (i + 1 >= vec.size())
        {
            break;
        }
        MyMap tmpMap = vec[i];
        MyMap tmpMap2 = vec[i + 1];

        QList<QString> keys = tmpMap.keys();
        QList<QString> keys2 = tmpMap2.keys();
        for (int j = 0; j < keys.count(); j++)
        {
            QString qsIP = keys[j];

            if (qsIP == "223.104.68.119")
            {
                qDebug() << "1: " << tmpMap.value(qsIP);
                qDebug() << "2: " << tmpMap2.value(qsIP);
            }

            if (keys2.contains(qsIP))
            {
                combinedIpSet.insert(qsIP);
            }

        }
    }

    QList<QString> qlIp = QList<QString>::fromSet(combinedIpSet);

    qDebug() << "\n qlIp size: " << qlIp.size() << "\n";
    qDebug() << qlIp << "\n";
    QVector<double> ticks;
    QVector<QString> labels;
    int i = 0;
    foreach (const QString &ip, qlIp)
    {
        ticks << i;
        labels << ip;
        //qDebug() << "ip: " << ip;
        ++i;
    }

    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    plot->yAxis->setTicker(textTicker);

    for (int i = 0; i < 2; i++)
    {
        plot->addGraph();
        //plot->graph(i)->setLineStyle(QCPGraph::lsNone);
        //plot->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc));
        if (i == 0)
        {
            plot->graph(0)->setPen(QPen(Qt::blue));
            plot->graph(0)->setName("Sheet 3");
        }
        else if (i == 1)
        {
            plot->graph(1)->setPen(QPen(Qt::red));
            plot->graph(1)->setName("Sheet 5");
        }
    }

    //添加数据
    qDebug() << "\n add data \n";
    for (int i = 0; i < vec.size(); i++)
    {
//        if ((i + 1) >=  vec.size())
//        {
//            continue;
//        }

        MyMap tmpMap = vec[i];
        //MyMap tmpMap2 = vec[i + 1];


        QList<QString> keys = tmpMap.keys();
        //QList<QString> keys2 = tmpMap2.keys();
        int nSameIpCnt = 0;
        for (int k = 0; k < keys.count(); k++)
        {
            QString qsIP = keys[k];

            if (qlIp.indexOf(qsIP) == -1)
            {
                continue;
            }

            //只添加相同IP的数据
//            if (!keys2.contains(qsIP))
//            {
//                continue;
//            }
//            qDebug() << "same ip: " << qsIP;
//            nSameIpCnt++;

            QStringList qlTime = tmpMap.value(qsIP);
            //qDebug() << "begin: " << qlTime.first() << ", end: " << qlTime.last() << "\n";
            for (int m = 0; m < qlTime.count(); m++)
            {
                qDebug() << "qlTime: " << qlTime[m] << ",qsIP: " << qsIP << ", index: " << qlIp.indexOf(qsIP);
                QDateTime dateTime = QDateTime::fromString(qlTime[m], "yyyy/MM/dd HH:mm:ss");
                double time = dateTime.toMSecsSinceEpoch()/1000.0 + 24 * 3600 * i;
                //plot->graph(0)->addData(time, qlIp.indexOf(qsIP));
                plot->graph(i)->addData(time, qlIp.indexOf(qsIP));
            }

//            QStringList qlTime2 = tmpMap2.value(qsIP);
//            //qDebug() << "begin: " << qlTime2.first() << ", end: " << qlTime2.last() << "\n";
//            for (int m = 0; m < qlTime2.count(); m++)
//            {
//                //qDebug() << "qlTime2: " << qlTime2[m];
//                //qDebug() << "graph count: " << plot->graphCount();
//                QDateTime dateTime = QDateTime::fromString(qlTime2[m], "yyyy/MM/dd HH:mm:ss");
//                double time = dateTime.toMSecsSinceEpoch()/1000.0 + 24 * 3600 * i;
//                plot->graph(1)->addData(time, qlIp.indexOf(qsIP));
//            }
        }

        qDebug() << "same ip count: " << nSameIpCnt << "\n";

    }

//    QCPDataContainer<QCPGraphData>::const_iterator it0 = plot->graph(0)->data()->begin();
//    QCPDataContainer<QCPGraphData>::const_iterator it1 = plot->graph(1)->data()->begin();

//    QCPGraph *commonGraph = plot->addGraph();

//    while (it0 != plot->graph(0)->data()->end() && it1 != plot->graph(1)->data()->end())
//    {
//        if (it0->key < it1->key)
//            ++it0;
//        else if (it0->key > it1->key)
//            ++it1;
//        else
//        {
//            if (it0->value == it1->value)
//            {
//                qDebug() << "it0->value: " << it0->value;
//                commonGraph->addData(it0->key, it0->value);
//            }
//            ++it0;
//            ++it1;
//        }
//    }

    //QDateTime dateTime = QDateTime::fromString("2023/6/11 04:17:36", "yyyy/MM/dd HH:mm:ss");
    //double time = dateTime.toMSecsSinceEpoch()/1000.0 + 24 * 3600 * i;
    //commonGraph->addData(time, 1);

    //plot->replot();

    //=====================================================================
    // 创建一个新的graph来显示相同的数据点及连线
    QCPGraph *commonGraph = plot->addGraph();
    QCPDataContainer<QCPGraphData>::const_iterator it0 = plot->graph(0)->data()->begin();
    QCPDataContainer<QCPGraphData>::const_iterator it1 = plot->graph(1)->data()->begin();

    while (it0 != plot->graph(0)->data()->end() && it1 != plot->graph(1)->data()->end())
    {
        if (it0->key < it1->key)
            ++it0;
        else if (it0->key > it1->key)
            ++it1;
        else
        {
            if (it0->value == it1->value)
            {
                qDebug() << "key: " << it0->key << ", it0->value: " << it0->value << "\n";
                commonGraph->addData(it0->key, it0->value);
            }
            ++it0;
            ++it1;
        }
    }

    // 隐藏原本两个graph的点和连线
//    plot->graph(0)->setLineStyle(QCPGraph::lsNone);
//    plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone));

//    plot->graph(1)->setLineStyle(QCPGraph::lsNone);
//    plot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone));

    // 设置新graph的线的样式
    commonGraph->setLineStyle(QCPGraph::lsLine);

    //commonGraph->setLineStyle(QCPGraph::lsNone);
    //commonGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc));
    commonGraph->setPen(QPen(Qt::black, 5));
    commonGraph->setName("Sheet 10");

    plot->rescaleAxes();
    plot->replot();

    plot->show();
}


