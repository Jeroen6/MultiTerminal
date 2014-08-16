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
#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    // Insert poor mans version number
    QString content = ui->textBrowser->toHtml();
    content.replace("__DATE__",__DATE__);
    content.replace("__TIME__",__TIME__);
    ui->textBrowser->clear();
    ui->textBrowser->insertHtml(content);
    // Hide this window
    this->hide();
}

About::~About()
{
    delete ui;
}

void About::on_pushButton_clicked()
{
    this->hide();
}
