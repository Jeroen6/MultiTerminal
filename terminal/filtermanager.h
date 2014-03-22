#ifndef FILTERMANAGER_H
#define FILTERMANAGER_H

#include <vector>
#include <QString>
#include <QTextStream>

#define SUCCESS 0

// Filtertype
class Filter
{
public:
    Filter(){

    }

    ~Filter(){

    }

    QString mask;  /* mask is unique */
    QList<QString> buffer;
};

// Filtermanager
class FilterManager : public QObject
{
    Q_OBJECT
public:
    /// @note All filter views are id'd by the filter string
    int add(QString s);                 /// Add a filter
    int remove(QString s);              /// Remove a filter
    int write(QString data);            /// Write to filtermanager, data is filtered automatically
    int write(QString f, QString data); /// Write to a specific filter
    QString readLine(QString f);        /// Read a line from filtered output

private:
    QList<QString> unprocessed;     /// Unprocessed data
    QList<Filter*> filters;         /// List of filtered data lists

    void filterEngine();            /// The filter algorithm
    int isAtIndex(QString s);       /// Find filter index from filter string

};

#endif // FILTERMANAGER_H
