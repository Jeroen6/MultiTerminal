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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QLabel>
#include <QListWidget>
#include <QThread>
#include <QFile>
#include "lineassembler_class.h"
#include "filtermanager.h"
#include "view.h"
#include "viewmanager.h"
#include "about.h"
#include "config.h"
#include "drivermanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void fileStreamerReadFile(const QStringList arguments);

private slots:
    void on_buttonAddFilter_clicked();

    void on_buttonRemoveFilter_clicked();

    void on_listFilter_doubleClicked(const QModelIndex &index);

    void on_lineEditFilter_returnPressed();

    void acceptConnection();

    void lostConnection();

    void newData();

    void displayError(QAbstractSocket::SocketError socketError);

    void on_actionConnect_triggered();

    void driver_stateChanged(DriverManager::DriverState state);

    void on_buttonSend_clicked();

    void closeEvent(QCloseEvent *event);

    void on_listFilter_itemChanged(QListWidgetItem *item);

    void updateViews();

    void on_actionRun_triggered();

    void on_actionAbout_triggered();

    void on_actionDisconnect_triggered();

    void updateMainWindow();

    void on_actionConfiguration_triggered();

    void config_closed(Config::ConfigExitCode exitCode);

    void on_viewVisibilityChanged(QString filter);

    void on_actionOpen_file_triggered();

    void on_buttonWipeInput_clicked();

    void on_buttonWipeOutput_clicked();

    void on_buttonWipeAll_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *mainWindowUpdater;
    About *about;
    Config *config;
    QLabel statusbartext;

    /* Driver */
    DriverManager *driverManager;

    /* Server */
    QTcpServer *tcpServer;
    QTcpSocket *tcpServerConnection;
    bool tcpServerConnectionValid;
    QLabel tcpServerStatus;

    /* Filter  */
    LineAssembler *lineAssembler;
    FilterManager *filters;

    /* Views */
    ViewManager *views;
    QList<int> spawnedViews;
    QList<Qt::CheckState> *checkboxes;

    /* Viewupdater */
    QTimer *viewUpdater;

    /* Configuration window */
    Config::TerminalSettings_t user_settings;
    void applyUserSettings();
    void defaultUserSettings();
};



#endif // MAINWINDOW_H
