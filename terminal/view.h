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
