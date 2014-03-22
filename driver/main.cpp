#include <QTextStream>
#include <QCoreApplication>
#include <QtSerialPort/QSerialPortInfo>
#include <QCommandLineParser>
#include <Qt>
#include <QtNetwork>
#include <QElapsedTimer>
#include <QByteArray>
#include "serialdriver.h"

QT_USE_NAMESPACE

/* Include C features, dirty but effective */

/* Globals */
typedef enum {Success = 0,
             ArgumentError = 1,
             ServerTimeout = 2,
             TCPStreamError = 3,
             FileError = 4,
             UnknownError = -1} SerialDriverExitCode;

/* Functions */
int list_ports(void){
    QTextStream out(stdout);
    QList<QSerialPortInfo> serialPortInfoList = QSerialPortInfo::availablePorts();

    out << QObject::tr("Total number of ports available: ") << serialPortInfoList.count() << endl;

    foreach (const QSerialPortInfo &serialPortInfo, serialPortInfoList) {
        out << endl
            << QObject::tr("Port: ") << serialPortInfo.portName() << endl
            << QObject::tr("Location: ") << serialPortInfo.systemLocation() << endl
            << QObject::tr("Description: ") << serialPortInfo.description() << endl
            << QObject::tr("Manufacturer: ") << serialPortInfo.manufacturer() << endl
            << QObject::tr("Vendor Identifier: ") << (serialPortInfo.hasVendorIdentifier() ? QByteArray::number(serialPortInfo.vendorIdentifier(), 16) : QByteArray()) << endl
            << QObject::tr("Product Identifier: ") << (serialPortInfo.hasProductIdentifier() ? QByteArray::number(serialPortInfo.productIdentifier(), 16) : QByteArray()) << endl
            << QObject::tr("Busy: ") << (serialPortInfo.isBusy() ? QObject::tr("Yes") : QObject::tr("No")) << endl;
    }

    return 0;
}


/// Stream file to tcp and return
int stream_file_to_tcp(QString filename, QString server, quint16 port){
    QTextStream cout(stdout);
    int error = Success;
    //QElapsedTimer startTime;
    // startTime.start();
    QTcpSocket socket;
    // Connect
    socket.connectToHost(QHostAddress(server),port,QIODevice::ReadWrite);
    // Verify
    if(socket.waitForConnected(10000)){
        // Open file
        QFile file(filename);
        // Verify
        if (!file.open(QIODevice::ReadOnly)){
            cout << "File error : " << file.errorString() << endl;
            socket.close();
            error = FileError;
        }else{
            bool noRepeat = false;
            const int chuck_size = 256;
            // Transmit only when not end of file
            while( (file.atEnd() == false) && (file.pos() < file.size()) && (noRepeat == false) ){
                // Transmit only when socket is valid
                if(socket.isValid() && socket.isWritable()){
                    // Calculate remaining bytes in file
                    int remains = file.size() - file.pos();
                    if(remains == 0){
                        // The file is fully transmitted!
                        noRepeat = true; // soft break;
                        file.close();
                    }
                    if(remains < chuck_size){
                        // Send remains
                        QByteArray data(remains, 0);
                        data = file.read(remains);
                        if(remains != socket.write(data)){
                            // There weren't enough bytes written, error
                            error = TCPStreamError;
                            file.close();
                            noRepeat = true;
                        }
                    }else{
                        // send 64 byte blocks
                        QByteArray data(chuck_size, 0);
                        data = file.read(chuck_size);
                        if(chuck_size != socket.write(data)){
                            // There weren't enough bytes written, error
                            error = TCPStreamError;
                            file.close();
                            noRepeat = true;
                        }
                    }
                    // Make sure the socket sends a packet!
                    socket.flush();
                }else{
                    // socket invalid
                    error = TCPStreamError;
                    file.close();
                    noRepeat = true;
                }
            }
            // Close the socket
            socket.close();
        }
    }else{
        // Not able to connect
        error = ServerTimeout;
        socket.close();
        cout << "Failed!" << endl;
    }
    return error;
}


/* Main */
int main(int argc, char *argv[])
{
    QTextStream cout(stdout);
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("SerialDriver");
    QCoreApplication::setApplicationVersion("(build: "__DATE__" "__TIME__")");

    // Visibility endline
    cout << endl;

    // Define command line arguments
    QCommandLineParser parser;
    QCommandLineOption listOption(QStringList() << "l" << "list",
                                  QCoreApplication::translate("main", "List tty ports"));
    QCommandLineOption streamOption(QStringList() << "s" << "stream",
                                    QCoreApplication::translate("main", "Stream the given file to TCP \neg: -s file.txt 127.0.0.1 100"),
                                    QCoreApplication::translate("main", "filename"));
    QCommandLineOption portNameOption(QStringList() << "t" << "serialport",
                                      QCoreApplication::translate("main", "Tunnel given port to TCP, COM1 or TTY1 \neg: -t COM1 -b 115200 127.0.0.1 100"),
                                      QCoreApplication::translate("main", "portname"));
    QCommandLineOption baudrateOption(QStringList() << "b" << "baudrate",
                                      QCoreApplication::translate("main", "baudrate eg: 9600"),
                                      QCoreApplication::translate("main", "baudrate"));
    QCommandLineOption portParityOption(QStringList() << "p" << "parity",
                                        QCoreApplication::translate("main", "parity N, E, O, M, S, None Even Odd Mark Space\neg: -p N"),
                                        QCoreApplication::translate("main", "parity"));
    QCommandLineOption portDatabitsOption(QStringList() << "d" << "databits",
                                        QCoreApplication::translate("main", "databits 5, 6, 7, 8, \neg: -d 8"),
                                        QCoreApplication::translate("main", "parity"));
    QCommandLineOption portStopBitsOption(QStringList() << "e" << "stopbits",
                                    QCoreApplication::translate("main", "stopbits, 1, 1.5 or 2\neg: -e 1"),
                                    QCoreApplication::translate("main", "stopbits"));
    QCommandLineOption portFlowctrlOption(QStringList() << "f" << "flowctrl",
                                    QCoreApplication::translate("main", "flowctrl N, H, S, None, CTS/RTS or Xon/Xoff\neg: -f N"),
                                    QCoreApplication::translate("main", "flowctrl"));

    baudrateOption.setDefaultValue(QString("9600"));
    portParityOption.setDefaultValue(QString("N"));
    portStopBitsOption.setDefaultValue(QString("1"));
    portFlowctrlOption.setDefaultValue(QString("N"));
    portDatabitsOption.setDefaultValue(QString("8"));

    parser.setApplicationDescription("Serialport to TCP Thread driver");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("TCP-server", QCoreApplication::translate("main", "TCP server"));
    parser.addPositionalArgument("TCP-port", QCoreApplication::translate("main", "TCP port"));
    parser.addOption(listOption);
    parser.addOption(streamOption);
    parser.addOption(portNameOption);
    parser.addOption(baudrateOption);
    parser.addOption(portParityOption);
    parser.addOption(portDatabitsOption);
    parser.addOption(portStopBitsOption);
    parser.addOption(portFlowctrlOption);

    // Process the actual command line arguments given by the user
    parser.process(app);
    const QStringList args = parser.positionalArguments();

    // List feature does not require any more
    if(parser.isSet(listOption))
    {
        // List ports
        return list_ports();
    }

    // Verify mandatory args for next steps
    if(args.size() < 1)
    {
        cout << "Incomplete mandatory arguments. (see --help)\nDid you forget the server address?" << endl << endl;
        return 1;   // Incomplete mandatory arguments
    }

    if(parser.isSet(portNameOption) && !parser.isSet(streamOption))
    {
        // Connect COM to TCP
        SerialDriver port;
        port.setPort(parser.value(portNameOption));
        port.setBaudrate(parser.value(baudrateOption));
        port.setParity(parser.value(portParityOption));
        port.setStopbits(parser.value(portStopBitsOption));
        port.setDatabits(parser.value(portDatabitsOption));
        port.setFlowctrl(parser.value(portFlowctrlOption));
        port.setHost( args.at(0), args.at(1) );

        if(port.getArgumentError()){
            return ArgumentError;
        }else{
            int e = port.start();
            if(e == 0)
                return app.exec();
            else
                return e;
        }
    }
    else if(!parser.isSet(portNameOption) && parser.isSet(streamOption))
    {
        // Stream file to TCP
        QString filename    = parser.value(streamOption);
        QString server      = args.at(0);
        bool conversionSucces;
        quint16 port = args.at(1).toInt(&conversionSucces);
        if(!conversionSucces){
            cout << "TCP port error" << endl << endl;
            return ArgumentError; // argument error
        }
        // Feedback
        cout << "Streaming: " << filename << " -> " << server << ":" << port << endl;
        // Transmit
        int err = stream_file_to_tcp(filename,server,port);
        switch(err){
        case ArgumentError:
            cout << "Success " << endl << endl;
            break;
        case ServerTimeout:
            cout << "Server timeout " << endl << endl;
            break;
        case TCPStreamError:
            cout << "Stream error " << endl << endl;
            break;
        case FileError:
            cout << "File error " << endl << endl;
            break;
        default:
            cout << "Error " << err << endl << endl;
            err = UnknownError;
            break;
        }
        return err;
    }

    // If you've arrived here none of the arguments given are valid
    cout << "Argument error, see --help" << endl << endl;
    parser.showHelp();

    return ArgumentError;
}

