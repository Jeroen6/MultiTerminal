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
#include "viewmanager.h"
#include "view.h"

/// Constructor
ViewManager::ViewManager(int setMaxViews, QWidget *parent) :
    QWidget(parent)
{
    maxViews = setMaxViews;
    // Allocate viewID's from 1 to maxview
    for(int i=0; i<maxViews; i++){
        viewIDs.push_back(i);
        viewIdTranslator.push_back(-1);
    }
}

/// Decontstructor
ViewManager::~ViewManager()
{
    while(viewList.count()){
        delete viewList.first();
        viewList.removeFirst();
    }
}

/// Create a new view
int ViewManager::spawn(QString filter){
    /* Acquire a viewID
     * Create a view on stack
     * Create viewID translator to stack index
     * return viewid
     */
    int tViewID = newViewID();
    if(tViewID != -1){
        // Create view
        View *view = new View(tViewID,filter);
        //connect(view,SIGNAL(closed(View*)),this,SLOT(someViewWasClosed(View *source)));
        connect(view,SIGNAL(closed(View*)),this,SLOT(someViewWasClosed(View*)));
        viewList.push_back(view);
        viewIdTranslator[tViewID] = viewList.count()-1;

        // Set view parameters
        QString title = filter + " - " + view->windowTitle();
        view->setWindowTitle(title);

        // Return
        return tViewID;
    }else{
        return -1; // Failure, full?
    }
}

/// Kill a view by viewID
int ViewManager::kill(int viewID){
    /* Validate viewID
     * Acquire view from stack using viedIdTranslator
     * delete view
     * clean viewmanager
     */
    if(viewIdTranslator[viewID] != -1){
        View *view = viewList[viewIdTranslator[viewID]];
        disconnect(view,SIGNAL(closed(View*)),this,SLOT(someViewWasClosed(View*)));
        viewList.removeAt(viewIdTranslator[viewID]);
        recycleViewID(viewID);
        delete view;
        return viewID;
    }else{
        return -1; // Failure, no such view?
    }
}

/// Kill a view by filterstring
int ViewManager::kill(QString filter){
    return kill(getID(filter));
}

/// Allocate a new ViewID
int ViewManager::newViewID(void){
    if(viewIDs.count()){
        int tViewID = viewIDs.at(0);
        viewIDs.pop_front();
        return tViewID;
    }else{
        return -1; // Full
    }
}

/// Recycle a viewID
void ViewManager::recycleViewID(int oldViewID){
    viewIdTranslator[oldViewID] = 0;
    viewIDs.push_back(oldViewID);
    // Rebuild viewIdTranslator
    for(int i=0; i<viewIdTranslator.count(); i++){
        viewIdTranslator[i] = -1;
    }
    for(int i=0; i<viewList.count(); i++){
        viewIdTranslator[viewList.at(i)->getViewID()] = i;
    }
}

/// Show a view by viewID
int ViewManager::show(int viewID){
    if(viewID >= 0 && viewID < viewIdTranslator.count()){
        if(viewIdTranslator[viewID] >= 0){
            viewList[viewIdTranslator[viewID]]->show();
            return viewID;
        }
    }
    return -1;
}

/// Show a view by filterstring
int ViewManager::show(QString filter){
    return show(getID(filter));
}

/// Hide a view by viewID
int ViewManager::hide(int viewID){
    if(viewID >= 0 && viewID < viewIdTranslator.count()){
        if(viewIdTranslator[viewID] >= 0){
            viewList[viewIdTranslator[viewID]]->hide();
            return viewID;
        }
    }
    return -1;
}

/// Hide a view by filterstring
int ViewManager::hide(QString filter){
    return hide(getID(filter));
}

int ViewManager::getID(QString filter){
    if(viewList.size() > 0){
        for(int i=0; i<viewIdTranslator.count(); i++){
            int viewID = viewIdTranslator[i];
            if(viewID >= 0){
                QString cmp = viewList[viewID]->getFilter();
                if(cmp == filter)
                    return i;
            }
        }
    }
    return -1;
}

/// Write to a view by filterstring
int ViewManager::write(QString filter, QString data){
    int viewID = getID(filter);
    if(viewID >= 0 && viewID < viewIdTranslator.count()){
        if(viewIdTranslator[viewID] >= 0){
            viewList[viewIdTranslator[viewID]]->write(data);
            return viewID;
        }
    }
    return -1;
}

/// Wipe a view by filterstring
int ViewManager::wipe(QString filter)
{
    int viewID = getID(filter);
    if(viewID >= 0 && viewID < viewIdTranslator.count()){
        if(viewIdTranslator[viewID] >= 0){
            viewList[viewIdTranslator[viewID]]->on_buttonWipe_clicked();
            return viewID;
        }
    }
    return -1;
}

/// Wipe all view
void ViewManager::wipeAll()
{
    for(int i=0; i<viewList.count(); i++)
        viewList.at(i)->on_buttonWipe_clicked();
}

/// Handle a view closed by the user
void ViewManager::someViewWasClosed(View *source){
    viewVisibilityChanged(source->getFilter());
}

/// return maxviews
int ViewManager::getMaxViews() const
{
    return maxViews;
}


