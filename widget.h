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

	QMap<QString, QList<QPair<QDateTime, QDateTime>>> getOverlappingTimePeriods(QMap<QString, QList<QPair<QDateTime, QDateTime>>>& hashMap);
	void printMap(QMap<QString, QList<QPair<QDateTime, QDateTime>>>& hashMap);

public slots:
	void showPointToolTip(QMouseEvent* event);

protected:
	void resizeEvent(QResizeEvent* event) override;

private:
	Ui::Widget* ui;
	QHBoxLayout* mainHLayout;

	QCustomPlot* plot;

	QVector<QList<IP_TIME>> vlIP_Time;

	QList<QString> qlLabels;

	QMap<QString, QList<QPair<QDateTime, QDateTime>>> hashMap;


};
#endif // WIDGET_H
