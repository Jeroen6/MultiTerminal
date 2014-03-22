#ifndef VIEW_H
#define VIEW_H

#include <QMainWindow>

namespace Ui {
class View;
}

class View : public QMainWindow
{
    Q_OBJECT

public:
    explicit View(int argViewID = 0, QString argFilter = "", QWidget *parent = 0);
    ~View();
    int getViewID(){ return ViewID; };
    QString getFilter() { return filter; }
    void write(QString data);

public slots:
    void on_buttonWipe_clicked();

signals:
    void closed(View *source);

private slots:
    void on_actionClose_triggered();

    void on_checkAutoScroll_stateChanged(int arg1);

private:
    Ui::View *ui;
    int ViewID;
    QString filter;

    void closeEvent( QCloseEvent * event );
};

#endif // VIEW_H
