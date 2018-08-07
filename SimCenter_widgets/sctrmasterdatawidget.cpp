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

#include "cwe_globals.h"

#include "SimCenter_widgets/sctrmasterdatawidget.h"
#include "../AgaveExplorer/remoteFileOps/filenoderef.h"
#include "CFDanalysis/CFDanalysisType.h"

SCtrMasterDataWidget::SCtrMasterDataWidget(QWidget *parent) :
    QFrame(parent)
{
    m_ViewState = SimCenterViewState::hidden;

    m_obj.displayName = "";
    m_obj.type = SimCenterDataType::unknown;
    m_obj.defaultValue = "error in configuration file";
    m_obj.unit = "";
    m_obj.precision = "";
    m_obj.sign = "";
    m_obj.options.clear();
}

SCtrMasterDataWidget::~SCtrMasterDataWidget(){}

SimCenterViewState SCtrMasterDataWidget::viewState()
{
    return m_ViewState;
}

void SCtrMasterDataWidget::setViewState(SimCenterViewState state)
{
    m_ViewState = state;
    switch (m_ViewState) {
    case SimCenterViewState::visible:
        setComponetsEnabled(false);
        this->show();
        break;
    case SimCenterViewState::hidden:
        setComponetsEnabled(false);
        this->hide();
        break;
    case SimCenterViewState::editable:
        setComponetsEnabled(true);
        this->show();
        break;
    }
}

void SCtrMasterDataWidget::setDataType(VARIABLE_TYPE & newTypeData)
{
    if (m_obj.type != SimCenterDataType::unknown)
    {
        cwe_globals::displayFatalPopup("Parameter widget initialized twice.", "Internal Error");
        return;
    }

    m_obj = newTypeData;
    initUI();

    /* set default */
    setValue(m_obj.defaultValue);

    if (savedValue != shownValue())
    {
         qCDebug(agaveAppLayer, "ERROR: Invalid defaults specified for parameter config");
    }
}

VARIABLE_TYPE SCtrMasterDataWidget::getTypeInfo()
{
    return m_obj;
}

void SCtrMasterDataWidget::setValue(QString newValue)
{
    savedValue = newValue;
    doingManualUpdate = true;
    setShownValue(savedValue);
    doingManualUpdate = false;
}

QString SCtrMasterDataWidget::savableValue()
{
    return shownValue();
}

void SCtrMasterDataWidget::saveShownValue()
{
    if (shownValueIsValid())
    {
        setValue(shownValue());
    }
}

void SCtrMasterDataWidget::revertShownValue()
{
    setShownValue(savedValue);
}

bool SCtrMasterDataWidget::isValueChanged()
{
    return (savedValue != shownValue());
}

bool SCtrMasterDataWidget::hasValidNewValue()
{
    return (isValueChanged() && shownValueIsValid());
}

void SCtrMasterDataWidget::changeMadeToUnderlyingDataWidget()
{
    if (doingManualUpdate) return;
    emit valueEdited();
}

bool SCtrMasterDataWidget::shownValueIsValid()
{
    return true;
}
