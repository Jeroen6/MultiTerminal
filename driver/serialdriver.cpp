/* MultiTerminal SerialDriver (tunnels a tty to TCP)
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
#include "serialdriver.h"

#include <Qt>
#include <QtSerialPort>
#include <QtNetwork>
#include <QThread>
#include <QTimer>

SerialDriver::SerialDriver()
{
    argumentError = false;

    connect(&serial,SIGNAL(readyRead()),this,SLOT(newSerialData()));
    connect(&socket,SIGNAL(readyRead()),this,SLOT(socketData()));

    connect(&serial,SIGNAL(error(QSerialPort::SerialPortError)),this,SLOT(serialControl(QSerialPort::SerialPortError)));
    connect(&socket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(socketControl(QAbstractSocket::SocketState)));

    serialRXAge.setInterval(100);
    connect(&serialRXAge, SIGNAL(timeout()),this,SLOT(bufferAged()));
}

void SerialDriver::bufferAged(){
    socket.write(serialRXbuffer);
    serialRXbuffer.clear();
    serialRXAge.stop();
}

void SerialDriver::newSerialData(){
    QTextStream cout(stdout);
    serialRXbuffer += serial.readAll();
    // wait for at least 8 bytes
    if(serialRXbuffer.count() > 8)
    {
        socket.write(serialRXbuffer);
        serialRXbuffer.clear();
    }
    else
    {
        // but make sure nothing is left behind
        serialRXAge.start();
    }
}

void SerialDriver::serialControl(QSerialPort::SerialPortError serror){
        QTextStream cout(stdout);
    switch(serror){
    case QSerialPort::NoError:
        break;
    case QSerialPort::DeviceNotFoundError:
        cout << "Serial: " << serial.errorString() << endl;
        exit(FileError);
        break;
    case QSerialPort::PermissionError:
        cout << "Serial: " << serial.errorString() << endl;
        exit(FileError);
        break;
    case QSerialPort::OpenError:
        cout << "Serial: " << serial.errorString() << endl;
        exit(FileError);
        break;
    case QSerialPort::NotOpenError:
        cout << "Serial: " << serial.errorString() << endl;
        exit(FileError);
        break;
    case QSerialPort::ParityError:
        break;
    case QSerialPort::FramingError:
        break;
    case QSerialPort::BreakConditionError:
        break;
    case QSerialPort::WriteError:
        cout << "Serial: " << serial.errorString() << endl;
        exit(FileError);
        break;
    case QSerialPort::ReadError:
        cout << "Serial: " << serial.errorString() << endl;
        exit(FileError);
        break;
    case QSerialPort::ResourceError:
        cout << "Serial: " << serial.errorString() << endl;
        exit(FileError);
        break;
    case QSerialPort::UnsupportedOperationError:
        cout << "Serial: " << serial.errorString() << endl;
        exit(FileError);
        break;
    case QSerialPort::TimeoutError:
    default:
        break;
    }
}


void SerialDriver::socketData(){
    QTextStream cout(stdout);
    QByteArray data = socket.readAll();
    serial.write(data);
    cout << data << endl;
}

void SerialDriver::socketControl(QAbstractSocket::SocketState state){
        QTextStream cout(stdout);
    switch(state){
    case QAbstractSocket::UnconnectedState:
        cout << "TCP: " << socket.errorString() << endl;
        exit(TCPStreamError);
        break;
    case QAbstractSocket::HostLookupState:
        break;
    case QAbstractSocket::ConnectingState:
        // This is OK
        break;
    case QAbstractSocket::ConnectedState:
        break;
    case QAbstractSocket::BoundState:
        break;
    case QAbstractSocket::ClosingState:
        break;
    case QAbstractSocket::ListeningState:
        break;
    default:
        break;
    }

}

int SerialDriver::start(void){

    QTextStream cout(stdout);
    int error = Success;
    cout << "Starting serialport session with arguments: " << endl
         << port << " @ " << baudrate << " with: "
         << databits << " databits, "
         << stopbits << " stopbits, "
         << parity << " parity and "
         << flowctrl << " flow control" << endl;
    if(!findPortInfoStruct()){
        cout << "No such port" << endl;
        error = FileError;
        return error;
    }
    serial.setPort(portInfo);
    if(serial.open(QIODevice::ReadWrite)){
        cout << "Serial opened" << endl;
        if(!serial.setBaudRate(baudrate))
            cout << "Could not configure baudrate " << endl;
        if(!serial.setDataBits(databits))
            cout << "Could not configure databits " << endl;
        if(!serial.setFlowControl(flowctrl))
            cout << "Could not configure flowcontrol " << endl;
        if(!serial.setParity(parity))
            cout << "Could not configure parity " << endl;
        if(!serial.setStopBits(stopbits))
            cout << "Could not configure stopbits " << endl;
        if(serial.error() != 0){
            cout << serial.error() << " " << serial.errorString() << endl;
            error = FileError;
            return error;
        }else{
            // Serialport configured
            cout << "Connecting to " << server << ":" << serverport << endl;
            socket.connectToHost(QHostAddress(server), serverport, QIODevice::ReadWrite);
            // Verify
            if(socket.waitForConnected(10000))
            {
                cout << "Connected" << endl;
            }
            else
            {
                // Not able to connect
                error = ServerTimeout;
                socket.close();
                cout << "Server timeout" << endl;
                error = ServerTimeout;
            }
        }
    }else{
        cout << "Serial open failed " << serial.error() << endl;
        error = FileError;
    }
    return error;
}

void SerialDriver::setPort(QString s)
{
    port = s;
}

void SerialDriver::setBaudrate(QString s)
{
    bool succes = false;
    baudrate = s.toInt(&succes);
    if(!succes)
    {
        baudrate = 0;
        argumentError = true;
    }
}

void SerialDriver::setParity(QString s)
{
    if(s == "N"){
        // None
        parity = QSerialPort::NoParity;
    }else if(s == "E"){
        // Even
        parity = QSerialPort::EvenParity;
    }else if(s == "O"){
        // Odd
        parity = QSerialPort::OddParity;
    }else if(s == "M"){
        // Mark
        parity = QSerialPort::SpaceParity;
    }else if(s == "S"){
        // Space
        parity = QSerialPort::MarkParity;
    }else{
        // Unknown
        parity = QSerialPort::NoParity;
        argumentError = true;
    }
}

void SerialDriver::setStopbits(QString s)
{
    if(s == "1"){
        stopbits = QSerialPort::OneStop;
    }else if(s == "1.5"){
        stopbits = QSerialPort::OneAndHalfStop;
    }else if(s == "2"){
        stopbits = QSerialPort::TwoStop;
    }else{
        stopbits = QSerialPort::OneStop;
        argumentError = true;
    }
}

void SerialDriver::setFlowctrl(QString s)
{
    if(s == "N"){
        flowctrl = QSerialPort::NoFlowControl;
    }else if(s == "H"){
        flowctrl = QSerialPort::HardwareControl;
    }else if(s == "S"){
        flowctrl = QSerialPort::SoftwareControl;
    }else{
        flowctrl = QSerialPort::NoFlowControl;
        argumentError = true;
    }
}

void SerialDriver::setDatabits(QString s)
{
    if(s == "8"){
        databits = QSerialPort::Data8;
    }else if(s == "7"){
        databits = QSerialPort::Data7;
    }else if(s == "6"){
        databits = QSerialPort::Data6;
    }else if(s == "5"){
        databits = QSerialPort::Data5;
    }else{
        databits = QSerialPort::Data8;
        argumentError = true;
    }
}

void SerialDriver::setHost(QString s, QString port )
{
    // Stream file to TCP
    server = s;
    bool conversionSucces;
    serverport = port.toInt(&conversionSucces);
    if(!conversionSucces){
        argumentError = true;
    }
}

bool SerialDriver::getArgumentError() const
{
    return argumentError;
}

bool SerialDriver::findPortInfoStruct(){
    QList<QSerialPortInfo> serialPortInfoList = QSerialPortInfo::availablePorts();
    bool found = false;

    foreach (const QSerialPortInfo &serialPortInfo, serialPortInfoList) {
        if(serialPortInfo.portName() == port){
            portInfo = serialPortInfo;
            found = true;
        }
    }
    return found;
}

