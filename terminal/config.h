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
#ifndef CONFIG_H
#define CONFIG_H

#include <QDialog>
#include <QTreeWidget>
#include <QSerialPort>

namespace Ui {
class Config;
}

class Config : public QDialog
{
    Q_OBJECT

public:
    explicit Config(QWidget *parent = 0);
    ~Config();

    /// @typedef ConfigExitCode as exit code typedef
    typedef enum { DISCARD, SAVE, APPLY } ConfigExitCode;

    /// @typedef Reconnect_t for simple reconnect option
    typedef enum { No=0, Immediately = 1000, After10Seconds = 10000 } Reconnect_t;

    /// @typedef configuration structure for terminal as used in config window
    typedef struct {
        /// @struct serial configuration
        struct serial_config_t{
            QString tty; /// tty or com number
            quint32 baud;/// any baudrate
            QSerialPort::DataBits databits;     /// DataBits as enum
            QSerialPort::StopBits stopbits;     /// StopBits as enum
            QSerialPort::Parity parity;         /// Parity as enum
            QSerialPort::FlowControl flowctrl;  /// FlowControl as enum
            Reconnect_t reconnect;              /// Number of milliseconds in @p Reconnect_t before reconnect
        }serial;
        /// @struct server configuration
        struct server_config_t{
            quint16 portnumber;     /// 16 bit TCPv4 portnumber
            bool localOnly;  /// Allow only localhosted connection
        }server;
    } TerminalSettings_t;

    void LoadSettings(TerminalSettings_t s);
    TerminalSettings_t GetSettings() { return settings; }

    void update();  /// Update configview with data form @p TerminalSettings_t and serialports

signals:
    void closed(Config::ConfigExitCode exitCode);

private slots:
    void on_buttonOk_clicked();

    void on_buttonCancel_clicked();

    void on_buttonApply_clicked();

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_buttonDefaults_clicked();

private:
    Ui::Config *ui;

    QList<QTreeWidgetItem *> treeWidgetItems;
    QList< QList<qint32> > baudRatesPerTreeIndex;

    void closeEvent(QCloseEvent * e);
    void clearSerialPortList();

    TerminalSettings_t settings;
    void reject();
};

#endif // CONFIG_H
