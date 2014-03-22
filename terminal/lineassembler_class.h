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
#ifndef LINEASSEMBLER_CLASS_H
#define LINEASSEMBLER_CLASS_H

#include <Qt>
#include <QByteArray>
#include <QStringList>
#include <QElapsedTimer>

class LineAssembler
{
public:
    LineAssembler();
    ~LineAssembler();

    void push_chunk(QByteArray);
    void push_chunk(QString);
    void push_chunk(char* );

    // Status
    int lines_ready();
    int remains_ready();
    int remainder_age();

    //
    QString pull_line();
    QString pull_remains();

private:
    QByteArray *raw;
    QByteArray *temp;
    QStringList *lines;
    QElapsedTimer *age;

    void readLines();

    int lines_in_stock;
    int remains_in_stock;

};

#endif // LINEASSEMBLER_CLASS_H
