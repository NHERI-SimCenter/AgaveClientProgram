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

#include "sctrchoicedatawidget.h"

#include "cwe_globals.h"

SCtrChoiceDataWidget::SCtrChoiceDataWidget(QWidget *parent):
    SCtrMasterDataWidget(parent)
{

}

SCtrChoiceDataWidget::~SCtrChoiceDataWidget()
{
    if (theComboBox != NULL) theComboBox->deleteLater();
    if (label_unit != NULL) label_unit->deleteLater();
    if (label_varName != NULL) label_varName->deleteLater();
    if (theModel != NULL) theModel->deleteLater();
}

QString SCtrChoiceDataWidget::shownValue()
{
    /*
     * the model contains two columns:
     *   col 0: the variable name
     *   col 1: the human readable variable description
     *
     * theComboBox displays col 1
     * the driver uses the associated value from col 0
     * thus, the return value is the associate QString from col 0
     */

    QStandardItemModel *model = (QStandardItemModel *)theComboBox->model();
    QStandardItem *item = model->item(theComboBox->currentIndex(), 0);

    return item->text();
}

void SCtrChoiceDataWidget::initUI()
{
    QBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);

    label_unit = new QLabel(getTypeInfo().unit, this);
    label_varName = new QLabel(getTypeInfo().displayName, this);

    theModel = new QStandardItemModel(this);

    for (auto itr = getTypeInfo().options.cbegin(); itr != getTypeInfo().options.cend(); itr++)
    {
        QList<QStandardItem *> newRow;
        newRow.append(new QStandardItem(itr.key()));
        newRow.append(new QStandardItem(*itr));

        theModel->appendRow(newRow);
    }

    theComboBox = new QComboBox(this);
    theComboBox->setModel(theModel);
    theComboBox->setModelColumn(1);

    layout->addWidget(label_varName, 3);
    layout->addWidget(theComboBox, 4);
    layout->addWidget(label_unit, 1);

    this->setLayout(layout);

    QObject::connect(theComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(changeMadeToUnderlyingDataWidget()));
}

void SCtrChoiceDataWidget::setComponetsEnabled(bool newSetting)
{
    theComboBox->setEnabled(newSetting);
    label_unit->setEnabled(newSetting);
}

void SCtrChoiceDataWidget::setShownValue(QString newValue)
{
    QStandardItemModel *theModel = (QStandardItemModel *)theComboBox->model();
    QList<QStandardItem *> itemList = theModel->findItems(newValue, Qt::MatchExactly, 0);

    if (itemList.isEmpty())
    {
        QString varName = getTypeInfo().displayName;
        qCDebug(agaveAppLayer, "Variable %s of unknown selection option \'%s\'.\nVariable ignored.", qPrintable(varName), qPrintable(newValue));
        return;
    }

    /* the following loop should never have more than one item in the itemList ...  */
    QModelIndex idx = theModel->indexFromItem(itemList.at(0));
    theComboBox->setCurrentIndex(idx.row());
}
