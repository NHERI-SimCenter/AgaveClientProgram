/*********************************************************************************
**
** Copyright (c) 2017 The University of Notre Dame
** Copyright (c) 2017 The Regents of the University of California
**
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice, this
** list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright notice, this
** list of conditions and the following disclaimer in the documentation and/or other
** materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its contributors may
** be used to endorse or promote products derived from this software without specific
** prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
** EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
** SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
** TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
** BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
** IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
**
***********************************************************************************/

// Contributors:
// Written by Peter Sempolinski, for the Natural Hazard Modeling Laboratory, director: Ahsan Kareem, at Notre Dame

#ifndef PANELWINDOW_H
#define PANELWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStackedWidget>

class FileTreeModelReader;
class TaskPanelEntry;
class RemoteDataInterface;
class VWTinterfaceDriver;

namespace Ui {
class PanelWindow;
}

class PanelWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PanelWindow(VWTinterfaceDriver *newDriver, QWidget *parent = 0);
    ~PanelWindow();

    void setupTaskList();

private slots:
    void taskEntryClicked(QModelIndex clickedItem);
    void menuExit();
    void menuCopyInfo();

private:
    Ui::PanelWindow *ui;
    VWTinterfaceDriver * myDriver;
    QTreeView * taskTreeView;
    QStackedWidget *sharedWidget;
    RemoteDataInterface * dataLink;
    FileTreeModelReader * fileTreeModel;

    QVector<TaskPanelEntry *> taskPanelList;
    QStandardItemModel taskListModel;
    const QStringList taskHeaderList = {"Task List:","idNum"};

    void registerTaskPanel(TaskPanelEntry * newPanel);
    void takePanelOwnership(TaskPanelEntry * newOwner);
};

#endif // PANELWINDOW_H
