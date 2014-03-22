#include "filtermanager.h"

#include <QTextStream>
#include <QString>

/// @brief add filter
int FilterManager::add(QString s){
    // Verify unicity
    if(isAtIndex(s) == -1){
        // Create
        Filter *f = new Filter;
        filters.push_back(f);
        f->mask = s;
        return 0;
    }
    // Filter already exist
    return -1;
}

/// @brief remove filter
int FilterManager::remove(QString s){
    // Verify availability
    int i = isAtIndex(s);
    if(i > 0){
        // Destroy
        Filter *f = filters[i];
        filters.removeAt(i);
        delete f;
        return 0;
    }
    // Filter not available
    return -1;
}

/// @brief Write to filter engine
int FilterManager::write(QString data){
    unprocessed.push_back(data);
    filterEngine();
    return data.size();
}

/// @brief Write directly to filter output bypassing filtering
int FilterManager::write(QString filter, QString data){
    if(isAtIndex(data) > 0){
        filters[isAtIndex(filter)]->buffer.push_back(data);
        return data.size();
    }else{
        return -1;
    }
}

/// @brief Read filter output
QString FilterManager::readLine(QString filter){
    int i = isAtIndex(filter);
    if(i >= 0){
        QString data;
        if(! filters[i]->buffer.isEmpty()){
            data = filters[i]->buffer.first();
            filters[i]->buffer.removeFirst();
            return data;
        }
        return NULL;
    }else{
        return NULL;
    }
}

/// @brief Return index
int FilterManager::isAtIndex(QString s){
    int c = filters.count();
    for(int i=0; i<c; i++){
        if(filters[i]->mask == s){
            return i;
        }
    }
    return -1;
}

/// @brief Write to filter engine
void FilterManager::filterEngine(){
    // Loop all data
    while(unprocessed.size()){
        QString input = unprocessed.first();
        unprocessed.removeFirst();
        // Loop all filters
        int c = filters.count();
        for(int i=0; i<c; i++){
            // Compare
            if(input.indexOf(filters[i]->mask,0) == 0){
                filters[i]->buffer.push_back(input);
                break;
            }
        }
    }
}


