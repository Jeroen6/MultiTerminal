#ifndef SERIALDRIVER_H
#define SERIALDRIVER_H
#include <Qt>
#include <QObject>
#include <QtSerialPort>
#include <QtNetwork>

class SerialDriver : public QObject
{
    Q_OBJECT
public:

    typedef enum {Success = 0,
                 ArgumentError = 1,
                 ServerTimeout = 2,
                 TCPStreamError = 3,
                 FileError = 4,
                 UnknownError = -1} SerialDriverExitCode;

    SerialDriver();
    /// Setters
    void setPort(QString s);
    void setBaudrate(QString s);
    void setParity(QString s);
    void setStopbits(QString s);
    void setFlowctrl(QString s);
    void setDatabits(QString s);
    bool getArgumentError() const;
    void setHost(QString s, QString port);
    int start();

public slots:
    void newSerialData();
    void socketData();
    void bufferAged();
    void socketControl(QAbstractSocket::SocketState);
    void serialControl(QSerialPort::SerialPortError);

private:
    QByteArray serialRXbuffer;
    QTimer serialRXAge;
    bool argumentError;
    QString port;                       /// Portname
    quint32 baudrate;                   /// Baudrate
    QSerialPort::DataBits databits;     /// DataBits as enum
    QSerialPort::StopBits stopbits;     /// StopBits as enum
    QSerialPort::Parity parity;         /// Parity as enum
    QSerialPort::FlowControl flowctrl;  /// FlowControl as enum
    QTcpSocket socket;
	QSerialPort serial;
    QString server;
    quint16 serverport;
    QSerialPortInfo portInfo;
    bool findPortInfoStruct();

};

#endif // SERIALDRIVER_H
