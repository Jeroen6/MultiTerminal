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
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationVersion(APP_VERSION);
    QCoreApplication::setApplicationName(APP_PRODUCT);
    QCoreApplication::setOrganizationName(APP_COMPANY);
    QCoreApplication::setOrganizationDomain("github.com/jeroen6");
    MainWindow w;
    w.show();

    return a.exec();
}
