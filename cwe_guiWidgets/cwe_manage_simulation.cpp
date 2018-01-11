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

#include "cwe_manage_simulation.h"
#include "ui_cwe_manage_simulation.h"

#include "../AgaveExplorer/remoteFileOps/filetreenode.h"

#include "CFDanalysis/CFDanalysisType.h"
#include "CFDanalysis/CFDcaseInstance.h"

#include "mainWindow/cwe_mainwindow.h"

#include "vwtinterfacedriver.h"

CWE_manage_simulation::CWE_manage_simulation(QWidget *parent) :
    CWE_Super(parent),
    ui(new Ui::CWE_manage_simulation)
{
    ui->setupUi(this);

    QObject::connect(ui->treeView, SIGNAL(newFileSelected(FileTreeNode*)),
                             this, SLOT(newFileSelected(FileTreeNode*)));

    clearSelectView();

    ui->stageListView->setModel(&stageListModel);
}

CWE_manage_simulation::~CWE_manage_simulation()
{
    delete ui;
}

void CWE_manage_simulation::linkDriver(VWTinterfaceDriver * theDriver)
{
    CWE_Super::linkDriver(theDriver);
    if (!theDriver->inOfflineMode())
    {
        ui->treeView->setFileOperator(theDriver->getFileHandler());
    }
    QObject::connect(myDriver, SIGNAL(haveNewCase()),
                     this, SLOT(newCaseGiven()));
}

void CWE_manage_simulation::newFileSelected(FileTreeNode * newFile)
{
    if (newFile == NULL)
    {
        myDriver->setCurrentCase(NULL);
        return;
    }

    myDriver->setCurrentCase(new CFDcaseInstance(newFile, myDriver));
}

void CWE_manage_simulation::newCaseGiven()
{
    CFDcaseInstance * newCase = myDriver->getCurrentCase();

    ui->label_caseStatus->setCurrentCase(newCase);    

    ui->treeView->setEnabled(true);
    clearSelectView();

    ui->pb_viewParameters->setEnabled(false);
    ui->pb_viewResults->setEnabled(false);

    if (newCase != NULL)
    {
        ui->label_caseName->setText(newCase->getCaseName());

        QObject::connect(newCase, SIGNAL(haveNewState(CaseState)),
                         this, SLOT(newCaseState(CaseState)));
        newCaseState(newCase->getCaseState());
    }
    else
    {
        ui->treeView->clearSelection();
    }
}

void CWE_manage_simulation::newCaseState(CaseState newState)
{
    if (newState == CaseState::OP_INVOKE)
    {
        //TODO: This should be visible but unclickable
        ui->treeView->setEnabled(false);

        showSelectView();
        ui->pb_viewParameters->setEnabled(false);
        ui->pb_viewResults->setEnabled(false);
        return;
    }

    ui->treeView->setEnabled(true);

    if ((newState == CaseState::DEFUNCT) ||
            (newState == CaseState::ERROR) ||
            (newState == CaseState::INVALID) ||
            (newState == CaseState::LOADING))
    {
        if (newState == CaseState::DEFUNCT)
        {
            ui->treeView->clearSelection();
        }
        if (newState != CaseState::LOADING)
        {
            clearSelectView();
            ui->pb_viewParameters->setEnabled(false);
            ui->pb_viewResults->setEnabled(false);
        }
        return;
    }

    if ((newState == CaseState::JOB_RUN) || (newState == CaseState::READY))
    {
        ui->pb_viewParameters->setEnabled(true);
        ui->pb_viewResults->setEnabled(true);

        CFDcaseInstance * theCase = myDriver->getCurrentCase();

        CFDanalysisType * theType = theCase->getMyType();
        if (theType == NULL)
        {
            myDriver->fatalInterfaceError("Type/stage mismatch for case.");
            return;
        }
        ui->label_CaseTypeIcon->setPixmap(theType->getIcon()->pixmap(150,100));

        QMap<QString, StageState> stages = theCase->getStageStates();
        stageListModel.clear();

        for (auto itr = stages.cbegin(); itr != stages.cend(); itr++)
        {
            QString aLine = theCase->getMyType()->translateStageId(itr.key());
            aLine = aLine.append(" - ");
            aLine = aLine.append(getStateText(*itr));
            stageListModel.appendRow(new QStandardItem(aLine));
        }

        showSelectView();
    }
}

void CWE_manage_simulation::on_pb_viewParameters_clicked()
{
    // switch main window to parameters tab
    myDriver->getMainWindow()->switchToParameterTab();
}

void CWE_manage_simulation::on_pb_viewResults_clicked()
{
    // switch main window to results tab
    myDriver->getMainWindow()->switchToResultsTab();
}

void CWE_manage_simulation::clearSelectView()
{
    ui->label_caseTypeTag->setVisible(false);
    ui->label_CaseTypeIcon->setVisible(false);
    ui->label_stageListTag->setVisible(false);
    ui->stageListView->setVisible(false);
}

void CWE_manage_simulation::showSelectView()
{
    ui->label_caseTypeTag->setVisible(true);
    ui->label_CaseTypeIcon->setVisible(true);
    ui->label_stageListTag->setVisible(true);
    ui->stageListView->setVisible(true);
}

QString CWE_manage_simulation::getStateText(StageState theState)
{
    if (theState == StageState::ERROR)
        return "*** ERROR ***";
    if (theState == StageState::FINISHED)
        return "Finished";
    if (theState == StageState::LOADING)
        return "Loading ...";
    if (theState == StageState::RUNNING)
        return "Running ...";
    if (theState == StageState::UNRUN)
        return "Not Yet Run";
    return "*** TOTAL ERROR ***";
}
