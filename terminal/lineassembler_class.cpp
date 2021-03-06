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
#include "lineassembler_class.h"
#include <Qt>
#include <QByteArray>
#include <QStringList>
#include <QElapsedTimer>

LineAssembler::LineAssembler()
{
    raw = new QByteArray();
    lines = new QStringList();
    age = new QElapsedTimer;
    age->start();
    lines_in_stock = 0;
    remains_in_stock = 0;
}

LineAssembler::~LineAssembler()
{
    delete raw ;
    delete lines ;
    delete age ;
}

void LineAssembler::push_chunk(char* input){
    raw->push_back(input);
    readLines();
}

void LineAssembler::push_chunk(QByteArray input){
    raw->push_back(input);
    readLines();
}

void LineAssembler::push_chunk(QString input){
    QByteArray array(input.toStdString().c_str());
    raw->push_back(array);
    readLines();
}

int LineAssembler::lines_ready(){
    return lines_in_stock;
}

int LineAssembler::remains_ready(){
    return remains_in_stock;
}

QString LineAssembler::pull_line(){
    if(lines_in_stock > 0){
        QString line = lines->first();
        lines->removeFirst();
        lines_in_stock--;
        return line;
    }else{
        return NULL;
    }
}

int LineAssembler::remainder_age(){
    if(remains_in_stock > 0){
        return age->elapsed();
    }else{
        return -1;
    }
}

QString LineAssembler::pull_remains(){
    if(remains_in_stock > 0){
        QString line(*raw);
        raw->clear();
        remains_in_stock = 0;
        return line;
    }else{
        return NULL;
    }
}

/* Privates */
void LineAssembler::readLines(){
    int pos = raw->indexOf('\n');
    while(pos > -1){
        QString line(raw->mid(0,pos+1));
        raw->remove(0,pos+1);
        lines->push_back(line);
        lines_in_stock++;
        pos = raw->indexOf('\n');
    }
    if(raw->count()){
        remains_in_stock = raw->count();
        // Start age timer
        age->restart();
    }else{
        remains_in_stock = 0;
    }
}
