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
#ifndef FILTERMANAGER_H
#define FILTERMANAGER_H

#include <vector>
#include <QString>
#include <QTextStream>

#define SUCCESS 0

// Filtertype
class Filter
{
public:
    Filter(){

    }

    ~Filter(){

    }

    QString mask;  /* mask is unique */
    QList<QString> buffer;
};

// Filtermanager
class FilterManager : public QObject
{
    Q_OBJECT
public:
    /// @note All filter views are id'd by the filter string
    int add(QString s);                 /// Add a filter
    int remove(QString s);              /// Remove a filter
    int write(QString data);            /// Write to filtermanager, data is filtered automatically
    int write(QString f, QString data); /// Write to a specific filter
    QString readLine(QString f);        /// Read a line from filtered output
    QString readLine(int i);        /// Read a line from filtered output
    int isAtIndex(QString s);       /// Find filter index from filter string

    int lineReady(int i);
private:
    QList<QString> unprocessed;     /// Unprocessed data
    QList<Filter*> filters;         /// List of filtered data lists

    void filterEngine();            /// The filter algorithm


};

#endif // FILTERMANAGER_H
