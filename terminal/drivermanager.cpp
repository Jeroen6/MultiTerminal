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
#include <Qt>
#include <QObject>
#include <QProcess>
#include "drivermanager.h"

DriverManager::DriverManager(Config::TerminalSettings_t *s)
{
    user_settings = s;
    applyUserSettings();


    // Create tty->tcp
    driver = new QProcess();
    connect(driver,SIGNAL(stateChanged(QProcess::ProcessState)),
            this,SLOT(process_stateChanged(QProcess::ProcessState)));
    connect(driver,SIGNAL(readyRead()),this,SLOT(driverStdoutDebug()));

    driverExpectStop = false;
    state = STOPPED;


#ifdef Q_OS_WIN
#if USE_COM2TCP
    driver_path = "com2tcp.exe";
#endif
#if USE_SERIALDRIVER
    driver_path = "SerialDriver.exe";
#endif
#elif Q_OS_LINUX
    /** @todo handle this */
#elif Q_OS_MAC
    /** @todo handle this */
#else
    /** @todo handle this */
#endif
}

DriverManager::~DriverManager()
{
    driverExpectStop = true;
    driver->kill();
    driver->waitForFinished();
    delete driver;
}

void DriverManager::driverStdoutDebug()
{
    while(driver->bytesAvailable())
        qDebug(driver->readLine());
}

void DriverManager::applyUserSettings()
{

}

void DriverManager::connectSerial()
{
#if USE_COM2TCP
    /// @todo Use compiled string from settings
    //QString exe_args("--baud 9600 --ignore-dsr COM7 127.0.0.1 6666");
    QString compiled_args;
    compiled_args += "--baud ";
    compiled_args += QString().setNum(user_settings->serial.baud);
    compiled_args += " --ignore-dsr ";
    compiled_args += user_settings->serial.tty;
    compiled_args += " 127.0.0.1 ";
    compiled_args += QString().setNum(user_settings->server.portnumber);
#endif
#if USE_SERIALDRIVER
    QString parity;
    QString databits;
    QString stopbits;
    QString flowctrl;
    switch(user_settings->serial.parity){
    case QSerialPort::EvenParity:
        parity = "E";
        break;
    case QSerialPort::OddParity:
        parity = "O";
        break;
    case QSerialPort::SpaceParity:
        parity = "S";
        break;
    case QSerialPort::MarkParity:
        parity = "M";
        break;
    case QSerialPort::NoParity:
    default:
        parity = "N";
        break;
    }
    switch(user_settings->serial.databits){
    case  QSerialPort::Data7:
        databits = "7";
        break;
    case QSerialPort::Data6:
        databits = "6";
        break;
    case QSerialPort::Data5:
        databits = "5";
        break;
    case QSerialPort::Data8:
    default:
        databits = "8";
        break;
    }
    switch(user_settings->serial.stopbits){
    case QSerialPort::OneAndHalfStop:
        stopbits = "1.5";
        break;
    case QSerialPort::TwoStop:
        stopbits = "2";
        break;
    case QSerialPort::OneStop:
    default:
        stopbits = "1";
        break;
    }
    switch(user_settings->serial.flowctrl){
    case QSerialPort::HardwareControl:
        flowctrl = "H";
        break;
    case QSerialPort::SoftwareControl:
        flowctrl = "S";
        break;
    case QSerialPort::NoFlowControl:
    default:
        flowctrl = "N";
        break;
    }
    QString compiled_args;
    compiled_args += " -t ";
    compiled_args += user_settings->serial.tty;
    compiled_args += " -b ";
    compiled_args += QString().setNum(user_settings->serial.baud);
    compiled_args += " -p ";
    compiled_args += parity;
    compiled_args += " -d ";
    compiled_args += databits;
    compiled_args += " -e ";
    compiled_args += stopbits;
    compiled_args += " -f ";
    compiled_args += flowctrl;
    compiled_args += " 127.0.0.1 ";
    compiled_args += QString().setNum(user_settings->server.portnumber);
#endif
    driver->setProgram(driver_path);
    driver->setNativeArguments(compiled_args);
    driver->open();
}

void DriverManager::disconnectSerial()
{
    driverExpectStop = true;
    driver->kill();
}

void DriverManager::streamFile(QString path)
{
    /// @todo call SerialDriver with -s filename 127.0.0.1 6666
    QString exe_path("SerialDriver.exe");
    QString exe_args(QString("-s ") + path + QString(" 127.0.0.1 ") + QString().setNum(user_settings->server.portnumber));
    driver->setProgram(exe_path);
    driver->setNativeArguments(exe_args);
    driver->open();
    driverExpectStop = true;
}

/// @brief return driverstate
DriverManager::DriverState DriverManager::getState() const
{
    return state;
}

/// @brief return last error in human
QString DriverManager::getLastError() const
{
    return lastError;
}

void DriverManager::errorHandled()
{
    state = STOPPED;
    stateChanged(state);
}

/// @brief handle process state changes
void DriverManager::process_stateChanged(QProcess::ProcessState dstate)
{
    driverStdoutDebug();
    switch(dstate){
    case QProcess::NotRunning:
        if(driverExpectStop){
            // Expected stop
            state = STOPPED;
        }else{
            // Unexpected stop
            state = ABORTED;
#if USE_COM2TCP
            lastError = "Unknown error";
#endif
#if USE_SERIALDRIVER
            switch(driver->exitCode()){
            case 0:
                // no error, just stopped?
                break;
            case 1:
                lastError = "ArgumentError, wrong settings.";
                break;
            case 2:
                lastError = "ServerTimeout, couldn't find server.";
                break;
            case 3:
                lastError = "TCPStreamError, the server terminated communications.";
                break;
            case 4:
                lastError = "FileError, the serialport doesn't exist and/or cannot be opened.";
                break;
            }
#endif
        }
        break;
    case QProcess::Starting:
        state = STARTING;
        break;
    case QProcess::Running:
        state = RUNNING;
        break;
    }
    driverExpectStop = false;
    stateChanged(state);
}

