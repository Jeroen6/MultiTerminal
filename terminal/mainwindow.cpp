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
#include <QtGlobal>
#include <QtNetwork>
#include <QListWidget>
#include <QStringListModel>
#include <QTimer>
#include <QThread>
#include <QtWidgets>
#include <QMessageBox>
#include <QProcess>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "view.h"
#include "viewmanager.h"
#include "about.h"
#include "drivermanager.h"

/// @brief mainwindow constructor
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Load settings
    /// @todo, replace for ini file or qsettings.
    defaultUserSettings();

    // Create secondary windows
    about = new About();
    config = new Config();
    // Connect close event, this inclused save
    connect(config,SIGNAL(closed(Config::ConfigExitCode)),this,SLOT(config_closed(Config::ConfigExitCode)));

    // Initialise the line assembler
    lineAssembler = new LineAssembler();

    // Create viewmanager
    views = new ViewManager();
    connect(views,SIGNAL(viewVisibilityChanged(QString)),this,SLOT(on_viewVisibilityChanged(QString)));

    // Create the filtermanager
    filters = new FilterManager();

    // Initialize listFilter system
    checkboxes = new QList<Qt::CheckState>;
    checkboxes->append(Qt::Checked);
    ui->listFilter->addItem(QString("*"));
    ui->listFilter->item(0)->setCheckState(Qt::Checked);
    ui->labelFilterTitle->setText(QString("Filters 0 of ").append(QString().setNum(views->getMaxViews())));

    // Create a driverManager
    driverManager = new DriverManager(&user_settings);
    // Connect driver state changed
    connect(driverManager,SIGNAL(stateChanged(DriverManager::DriverState)),
            this,SLOT(driver_stateChanged(DriverManager::DriverState)));

    // Platform specific init
#ifdef Q_OS_WIN

    //#elif Q_OS_LINUX
    /// @todo implement linux of tty->tcp
    //#elif Q_OS_MAC
    /// @todo implement mac of tty->tcp
#else
    QMessageBox msgBox;
    msgBox.setText("There is no com/tty driver support for your platform,\nplease connect to the tcp server manually ");
    msgBox.exec();
#endif

    // Start the TCP server
    tcpServer = new QTcpServer();
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
    while(!tcpServer->listen(QHostAddress::Any,user_settings.server.portnumber)){
        user_settings.server.portnumber++;
    }
    tcpServerConnectionValid = false;
    tcpServerStatus.setText("Server: Initializing");

    // Initialize the statusbar
    statusbartext.setText("<img src=':/icons/resources/icons/cross.png' /> Driver: Stopped");
    ui->statusBar->addWidget(&statusbartext,1);
    ui->statusBar->addWidget(&tcpServerStatus,1);

    // Spawn some timers for refreshing gui
    viewUpdater = new QTimer();
    connect(viewUpdater, SIGNAL(timeout()), this, SLOT(updateViews()));
    viewUpdater->setInterval(1);
    viewUpdater->start();
    mainWindowUpdater = new QTimer();
    connect(mainWindowUpdater,SIGNAL(timeout()),this,SLOT(updateMainWindow()));
    mainWindowUpdater->setInterval(100);
    mainWindowUpdater->start();
}

/// @brief mainwindow destructor
MainWindow::~MainWindow()
{
    delete driverManager;
    delete lineAssembler;
    delete views;
    delete filters;
    delete checkboxes;
    delete tcpServer;
    delete viewUpdater;
    delete mainWindowUpdater;
    delete config;
    delete about;
    delete ui;
}

/// @brief Timed update of views
void MainWindow::updateViews(){
    // For each filter, except *
    int i = ui->listFilter->count();
    while(i-- > 0){
        QString filter = ui->listFilter->item(i)->text();
        QString data = filters->readLine(filter);
        views->write(filter, data);
    }
}

/// @brief Update some mainwindow elements
void MainWindow::updateMainWindow(){
    QList<QAction*>actions = ui->menuConnect->actions();
    if(tcpServerConnectionValid){
        switch(tcpServerConnection->state()){
        case QAbstractSocket::UnconnectedState:
            break;
        case QAbstractSocket::HostLookupState :
            break;
        case QAbstractSocket::ConnectingState :
            break;
        case QAbstractSocket::ConnectedState  :
            tcpServerStatus.setText("<img src=':/icons/resources/icons/accept.png' /> Server: Connected with " + tcpServerConnection->peerAddress().toString());
            ui->buttonSend->setEnabled(true);
            actions.at(0)->setEnabled(false);
            break;
        case QAbstractSocket::BoundState      :
            break;
        case QAbstractSocket::ClosingState    :
            break;
        case QAbstractSocket::ListeningState  :
            break;
        default:
            break;
        }
    }else{
        if(tcpServer->isListening()){
            tcpServerStatus.setText("<img src=':/icons/resources/icons/error.png' /> Server: Listening at " + QString().setNum(user_settings.server.portnumber));
            actions.at(0)->setEnabled(true);
        }else{
            tcpServerStatus.setText("<img src=':/icons/resources/icons/cross.png' / Server:> Stopped");
        }
        ui->buttonSend->setEnabled(false);
    }
    // Update + button
    if((ui->listFilter->count()-1) < views->getMaxViews()){
        ui->buttonAddFilter->setEnabled(true);
    }else{
        ui->buttonAddFilter->setEnabled(false);
    }
}

/// Filter operators
/// @{
/// @brief Enter key linked to lineEditFilter
void MainWindow::on_lineEditFilter_returnPressed()
{
    on_buttonAddFilter_clicked();
}

/// @brief Add filter to filterlist
void MainWindow::on_buttonAddFilter_clicked()
{
    bool multiple = false;
    QString text = ui->lineEditFilter->text();
    // Only when text is valid, is not raw id, and not exceeding max number
    if(text.length() > 0
            && text != QString("*")
            && (ui->listFilter->count()-1) < views->getMaxViews() ){
        // Check for duplicates
        int i = ui->listFilter->count();
        while(i-- > 0){
            // Not allowed to remove 0
            if(ui->listFilter->item(i)->text() == text){
                multiple = 1; break;
            }
        }

        // Create
        if(!multiple){
            // Creat view (before list entry!)
            filters->add(text);
            views->spawn(text);
            views->show(text);
            // Add the element to list
            checkboxes->append(Qt::Checked);
            ui->listFilter->addItem(text);
            int index = ui->listFilter->count()-1;
            ui->listFilter->item(index)->setCheckState(Qt::Checked);
            ui->lineEditFilter->clear();
            ui->lineEditFilter->setFocus();
            // update text
            ui->labelFilterTitle->setText(QString("Filters ").
                                          append(QString().setNum(ui->listFilter->count()-1).
                                                 append(QString(" of ").
                                                        append(QString().setNum(views->getMaxViews())))));
        }
    }
}

/// @brief Remove filter from filterlist
void MainWindow::on_buttonRemoveFilter_clicked()
{
    QString text = ui->lineEditFilter->text();
    if(text.length() > 0){
        int i = ui->listFilter->count();
        while(i-- > 1){
            // Not allowed to remove 0
            if(ui->listFilter->item(i)->text() == text){
                checkboxes->removeAt(i);
                delete ui->listFilter->item(i);
                // Remove view (after list edit!)
                views->kill(views->getID(text));
                // Create filter
                filters->remove(text);
                // Load last entry for comfort
                int last = ui->listFilter->count()-1;
                if(i==0)
                    ui->lineEditFilter->clear();
                else
                    ui->lineEditFilter->setText(ui->listFilter->item(last)->text());
                ui->lineEditFilter->setFocus();
                // update text
                ui->labelFilterTitle->setText(QString("Filters ").
                                              append(QString().setNum(ui->listFilter->count()-1).
                                                     append(QString(" of ").
                                                            append(QString().setNum(views->getMaxViews())))));
            }
        }
    }
}

/// @brief Filterlist doubleclicked, place string in textbox
void MainWindow::on_listFilter_doubleClicked(const QModelIndex &index)
{
    int i = index.row();
    ui->lineEditFilter->setText(ui->listFilter->item(i)->text());
    //delete ui->listFilter->item(i);
}

/// @brief perform checkboxes changesteate signal in @p listFilter
void MainWindow::on_listFilter_itemChanged(QListWidgetItem *item)
{
    QList<Qt::CheckState> old_checkboxes;
    // age list
    old_checkboxes = *checkboxes;
    checkboxes->clear();
    // rebuid list
    for(int i=0; i<ui->listFilter->count(); i++){
        checkboxes->append(ui->listFilter->item(i)->checkState());
    }
    // compare list
    while(old_checkboxes != *checkboxes){
        for(int i=0; i<ui->listFilter->count(); i++){
            if(old_checkboxes.at(i) != checkboxes->at(i)){
                QString filter = item->text();
                // A checkbox has been changed
                if(filter == QString("*")){
                    //                    /*
                    //                    switch(item->checkState()){
                    //                    case Qt::Checked:
                    //                        ui->textInput->setEnabled(true);
                    //                        ui->textInput->setVisible(true);
                    //                        break;
                    //                    case Qt::Unchecked:
                    //                        ui->textInput->setEnabled(false);
                    //                        ui->textInput->setVisible(false);
                    //                        break;
                    //                    case Qt::PartiallyChecked:
                    //                        // not implemented
                    //                        break;
                    //                    }
                }else{
                    switch(item->checkState()){
                    case Qt::Checked:
                        /// @todo show, enabling filter
                        views->show(filter);
                        break;
                    case Qt::Unchecked:
                        /// @todo hide, disabling filter
                        views->hide(filter);
                        break;
                    case Qt::PartiallyChecked:
                        // not implemente
                        break;
                    }
                }
                // Equalize lists, preventing infinite run
                old_checkboxes[i] = checkboxes->at(i);
            }
        }
    }
    ui->listFilter->sortItems();
}
/// @}

/// @brief Connect button in menu
void MainWindow::on_actionConnect_triggered()
{
    driverManager->connectSerial();
}

/// @brief Driver has changed state
void MainWindow::driver_stateChanged(DriverManager::DriverState state){
    switch(state){
    case DriverManager::STOPPED:
    {
        QList<QAction*>actions = ui->menuConnect->actions();
        // Enable connect and Streamfile
        actions.at(0)->setEnabled(true);
        actions.at(1)->setEnabled(false);
        actions.at(3)->setEnabled(true);
        actions.at(5)->setEnabled(true);
        statusbartext.setText("<img src=':/icons/resources/icons/cross.png' /> Driver: Stopped");
    }
        break;
    case DriverManager::STARTING:
    {
        statusbartext.setText("<img src=':/icons/resources/icons/error.png' /> Driver: Starting");
    }
        break;
    case DriverManager::RUNNING:
    {
        statusbartext.setText("<img src=':/icons/resources/icons/accept.png' /> Driver: Running");
        QList<QAction*>actions = ui->menuConnect->actions();
        // Disbable connect and Streamfile
        actions.at(0)->setEnabled(false);
        actions.at(1)->setEnabled(true);
        actions.at(3)->setEnabled(false);
        actions.at(5)->setEnabled(false);
    }
        break;
    case DriverManager::ABORTED:
    {
        // There was an error, handle it
        QMessageBox msgBox;
        msgBox.setText(driverManager->getLastError());
        msgBox.exec();
        driverManager->errorHandled();
    }
        break;
    default:
        break;
    }
}

/// @brief menubar disconnect
void MainWindow::on_actionDisconnect_triggered()
{
    driverManager->disconnectSerial();
}

/// @brief menubar about
void MainWindow::on_actionAbout_triggered()
{
    about->show();
}

/// Server operators
/// @{
/// @brief Server received request, auto accept
void MainWindow::acceptConnection()
{
    tcpServerConnection = tcpServer->nextPendingConnection();
    if( (tcpServerConnection->peerAddress() != tcpServer->serverAddress())
            && (user_settings.server.localOnly))
    {
        // Reject
        tcpServerConnection->close();
    }else{
        // Valid
        tcpServerConnectionValid = true;

        connect(tcpServerConnection, SIGNAL(readyRead()),
                this, SLOT(newData()));
        connect(tcpServerConnection, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(displayError(QAbstractSocket::SocketError)));
        connect(tcpServerConnection, SIGNAL(disconnected()),
                this, SLOT(lostConnection()));
        connect(tcpServerConnection, SIGNAL(disconnected()),
                tcpServerConnection, SLOT(deleteLater()));
    }
}

/// @brief Socket disconnected, restart server
void MainWindow::lostConnection(){
    tcpServerConnectionValid = false;
}

/// @brief Socket newdata
void MainWindow::newData(){
    /// @todo replace for timer
    QByteArray data = tcpServerConnection->readAll();
    lineAssembler->push_chunk(data);

    while(lineAssembler->lines_ready()){
        QString line = lineAssembler->pull_line();
        filters->write(line);
        if(ui->listFilter->item(0)->checkState() == Qt::Checked){
            line.replace(QChar('\n'),QString("<font color=\"red\">\\n<\font>"));
            line.replace(QChar('\r'),QString("<font color=\"red\">\\r<\font>"));
            QCursor c = ui->textInput->cursor();
            ui->textInput->append(line);
            if(ui->boxAutoScrollInput->checkState()==Qt::Checked)
                ui->textInput->moveCursor(QTextCursor::End);
            else
                ui->textInput->setCursor(c);
        }
    }


}

/// @brief Socket error tied to socket
void MainWindow::displayError(QAbstractSocket::SocketError socketError)
{
    if (socketError == QTcpSocket::RemoteHostClosedError)
        return;

    QMessageBox::information(this, tr("Network error"),
                             tr("The following error occurred: ."));

    tcpServerConnection->close();
    tcpServer->listen(QHostAddress::Any,user_settings.server.portnumber);

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
}
/// @}

/// @brief Sendbutton
void MainWindow::on_buttonSend_clicked()
{
    if(tcpServerConnectionValid){
        if(tcpServerConnection->ConnectedState == QAbstractSocket::ConnectedState){
            QString text = ui->lineEditSend->text();

            text.replace(QString("\\a"),QString("\a"));
            text.replace(QString("\\b"),QString("\b"));
            text.replace(QString("\\f"),QString("\f"));
            text.replace(QString("\\n"),QString("\n"));
            text.replace(QString("\\r"),QString("\r"));
            text.replace(QString("\\t"),QString("\t"));
            text.replace(QString("\\v"),QString("\v"));
            text.replace(QString("\\0"),QString("\0"));

            QByteArray t(text.toStdString().c_str());
            tcpServerConnection->write(t);
            QCursor c = ui->textOutput->cursor();
            ui->textOutput->moveCursor(QTextCursor::End);
            ui->textOutput->insertPlainText(text);
            if(ui->boxAutoScrollOutput->checkState()==Qt::Checked)
                ui->textOutput->moveCursor(QTextCursor::End);
            else
                ui->textOutput->setCursor(c);
        }
    }
}

/// @brief Exit app
void MainWindow::closeEvent(QCloseEvent *event) {
    qApp->exit();
}

/// @brief Spawn some standard test filters
void MainWindow::on_actionRun_triggered()
{
    ui->lineEditFilter->setText(QString("a"));
    on_buttonAddFilter_clicked();
    ui->lineEditFilter->setText(QString("b"));
    on_buttonAddFilter_clicked();
    ui->lineEditFilter->setText(QString("c"));
    on_buttonAddFilter_clicked();
    ui->lineEditFilter->setText(QString("d"));
    on_buttonAddFilter_clicked();
}

/// @brief menubar open config
void MainWindow::on_actionConfiguration_triggered()
{
    config->setGeometry(this->geometry());
    //config->setFixedSize(this->size());
    config->LoadSettings(user_settings);
    config->show();
    this->setEnabled(false);
    config->update();
}

/// @brief config has closed
void MainWindow::config_closed(Config::ConfigExitCode exitCode)
{
    switch(exitCode){
    case Config::DISCARD:
        // Do nothing
        break;
    case Config::SAVE:
        // Copy new settings from Config to MainWindow
        user_settings = config->GetSettings();
        applyUserSettings();
        break;
    case Config::APPLY:
        // Not implemented
        break;
    default:
        break;
    }
    this->setEnabled(true);
}

/// @brief handle closed views
void MainWindow::on_viewVisibilityChanged(QString filter){
    for(int i=0; i<ui->listFilter->count(); i++){
        if(ui->listFilter->item(i)->text() == filter){
            ui->listFilter->item(i)->setCheckState(Qt::Unchecked);
        }
    }
}

/// @brief Stream in a file
void MainWindow::on_actionOpen_file_triggered()
{    ///@todo fix file reading
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    if (dialog.exec()){
        QStringList fileNames = dialog.selectedFiles();
        driverManager->streamFile(fileNames.first());
    }
}

/// @brief Applies user settings from @var user_settings
void MainWindow::applyUserSettings(){
    if(!tcpServerConnectionValid){
        tcpServer->close();
        tcpServer->listen(QHostAddress::Any,user_settings.server.portnumber);
    }
    /// @todo Restart server with correct settings
    /// @todo Load other settings variables
}

/// @brief Load default user settings into @var user_settings
void MainWindow::defaultUserSettings(){
    user_settings.serial.tty        = "COM7";
    user_settings.serial.baud       = 9600;
    user_settings.serial.databits   = QSerialPort::Data8;
    user_settings.serial.stopbits   = QSerialPort::OneStop;
    user_settings.serial.parity     = QSerialPort::NoParity;
    user_settings.serial.flowctrl   = QSerialPort::NoFlowControl;
    user_settings.server.portnumber = 6666;
    user_settings.server.localOnly  = false;
    user_settings.serial.reconnect  = Config::No;
}

/// @brief wipe button Input textbox
void MainWindow::on_buttonWipeInput_clicked()
{
    ui->textInput->clear();
}

/// @brief wipe button Ouput textbox
void MainWindow::on_buttonWipeOutput_clicked()
{
    ui->textOutput->clear();
}

/// @brief Wipe all views
void MainWindow::on_buttonWipeAll_clicked()
{
    ui->textInput->clear();
    ui->textOutput->clear();
    views->wipeAll();
}

void MainWindow::on_actionSave_triggered()
{
    QString fn = QFileDialog::getSaveFileName(this, tr("Save as..."), QString(), tr("Text-files (*.txt);;All Files (*)"));
    int saveMode = -1;
    bool openSucces = false;
    if (fn.isEmpty()){
        //QMessageBox msgBox;
        //msgBox.setText("Invalid filename");
        //msgBox.exec();
    }else{
        QFile file(fn);
        if(file.exists()){
//            QMessageBox msgBox;
//            QPushButton *appendButton = msgBox.addButton(tr("Append"), QMessageBox::ActionRole);
//            QPushButton *overwriteButton = msgBox.addButton(tr("Overwrite"), QMessageBox::ActionRole);
//            QPushButton *cancelButton = msgBox.addButton(QMessageBox::Cancel);
//            msgBox.exec();
//            if (msgBox.clickedButton() == appendButton) {
//                 append
//                saveMode = 1;
//            }else if (msgBox.clickedButton() == overwriteButton) {
//                // overwrite
//                saveMode = 2;
//            }else if (msgBox.clickedButton() == cancelButton){
//                // abort
//                saveMode = -1;
//            }else{
//                // abort
//                saveMode = -1;
//            }
        }else{
            saveMode = 0;
        }
        // Open file
        switch(saveMode){
        case 0:
            // Create
            openSucces = file.open(QIODevice::WriteOnly);
            break;
        case 1:
            // Append
            openSucces = file.open(QIODevice::WriteOnly | QIODevice::Append);
            break;
        default:
        case 2:
            // Overwrite
            openSucces = file.open(QIODevice::WriteOnly | QIODevice::Truncate);
            break;
        }
        if (openSucces){
            file.close();
            QTextDocumentWriter w(fn);
            w.write(ui->textInput->document());
            /*
            QString *text;
            *text = ui->textInput->toPlainText();
            text->replace(QRegularExpression("<[^>]*>"),"");
            QByteArray *data = new QByteArray();
            data->append(*text);
            delete text;
            file.write(*data);
            */
        }else{
            // File error
            QMessageBox msgBox;
            msgBox.setText("Could not save file");
            msgBox.exec();
        }
    }
}

void MainWindow::on_actionSave_output_triggered()
{
    QString fn = QFileDialog::getSaveFileName(this, tr("Save as..."), QString(), tr("Text-files (*.txt);;All Files (*)"));
    int saveMode = -1;
    bool openSucces = false;
    if (fn.isEmpty()){
        //QMessageBox msgBox;
        //msgBox.setText("Invalid filename");
        //msgBox.exec();
    }else{
        QFile file(fn);
        if(file.exists()){
//            QMessageBox msgBox;
//            QPushButton *appendButton = msgBox.addButton(tr("Append"), QMessageBox::ActionRole);
//            QPushButton *overwriteButton = msgBox.addButton(tr("Overwrite"), QMessageBox::ActionRole);
//            QPushButton *cancelButton = msgBox.addButton(QMessageBox::Cancel);
//            msgBox.exec();
//            if (msgBox.clickedButton() == appendButton) {
//                 append
//                saveMode = 1;
//            }else if (msgBox.clickedButton() == overwriteButton) {
//                // overwrite
//                saveMode = 2;
//            }else if (msgBox.clickedButton() == cancelButton){
//                // abort
//                saveMode = -1;
//            }else{
//                // abort
//                saveMode = -1;
//            }
        }else{
            saveMode = 0;
        }
        // Open file
        switch(saveMode){
        case 0:
            // Create
            openSucces = file.open(QIODevice::WriteOnly);
            break;
        case 1:
            // Append
            openSucces = file.open(QIODevice::WriteOnly | QIODevice::Append);
            break;
        default:
        case 2:
            // Overwrite
            openSucces = file.open(QIODevice::WriteOnly | QIODevice::Truncate);
            break;
        }
        if (openSucces){
            file.close();
            QTextDocumentWriter w(fn);
            w.write(ui->textOutput->document());
            /*
            QString *text;
            *text = ui->textInput->toPlainText();
            text->replace(QRegularExpression("<[^>]*>"),"");
            QByteArray *data = new QByteArray();
            data->append(*text);
            delete text;
            file.write(*data);
            */
        }else{
            // File error
            QMessageBox msgBox;
            msgBox.setText("Could not save file");
            msgBox.exec();
        }
    }
}
