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
#include "view.h"
#include "ui_view.h"

View::View(int argViewID, QString argFilter, QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::View)
{
    ViewID = argViewID;
    filter = argFilter;

    ui->setupUi(this);
}

View::~View()
{
    delete ui;
}

void View::on_actionClose_triggered()
{
    this->close();
}

void View::write(QString data){
    if(data != ""){
        data.replace(QChar('\n'),QString("<font color=\"red\">\\n<\font>"));
        data.replace(QChar('\r'),QString("<font color=\"red\">\\r<\font>"));
        QCursor c = ui->textInput->cursor();
        ui->textInput->append(data);
        if(ui->checkAutoScroll->checkState()==Qt::Checked)
            ui->textInput->moveCursor(QTextCursor::End);
        else
            ui->textInput->setCursor(c);
    }

    /*
    ui->textInput->moveCursor(QTextCursor::End);
    ui->textInput->insertPlainText(data);
    ui->textInput->moveCursor(QTextCursor::End);
    */
}

void View::on_checkAutoScroll_stateChanged(int arg1)
{
    if(ui->checkAutoScroll->checkState()==Qt::Unchecked)
        ui->textInput->moveCursor(QTextCursor::WordLeft);

}

void View::on_buttonWipe_clicked()
{
    ui->textInput->clear();
}

void View::closeEvent( QCloseEvent * event ){
    closed(this);
}
