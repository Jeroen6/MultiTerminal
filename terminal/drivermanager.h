#ifndef DRIVERMANAGER_H
#define DRIVERMANAGER_H
#include <Qt>
#include <QObject>
#include <QProcess>
#include "config.h"

#define USE_COM2TCP         0
#define USE_SERIALDRIVER    1

class DriverManager : public QObject
{
    Q_OBJECT

public:
    DriverManager(Config::TerminalSettings_t *s);
    ~DriverManager();

    typedef enum { STOPPED, STARTING, RUNNING, ABORTED } DriverState;

    void applyUserSettings();

    void connectSerial();
    void disconnectSerial();

    void streamFile(QString path);

    DriverState getState() const;
    QString getLastError() const;

    void errorHandled();

signals:
    void stateChanged(DriverManager::DriverState);

private slots:
    void process_stateChanged(QProcess::ProcessState);
    void driverStdoutDebug();

private:
    Config::TerminalSettings_t *user_settings;
    DriverState state;
    QString lastError;
    QString driver_path;

    /* Process */
    QProcess *driver;
    bool driverExpectStop;
    QString tcpServerArgumentString; /// Used to call SerialDriver
//tcpServerArgumentString = QString("127.0.0.1 ")+QString().setNum(user_settings.server.portnumber);


};

/// preprocessor error checking
#if USE_COM2TCP && USE_SERIALDRIVER
#error USE_COM2TCP & USE_SERIALDRIVER cannot both be true
#endif


#endif // DRIVERMANAGER_H
