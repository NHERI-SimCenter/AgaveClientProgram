/*********************************************************************************
**
** Copyright (c) 2018 The University of Notre Dame
** Copyright (c) 2018 The Regents of the University of California
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

#ifndef CWE_PARAMETERS_H
#define CWE_PARAMETERS_H

#include "cwe_super.h"

#include <QVector>
#include <QLabel>
#include <QJsonObject>

class CWE_MainWindow;

class CWE_StageStatusTab;
class CWE_GroupTab;
class CWE_ParamTab;
class SCtrMasterDataWidget;

struct VARIABLE_TYPE;

enum class CaseState;
enum class StageState;

enum class SimCenterViewState;

#define SimCenterButtonMode  std::uint32_t

/*
 * SimCenterButtonMode_NONE      0000 0000 0000 0000
 * SimCenterButtonMode_RUN       0000 0000 0000 0001
 * SimCenterButtonMode_CANCEL    0000 0000 0000 0010
 * SimCenterButtonMode_RESET     0000 0000 0000 0100
 * SimCenterButtonMode_RESULTS   0000 0000 0000 1000
 */

#define SimCenterButtonMode_NONE      0x0000u
#define SimCenterButtonMode_RUN       0x0001u
#define SimCenterButtonMode_CANCEL    0x0002u
#define SimCenterButtonMode_RESET     0x0004u
#define SimCenterButtonMode_RESULTS   0x0008u

namespace Ui {
class CWE_Parameters;
}

class CWE_Parameters : public CWE_Super
{
    Q_OBJECT

public:
    explicit CWE_Parameters(QWidget *parent = nullptr);
    ~CWE_Parameters();

    virtual void linkMainWindow(CWE_MainWindow *theMainWin);
    virtual bool allowClickAway();

private slots:
    void save_all_button_clicked();
    void run_button_clicked();
    void cancel_button_clicked();
    void results_button_clicked();
    void rollback_button_clicked();

    void newCaseGiven();
    void newCaseState(CaseState newState);

    void stageSelected(CWE_ParamTab * chosenTab);
    void groupSelected(CWE_ParamTab * chosenTab);

    void paramWidgetChanged();

private:
    bool paramsChanged();
    bool panelSwitchPermitted();

    void resetButtonAndView();
    void setButtonState(SimCenterButtonMode newMode);
    void setButtonState(StageState newMode);
    void setViewState(SimCenterViewState newState);
    void setViewState(StageState newMode);

    bool widgetIsHiddenByCondition(SCtrMasterDataWidget * aWidget);
    bool checkVarCondition(QString conditionToCheck);
    bool lexifyConditionString(QString conditionToCheck, QString & leftSide, QString & rightSide, QString condition);
    QString getVarValByName(QString varName);

    void setHeaderLabels();

    void createStageTabs();
    void createGroupTabs();
    void createParamWidgets();
    void addVariable(QString varName, VARIABLE_TYPE &theVariable, QString *nonDefaultValue = nullptr);

    void clearStageTabs();
    void clearGroupTabs();
    void clearParamScreen();

    static QString getStateText(StageState theState);

    Ui::CWE_Parameters *ui;

    QVector<CWE_StageStatusTab *> stageTabList;
    QVector<CWE_GroupTab *> groupTabList;
    QVector<SCtrMasterDataWidget *> paramWidgetList;

    CWE_StageStatusTab * selectedStage = nullptr;
    CWE_GroupTab * selectedGroup = nullptr;

    QLabel * loadingLabel = nullptr;
};

#endif // CWE_PARAMETERS_H
