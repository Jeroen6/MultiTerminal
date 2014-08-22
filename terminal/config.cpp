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
#include "config.h"
#include "ui_config.h"
#include <QtSerialPort>
#include <QTreeWidget>

Config::Config(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Config)
{
    ui->setupUi(this);
}

Config::~Config()
{
    delete ui;
}

/// Load settings from struct to gui
void Config::LoadSettings(Config::TerminalSettings_t s)
{
    settings = s;
    qint32 i = ui->boxBaudrate->findText(QString().setNum(s.serial.baud));
    if(i<0) i=0;
    ui->boxBaudrate->setCurrentIndex(i);
    ui->editBaudrate->setText(QString().setNum(s.serial.baud));
    i = ui->boxBaudrate->findText(s.serial.tty);
    if(i<0) i=0;
    ui->boxTTY->setCurrentIndex(i);

    switch(s.serial.parity){
    case QSerialPort::EvenParity:
        settings.serial.parity = QSerialPort::EvenParity;
        ui->boxParity->setCurrentIndex(1);
        break;
    case QSerialPort::OddParity:
        ui->boxParity->setCurrentIndex(2);
        break;
    case QSerialPort::SpaceParity:
        ui->boxParity->setCurrentIndex(3);
        break;
    case QSerialPort::MarkParity:
        ui->boxParity->setCurrentIndex(4);
        break;
    case QSerialPort::NoParity:
    default:
        ui->boxParity->setCurrentIndex(0);
        break;
    }

    switch(settings.serial.stopbits){
    case QSerialPort::OneAndHalfStop:
        ui->boxStopbits->setCurrentIndex(1);
        break;
    case QSerialPort::TwoStop:
        ui->boxStopbits->setCurrentIndex(2);
        break;
    case QSerialPort::OneStop:
    default:
        ui->boxStopbits->setCurrentIndex(0);
        break;
    }

    switch(s.serial.databits){
    case  QSerialPort::Data7:
        ui->boxDatabits->setCurrentIndex(1);
        break;
    case QSerialPort::Data6:
        ui->boxDatabits->setCurrentIndex(2);
        break;
    case QSerialPort::Data5:
        ui->boxDatabits->setCurrentIndex(3);
        break;
    case QSerialPort::Data8:
    default:
        ui->boxDatabits->setCurrentIndex(0);
        break;
    }

    switch(s.serial.reconnect){
    case Immediately:
        ui->boxReconnect->setCurrentIndex(1);
        break;
    case After10Seconds:
        ui->boxReconnect->setCurrentIndex(2);
        break;
    case No:
    default:
        ui->boxReconnect->setCurrentIndex(0);
        break;
    }

    switch(settings.serial.flowctrl){
    case QSerialPort::HardwareControl:
        ui->boxFlowctrl->setCurrentIndex(1);
        break;
    case QSerialPort::SoftwareControl:
        ui->boxFlowctrl->setCurrentIndex(2);
        break;
    case QSerialPort::NoFlowControl:
    default:
        ui->boxFlowctrl->setCurrentIndex(0);
        break;
    }

    ui->boxTCPPort->setValue( s.server.portnumber );
    ui->checkOnlyLocal->setChecked(s.server.localOnly);
}

/// General event on close window
void Config::closeEvent( QCloseEvent * event )
{
    (void)event;
    this->hide();
    clearSerialPortList();
    closed(DISCARD);
}

/// Escape key close
void Config::reject()
{
    this->hide();
    clearSerialPortList();
    closed(DISCARD);
}

/// Pull settings from GUI and store in struct
void Config::on_buttonOk_clicked()
{
    settings.serial.baud        = ui->boxBaudrate->currentText().toInt();
    switch(ui->boxDatabits->currentIndex()){
    case 1:
        settings.serial.databits = QSerialPort::Data7;
        break;
    case 2:
        settings.serial.databits = QSerialPort::Data6;
        break;
    case 3:
        settings.serial.databits = QSerialPort::Data5;
        break;
    case 0:
    default:
        settings.serial.databits = QSerialPort::Data8;
        break;
    }

    switch(ui->boxStopbits->currentIndex()){
    case 1:
        settings.serial.stopbits = QSerialPort::OneAndHalfStop;
        break;
    case 2:
        settings.serial.stopbits = QSerialPort::TwoStop;
        break;
    case 0:
    default:
        settings.serial.stopbits = QSerialPort::OneStop;
        break;
    }

    switch(ui->boxParity->currentIndex()){
    case 1:
        settings.serial.parity = QSerialPort::EvenParity;
        break;
    case 2:
        settings.serial.parity = QSerialPort::OddParity;
        break;
    case 3:
        settings.serial.parity = QSerialPort::SpaceParity;
        break;
    case 4:
        settings.serial.parity = QSerialPort::MarkParity;
        break;
    case 0:
    default:
        settings.serial.parity = QSerialPort::NoParity;
        break;
    }

    switch(ui->boxFlowctrl->currentIndex()){
    case 0:
        settings.serial.flowctrl = QSerialPort::NoFlowControl;
        break;
    case 1:
        settings.serial.flowctrl = QSerialPort::HardwareControl;
        break;
    case 2:
        settings.serial.flowctrl = QSerialPort::SoftwareControl;
        break;
    default:
        break;
    }

    switch(ui->boxReconnect->currentIndex()){
    case 1:
        settings.serial.reconnect = Immediately;
        break;
    case 2:
        settings.serial.reconnect = After10Seconds;
        break;
    case 0:
    default:
        settings.serial.reconnect = No;
        break;
    }

    if(ui->boxTTY->currentText() != QString("Select port")){
        settings.serial.tty = ui->boxTTY->currentText();
    }

    settings.server.portnumber = ui->boxTCPPort->value();
    settings.server.localOnly = (ui->checkOnlyLocal->checkState()==Qt::Checked)?(true):(false);

    this->hide();
    clearSerialPortList();
    closed(SAVE);
}

/// Cancel button clicked, don't store settings
void Config::on_buttonCancel_clicked()
{
    this->hide();
    clearSerialPortList();
    closed(DISCARD);
}

/// Apply button clicked, not implemented
void Config::on_buttonApply_clicked()
{
    /*
    this->hide();
    clearSerialPortList();
    closed(APPLY);
    */
}

/// Empty the serial port list
void Config::clearSerialPortList(){
    while(treeWidgetItems.count()){
        delete treeWidgetItems.first();
        treeWidgetItems.removeFirst();
    }
    baudRatesPerTreeIndex.clear();
}

/// Update serialport list
void Config::update()
{
    // Prep comboBox
    ui->boxTTY->clear();
    ui->boxTTY->addItem(QString("Select port"));
    ui->boxBaudrate->clear();
    ui->boxBaudrate->addItem(QString("Select port"));

    // List ports and add to list
    QList<QSerialPortInfo> serialPortInfoList = QSerialPortInfo::availablePorts();
    foreach (const QSerialPortInfo &serialPortInfo, serialPortInfoList) {
        // Create
        QTreeWidgetItem *port = new QTreeWidgetItem(ui->treeWidget);
        QTreeWidgetItem *loc  = new QTreeWidgetItem(port);
        QTreeWidgetItem *des  = new QTreeWidgetItem(port);
        QTreeWidgetItem *man  = new QTreeWidgetItem(port);
        QTreeWidgetItem *vid  = new QTreeWidgetItem(port);
        QTreeWidgetItem *pid  = new QTreeWidgetItem(port);
        QTreeWidgetItem *busy  = new QTreeWidgetItem(port);
        // Fill
        port->setText(0, QString ( serialPortInfo.portName() ) );
        ui->boxTTY->addItem( serialPortInfo.portName() );
        loc ->setText(0, QString ( serialPortInfo.systemLocation() ) );
        des ->setText(0, QString ( serialPortInfo.description() ));
        man ->setText(0, QString ( serialPortInfo.manufacturer() ));
        vid ->setText(0, QString("VID: ")+QString ( (serialPortInfo.hasVendorIdentifier() ? QByteArray::number(serialPortInfo.vendorIdentifier(), 16) : QByteArray()) ));
        pid ->setText(0, QString("PID: ")+QString ( (serialPortInfo.hasProductIdentifier() ? QByteArray::number(serialPortInfo.productIdentifier(), 16) : QByteArray())) );
        busy->setText(0, QString ( (serialPortInfo.isBusy() ? QObject::tr("Busy: Yes") : QObject::tr("Busy: No")) ));
        port->setExpanded(true);
        baudRatesPerTreeIndex.append(serialPortInfo.standardBaudRates());

        // Keep reference
        treeWidgetItems.append(port);

        // If this port is the one in config
        if(port->text(0) == settings.serial.tty)
            on_treeWidget_itemClicked(port,0);
    }

    // Update all other widgets

}

/// Fill in the blanks when clicked on a treewidgetitem
void Config::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    QString text;
    // Only act on PARENT selects
    if(column == 0){
        // Fill boxBaudrate
        ui->boxBaudrate->clear();
        foreach (qint32 baud, baudRatesPerTreeIndex.at(0)) {
            text.setNum(baud);
            ui->boxBaudrate->addItem(text);
        }
        qint32 i = ui->boxBaudrate->findText(QString().setNum(settings.serial.baud));
        if(i<0) i=0;
        ui->boxBaudrate->setCurrentIndex(i);
        // Fill boxTTY
        text = item->text(0);
        i = ui->boxTTY->findText(text);
        if(i<0){
            // Fill boxTTY, pointing to parent
            text = item->parent()->text(0);
            qint32 i = ui->boxTTY->findText(text);
            if(i<0) i=0;
            ui->boxTTY->setCurrentIndex(i);
        }else{
            ui->boxTTY->setCurrentIndex(i);
        }
    }
}

/// Reset defaults
void Config::on_buttonDefaults_clicked()
{
    settings.serial.tty        = "COM1";
    settings.serial.baud       = 9600;
    settings.serial.databits   = QSerialPort::Data8;
    settings.serial.stopbits   = QSerialPort::OneStop;
    settings.serial.parity     = QSerialPort::NoParity;
    settings.serial.flowctrl   = QSerialPort::NoFlowControl;
    settings.server.portnumber = 6666;
    settings.server.localOnly  = false;
    settings.serial.reconnect  = Config::No;
    LoadSettings(settings);
}

/// Refresh complete view
void Config::on_buttonRefresh_clicked()
{
    clearSerialPortList();
    this->update();
}
