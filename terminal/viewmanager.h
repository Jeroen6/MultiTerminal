/* MultiTerminal (TCP or Serial terminal emulator with filtering capabilites)
 * Copyright (C) 2014  Jeroen Lodder
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * http://www.gnu.org/licenses/gpl-2.0.txt
 */
#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QWidget>
#include "view.h"

class ViewManager : public QWidget
{
    Q_OBJECT
public:
    explicit ViewManager(int setMaxViews = 32, QWidget *parent = 0);
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

    int getMaxViews() const;

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

