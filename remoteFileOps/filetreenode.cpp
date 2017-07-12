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

#include "filetreenode.h"

#include "../AgaveClientInterface/filemetadata.h"
#include "../AgaveClientInterface/remotedatainterface.h"

FileTreeNode::FileTreeNode(FileMetaData contents, FileTreeNode * parent):QObject((QObject *)parent)
{
    fileData = new FileMetaData(contents);

    if (parent == NULL)
    {
        rootNode = true;
    }
    else
    {
        rootNode = false;
        parent->childList.append(this);
    }
}

FileTreeNode::FileTreeNode(FileTreeNode * parent):QObject((QObject *)parent)
{
    fileData = new FileMetaData();
    if (parent == NULL)
    {
        rootNode = true;

        fileData->setFullFilePath("/");
        fileData->setType(FileType::DIR);
    }
    else
    {
        rootNode = false;
        parent->childList.append(this);

        //Create loading placeholder
        QString fullPath = parent->fileData->getFullPath();
        fullPath.append("/Loading");

        fileData->setFullFilePath(fullPath);
        fileData->setType(FileType::UNLOADED);
    }
}

FileTreeNode::~FileTreeNode()
{
    if (fileData != NULL)
    {
        delete fileData;
    }
    if (this->parent() != NULL)
    {
        FileTreeNode * parentNode = (FileTreeNode *)this->parent();
        if (parentNode->childList.contains(this))
        {
            parentNode->childList.removeAll(this);
        }
    }
    while (this->childList.size() > 0)
    {
        FileTreeNode * toDelete = this->childList.takeLast();
        toDelete->deleteLater();
    }
    if (this->fileDataBuffer != NULL)
    {
        delete this->fileDataBuffer;
    }
}

void FileTreeNode::insertOrUpdateFile(QList<FileMetaData> newDataList)
{
    if (rootNode == false) return;
    for (auto itr = newDataList.cbegin(); itr != newDataList.cend(); itr++)
    {
        insertOrUpdateFile(*itr);
    }
}

void FileTreeNode::insertOrUpdateFile(FileMetaData newData)
{
    if (rootNode == false) return;
    //TODO
}

FileMetaData FileTreeNode::getFileData()
{
    return *fileData;
}

QByteArray * FileTreeNode::getFileBuffer()
{
    return fileDataBuffer;
}

void FileTreeNode::setFileBuffer(QByteArray * newFileBuffer)
{
    fileDataBuffer = newFileBuffer;
}

FileTreeNode * FileTreeNode::getNodeWithName(QString filename, bool unrestricted)
{
    return pathSearchHelper(filename,false,unrestricted);
}

FileTreeNode * FileTreeNode::getClosestNodeWithName(QString filename, bool unrestricted)
{
    return pathSearchHelper(filename,true,unrestricted);
}

FileTreeNode * FileTreeNode::getParentNode()
{
    if (rootNode == false) return NULL;
    return (FileTreeNode *)this->parent();
}

bool FileTreeNode::childIsUnloaded()
{
    for (auto itr = childList.cbegin(); itr != childList.cend(); itr++)
    {
        if ((*itr)->getFileData().getFileType() == FileType::UNLOADED)
        {
            return true;
        }
    }
    return false;
}

void FileTreeNode::clearAllChildren()
{
    while (childList.size() > 0)
    {
        FileTreeNode *toDestroy = childList.takeLast();
        toDestroy->deleteLater();
    }
}

FileTreeNode * FileTreeNode::pathSearchHelper(QString filename, bool stopEarly, bool unrestricted)
{
    if (rootNode == false) return NULL;

    QStringList filePathParts = FileMetaData::getPathNameList(filename);
    FileTreeNode * searchNode = this;

    for (auto itr = filePathParts.cbegin(); itr != filePathParts.cend(); itr++)
    {
        FileTreeNode * nextNode = searchNode->getChildNodeWithName(*itr,unrestricted);
        if (nextNode == NULL)
        {
            if (stopEarly == true)
            {
                return searchNode;
            }
            return NULL;
        }
        searchNode = nextNode;
    }

    return searchNode;
}

FileTreeNode * FileTreeNode::getChildNodeWithName(QString filename, bool unrestricted)
{
    for (auto itr = this->childList.begin(); itr != this->childList.end(); itr++)
    {
        FileMetaData toSearch = (*itr)->getFileData();
        if ((unrestricted) || (toSearch.getFileType() == FileType::DIR) || (toSearch.getFileType() == FileType::FILE))
        {
            if (toSearch.getFileName() == filename)
            {
                return (*itr);
            }
        }
    }
    return NULL;
}
