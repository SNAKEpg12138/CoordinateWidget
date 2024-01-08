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
#include <QHash>

#include "Logger.h"

Widget::Widget(QWidget* parent)
	: QWidget(parent)
	, ui(new Ui::Widget)
	, plot(nullptr)
{
	ui->setupUi(this);
	//Logger::startLogService();

	mainHLayout = new QHBoxLayout(ui->widget);

	connect(ui->btn_open, &QPushButton::clicked, this, &Widget::SlotBtnOpen);

	ui->checkBox_logDetail->setChecked(true);
	ui->label_3->hide();
}

Widget::~Widget()
{
	//Logger::stopLogService();
	delete ui;
}

void Widget::SlotBtnOpen()
{
	if (plot != nullptr)
	{
		delete plot;
		plot = nullptr;
	}

	if (readExcel())
	{
		//显示
		test();

		test2();
		qDebug() << "\n hash size: " << hashMap.size() << "\n";

		//printMap(hashMap);

		QMap<QString, QList<QPair<QDateTime, QDateTime>>> tmpMap = getOverlappingTimePeriods(hashMap);
		qDebug() << "\n tmpMap size: " << tmpMap.size() << "\n";

		printMap(tmpMap);
	}
}

bool Widget::readExcel()
{
	ui->label_3->show();
	// 打开文件对话框，选择文件
	QString fileName = QFileDialog::getOpenFileName(nullptr, "Open Excel", QDir::currentPath(), "Excel Files (*.xls *.xlsx)");


	if (fileName.isEmpty())
	{
		ui->label_3->hide();
		return false;
	}


	Logger::writeLog(u8"当前选择文件：" + fileName);

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

	Logger::writeLog(QString(u8"表1读取上限（行）：%1").arg(QString::number(ui->lineEdit_sheet1Max->text().toInt())));
	Logger::writeLog(QString(u8"表2读取上限（行）：%1").arg(QString::number(ui->lineEdit_sheet2Max->text().toInt())));

	// 遍历工作表集合
	for (int i = 1; i <= worksheetCount; i++)
	{
		qDebug() << u8"\n =============" << "《 sheet" << i << "》============== \n\n";

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
		for (int row = 1; row <= rowCount; row++)
		{
			//这里有多余数据，强制排除一下
			if (i == 1 && row >= ui->lineEdit_sheet1Max->text().toInt())
			{
				continue;
			}
			else if (i == 2 && row >= ui->lineEdit_sheet2Max->text().toInt())
			{
				continue;
			}

			// 遍历工作表的所有列
			//固定：第一、二列是时间，第三列是IP
			QString qsTime;
			for (int column = 1; column <= columnCount; column++)
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
						//qDebug() << "Row:" << row << "Column:" << column << "value :" << cellValue;
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
				//qDebug() << "Row:" << row << "Column:" << column << "value :" << cellValue;
			}

		}//end for(int row = 1; row <= rowCount; row++)

		vlIP_Time.append(qlTmp);

	}

	// 关闭工作簿并关闭Excel应用
	workbook->dynamicCall("Close()");
	excel->dynamicCall("Quit()");
	delete excel;

	ui->label_3->hide();

	return true;
}

void Widget::getSameIP(const QList<IP_TIME>& ql, const QList<IP_TIME>& ql2, QList<QString>& common)
{
	common.clear();
	QSet<QString> commonSet;  // 使用QSet进行去重
	QHash<QString, IP_TIME> hash;

	// 将其中一个列表转换为QHash
	for (int i = 0; i < ql.size(); ++i)
	{
		hash.insert(ql[i].qsIP, ql[i]);
	}

	// 在另一列表中查询
	for (int j = 0; j < ql2.size(); ++j)
	{
		if (hash.contains(ql2[j].qsIP))
		{
			// 将共同元素的IP添加到QSet中
			commonSet.insert(ql2[j].qsIP);  // 注意这里只添加ql2[j].qsIP
		}
	}

	// 转换QSet为QList
	common = commonSet.values();

}

void Widget::getAllIP(const QList<IP_TIME>& ql, const QList<IP_TIME>& ql2, QList<QString>& qlOut)
{
	qlOut.clear();
	QSet<QString> commonSet;  // 使用QSet进行去重
	for (int i = 0; i < ql.size(); i++)
	{
		commonSet.insert(ql.at(i).qsIP);
	}

	for (int j = 0; j < ql2.size(); j++)
	{
		commonSet.insert(ql2.at(j).qsIP);
	}
	// 转换QSet为QList
	qlOut = commonSet.values();
}

void Widget::test()
{
	qDebug() << "\n test2 \n";
	plot = new QCustomPlot();
	mainHLayout->addWidget(plot);

	plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
	//plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectAxes);
	//plot->setSelectionTolerance(5); // 根据需要调整选择公差，此处值为15像素
	//connect(plot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(showPointToolTip(QMouseEvent*)));

	QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
	//dateTicker->setDateTimeFormat("hh:mm:ss");
	dateTicker->setDateTimeFormat("yyyy/MM/dd hh:mm:ss");
	plot->xAxis->setTicker(dateTicker);
	plot->xAxis->setLabel("Time");
	plot->yAxis->setLabel("IP Address");

	qlLabels.clear();

	for (int i = 0; i < vlIP_Time.size(); i++)
	{
		if (i + 1 >= vlIP_Time.size())
		{
			break;
		}

		QList<IP_TIME> ql = vlIP_Time[i];
		QList<IP_TIME> ql2 = vlIP_Time[i + 1];

		//只显示重复IP，对构图有影响
		//getSameIP(ql, ql2, qlLabels);
		getAllIP(ql, ql2, qlLabels);
	}
	// 使用 'qSort' 函数对 QList 进行排序
	std::sort(qlLabels.begin(), qlLabels.end());
	qDebug() << "\n qlLabels size: " << qlLabels.size() << "\n";
	qDebug() << qlLabels << "\n";

	QVector<double> ticks;
	QVector<QString> labels;
	int nIndex = 0;

	foreach(const QString & ip, qlLabels)
	{
		ticks << nIndex;
		labels << ip;
		//qDebug() << "ip: " << ip;
		++nIndex;
	}

	QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
	textTicker->addTicks(ticks, labels);
	plot->yAxis->setTicker(textTicker);

	for (int i = 0; i < vlIP_Time.size(); i++)
	{
		plot->addGraph();
		//plot->graph(i)->setLineStyle(QCPGraph::lsNone);
		plot->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc));
		if (i == 0)
		{
			plot->graph(i)->setPen(QPen(Qt::blue, 3));
			plot->graph(i)->setName("zhang");
		}
		else if (i == 1)
		{
			plot->graph(i)->setPen(QPen(Qt::red, 1));
			plot->graph(i)->setName("ma");
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
			double time = dateTime.toMSecsSinceEpoch() / 1000.0;

			//qDebug() << "qlTime: " << qsTime << ",qsIP: " << qsIP << ", index: " << qlLabels.indexOf(qsIP);

			if (qsIP != qsOldIP)
			{
				//添加辅助点，以更好观察
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

void Widget::showPointToolTip(QMouseEvent* event)
{
	int x = event->pos().x(); // 鼠标在窗口中的x坐标
	int y = event->pos().y(); // 鼠标在窗口中的y坐标

	double graph_x = plot->xAxis->pixelToCoord(x); // 将像素坐标转换为图表坐标
	double graph_y = plot->yAxis->pixelToCoord(y);

	// 遍历所有的图
	for (int i = 0; i < plot->graphCount(); ++i)
	{
		QCPGraph* graph = plot->graph(i);

		qDebug() << "all name: " << graph->name();

		int dataIndex = graph->findBegin(graph_x, true); // 查找鼠标指向点的索引
		// 如果找到，且该点的位置与鼠标指向的位置相近（在一定的公差范围内，此处为15像素）
		if (dataIndex >= 0 && dataIndex < graph->dataCount() && qAbs(plot->yAxis->coordToPixel(graph->dataMainValue(dataIndex)) - y) < 5)
		{
			QDateTime dt;
			dt.setSecsSinceEpoch(graph->dataMainKey(dataIndex));
			QString qsName = graph->name();
			QString qsTime = dt.toString("yyyy/MM/dd hh:mm:ss");
			int nIndex = (int)graph->dataMainValue(dataIndex);
			QString qsIP = qlLabels.at(nIndex);
			qDebug() << "qsName: " << qsName << ", qsTime: " << qsTime << ", nIndex: " << nIndex << "qsIP: " << qsIP;

			QString tooltip = QString("%1\nTime:%2\nIP:%3").arg(qsName).arg(qsTime).arg(qsIP);
			QToolTip::showText(event->globalPos(), tooltip); // 显示信息
			return;
		}
	}
	// 如果没有找到数据点，则隐藏提示框
	QToolTip::hideText();
}

void Widget::resizeEvent(QResizeEvent* event)
{
	//plot->resize(this->size());
	QWidget::resizeEvent(event);
}

void Widget::test2()
{
	hashMap.clear();

	qDebug() << "test2--qlLabels size: " << qlLabels.size();

	for (int i = 0; i < vlIP_Time.size(); i++)
	{
		QList<IP_TIME> qlTime = vlIP_Time[i];

		QString qsOldIP = qlTime.first().qsIP;
		QString qsOldTime = qlTime.first().qsTime;

		for (int j = 0; j < qlTime.count(); j++)
		{
			IP_TIME ip_time = qlTime[j];

			QString qsIP = ip_time.qsIP;
			QString qsTime = ip_time.qsTime;

			if (qsIP != qsOldIP)
			{
				QDateTime dateTime_begin = QDateTime::fromString(qsOldTime, "yyyy/MM/dd HH:mm:ss");
				QDateTime dateTime_end = QDateTime::fromString(qsTime, "yyyy/MM/dd HH:mm:ss");

				//qDebug() << "IP: " << qsOldIP << ", begin: " << qsOldTime << ", end: " << qsTime;

				QPair<QDateTime, QDateTime> pair(dateTime_begin, dateTime_end);

				if (!qlLabels.contains(qsOldIP))
				{
					qsOldIP = qsIP;
					qsOldTime = qsTime;
					continue;
				}

				//IP变化
				if (hashMap.contains(qsOldIP))
				{
					hashMap[qsOldIP].append(pair);
				}
				else
				{
					QList<QPair<QDateTime, QDateTime>> qlDate;
					qlDate.append(pair);
					hashMap.insert(qsOldIP, qlDate);
				}

				qsOldIP = qsIP;
				qsOldTime = qsTime;

			}
			else
			{

			}
		}
	}
}

QMap<QString, QList<QPair<QDateTime, QDateTime>>> Widget::getOverlappingTimePeriods(QMap<QString, QList<QPair<QDateTime, QDateTime>>>& hashMap)
{
	QMap<QString, QList<QPair<QDateTime, QDateTime>>> resultMap;

	QList<QString> keys = hashMap.keys();

	Logger::writeLog(QString(u8"两个表中都有用到的IP数量为：%1").arg(QString::number(keys.count())));

	Logger::writeLog(QString(u8"具体如下："));
	for (int i = 0; i < keys.count(); i++)
	{
		QString qsLog = QString(u8"%1: %2").arg(QString::number(i + 1)).arg(keys.at(i));
		Logger::writeLog(qsLog);
	}
	qDebug() << "qlLabels: " << qlLabels.size();

	Logger::writeLog(QString(u8"\n\n"));

	bool bRet = false;

	for (QString key : keys)
	{
		QList < QPair<QDateTime, QDateTime> > qlTime = hashMap[key];
		for (int i = 0; i < qlTime.size(); i++)
		{
			QPair<QDateTime, QDateTime> pair = qlTime[i];

			for (int j = i + 1; j < qlTime.size(); j++)
			{
				QPair<QDateTime, QDateTime> pair2 = qlTime[j];

				bool bIn = false;
				//有时间交集, 计算交集区间
				QPair<QDateTime, QDateTime> pairSame;
				if (pair.first > pair2.first && pair.second < pair2.second)
				{
					/*如下：
							<----->
						<------------------->
					*/
					qDebug() << u8"交集情况①";
					Logger::writeLog(u8"交集情况①");
					pairSame = pair;
					bIn = true;
				}
				else if (pair2.first > pair.first && pair2.second < pair.second)
				{
					/*如下：
						<---------------->
							<------>
					*/
					qDebug() << u8"交集情况②";
					Logger::writeLog(u8"交集情况②");
					pairSame = pair2;
					bIn = true;
				}
				else if (pair2.first > pair.first && pair2.first < pair.second)
				{
					/* 如下：
						<---->
						  <----->
					*/
					qDebug() << u8"交集情况③";
					Logger::writeLog(u8"交集情况③");
					pairSame.first = pair2.first;
					pairSame.second = pair.second;
					bIn = true;
				}
				else if (pair.first > pair2.first && pair.first < pair2.second)
				{
					/* 如下：
							<------->
						<------->
					*/
					qDebug() << u8"交集情况④";
					Logger::writeLog(u8"交集情况④");
					pairSame.first = pair.first;
					pairSame.second = pair2.second;
					bIn = true;
				}

				bRet |= bIn;
				if (bIn)
				{
					qDebug() << u8"当前比较IP: " << key;
					qDebug() << u8"表1 begin: " << pair.first << ", pair end: " << pair.second;
					qDebug() << u8"表2 begin: " << pair2.first << ", pair end: " << pair2.second;
					qDebug() << u8"===时间交集 begin: " << pairSame.first << ", end: " << pairSame.second << " ===\n";

					//确保交集必记录
					Logger::writeLog(QString(u8"当前比较IP: %1").arg(key));
					Logger::writeLog(QString(u8"表1 IP使用时间区间[%1, %2]").arg(pair.first.toString("yyyy/MM/dd hh:mm:ss")).arg(pair.second.toString("yyyy/MM/dd hh:mm:ss")));
					Logger::writeLog(QString(u8"表2 IP使用时间区间[%1, %2]").arg(pair2.first.toString("yyyy/MM/dd hh:mm:ss")).arg(pair2.second.toString("yyyy/MM/dd hh:mm:ss")));
					Logger::writeLog(QString(u8"存在时间交集 [%1, %2]").arg(pairSame.first.toString("yyyy/MM/dd hh:mm:ss")).arg(pairSame.second.toString("yyyy/MM/dd hh:mm:ss")));

					if (!ui->checkBox_logDetail->isChecked())
					{
						Logger::writeLog(u8"\n\n");
					}

					if (resultMap.contains(key))
					{
						resultMap[key].append(pairSame);
					}
					else
					{
						QList<QPair<QDateTime, QDateTime>> qlTime;
						qlTime.append(pairSame);
						resultMap.insert(key, qlTime);
					}
				}
				else
				{
					//qDebug() << u8"=====无时间交集=====";
					if (ui->checkBox_logDetail->isChecked())
					{
						Logger::writeLog(QString(u8"当前比较IP: %1").arg(key));
						Logger::writeLog(QString(u8"表1 IP使用时间区间[%1, %2]").arg(pair.first.toString("yyyy/MM/dd hh:mm:ss")).arg(pair.second.toString("yyyy/MM/dd hh:mm:ss")));
						Logger::writeLog(QString(u8"表2 IP使用时间区间[%1, %2]").arg(pair2.first.toString("yyyy/MM/dd hh:mm:ss")).arg(pair2.second.toString("yyyy/MM/dd hh:mm:ss")));
						Logger::writeLog(u8"=====无时间交集=====");
					}

				}
				if (ui->checkBox_logDetail->isChecked())
				{
					Logger::writeLog(u8"\n\n");
				}

			}//end for (int j = i + 1; j < qlTime.size(); j++)
		}

	}//end for (QString key : keys)

	if (!bRet)
	{
		Logger::writeLog(u8"\n=====所有IP均无时间交集=====\n");
	}

	return resultMap;
}

void Widget::printMap(QMap<QString, QList<QPair<QDateTime, QDateTime>>>& hashMap)
{
	QList<QString> keys = hashMap.keys();
	for (QString key : keys)
	{
		QList < QPair<QDateTime, QDateTime> > qlTime = hashMap[key];
		for (auto pair : qlTime)
		{
			qDebug() << "IP: " << key << ", begin: " << pair.first.toString("yyyy/MM/dd hh:mm:ss") << ", end: " << pair.second.toString("yyyy/MM/dd hh:mm:ss");
		}

		qDebug() << "\n\n ================================================== \n\n";
	}

	qDebug() << "\n\n ================================================== \n\n";
}