#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMap>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
class QCustomPlot;
class QHBoxLayout;


QT_END_NAMESPACE


typedef struct
{
	QString qsTime;
	QString qsIP;
}IP_TIME, * PIP_TIME;

class Widget : public QWidget
{
	Q_OBJECT

public:
	Widget(QWidget* parent = nullptr);
	~Widget();

	bool readExcel();
	void test();

	void test2();

	void getSameIP(const QList<IP_TIME>& ql, const QList<IP_TIME>& ql2, QList<QString>& qlOut);
	void getAllIP(const QList<IP_TIME>& ql, const QList<IP_TIME>& ql2, QList<QString>& qlOut);

	//计算IP使用时间交集
	QMap<QString, QList<QPair<QDateTime, QDateTime>>> getOverlappingTimePeriods(QMap<QString, QList<QPair<QDateTime, QDateTime>>>& hashMap);

	//打印/记录Map信息
	void printMap(QMap<QString, QList<QPair<QDateTime, QDateTime>>>& hashMap);

public slots:
	void showPointToolTip(QMouseEvent* event);
	void SlotBtnOpen();

protected:
	void resizeEvent(QResizeEvent* event) override;

private:
	Ui::Widget* ui;
	QHBoxLayout* mainHLayout;

	QCustomPlot* plot;

	QVector<QList<IP_TIME>> vlIP_Time;	//保存每张表，每行数据的信息

	QList<QString> qlLabels;	//保存vlIP_Time中，重复使用的IP列表

	QMap<QString, QList<QPair<QDateTime, QDateTime>>> hashMap;	//保存重复IP的每个使用时间段


};
#endif // WIDGET_H
