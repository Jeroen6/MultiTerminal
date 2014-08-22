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
#include "ascii.h"
#include "ui_ascii.h"

Ascii::Ascii(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Ascii)
{
    ui->setupUi(this);
}

Ascii::~Ascii()
{
    delete ui;
}

/// @brief funnel all pushbutton events to this slot
void Ascii::button_funnel()
{
    /** @todo detemine pressed button with sender and tabindex. */
}
