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
#ifndef ASCII_H
#define ASCII_H

#include <QDialog>

namespace Ui {
class Ascii;
}

class Ascii : public QDialog
{
    Q_OBJECT

public:
    explicit Ascii(QWidget *parent = 0);
    ~Ascii();

signals:
    void callback(bool send, QChar c);

private slots:
    void button_funnel();

private:
    Ui::Ascii *ui;
};

#endif // ASCII_H
