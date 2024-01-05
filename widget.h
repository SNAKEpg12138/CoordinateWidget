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
}IP_TIME, *PIP_TIME;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    bool readExcel();
    void test();

    void getSameIP(const QList<IP_TIME>& ql, const QList<IP_TIME>& ql2, QList<QString>& qlOut);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    Ui::Widget *ui;
    QHBoxLayout* mainHLayout;

    QCustomPlot* plot;

    QVector<QList<IP_TIME>> vlIP_Time;


};
#endif // WIDGET_H
