#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QWidget>
#include "view.h"

class ViewManager : public QWidget
{
    Q_OBJECT
public:
    explicit ViewManager(int setMaxViews = 16, QWidget *parent = 0);
    ~ViewManager();

    int spawn(QString filter);

    int kill(int viewID);
    int kill(QString filter);

    int show(int ViewID);
    int show(QString filter);

    int hide(int ViewID);
    int hide(QString filter);

    int getID(QString filter);

    int write(QString filter, QString data);

    int wipe(QString filter);
    void wipeAll();

signals:
    void viewVisibilityChanged(QString filter);

private slots:
    void someViewWasClosed(View *source);

private:
    QList<int> viewIDs;
    QList<View*> viewList;
    QList<int> viewIdTranslator; // views index [ viewID ]
    int maxViews;

    int newViewID(void);
    void recycleViewID(int oldViewID);

    //const QString titleTail = " - SerialTerminal";
};

#endif // VIEWMANAGER_H

