#ifndef LINEASSEMBLER_CLASS_H
#define LINEASSEMBLER_CLASS_H

#include <Qt>
#include <QByteArray>
#include <QStringList>
#include <QElapsedTimer>

class LineAssembler
{
public:
    LineAssembler();
    ~LineAssembler();

    void push_chunk(QByteArray);
    void push_chunk(QString);
    void push_chunk(char* );

    // Status
    int lines_ready();
    int remains_ready();
    int remainder_age();

    //
    QString pull_line();
    QString pull_remains();

private:
    QByteArray *raw;
    QByteArray *temp;
    QStringList *lines;
    QElapsedTimer *age;

    void readLines();

    int lines_in_stock;
    int remains_in_stock;

};

#endif // LINEASSEMBLER_CLASS_H
