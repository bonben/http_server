/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the example classes of the Qt Toolkit.
**
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file.  Please review the following information
** to ensure GNU General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.  In addition, as a special
** exception, Nokia gives you certain additional rights. These rights
** are described in the Nokia Qt GPL Exception version 1.3, included in
** the file GPL_EXCEPTION.txt in this package.
**
** Qt for Windows(R) Licensees
** As a special exception, Nokia, as the sole copyright holder for Qt
** Designer, grants users of the Qt/Eclipse Integration plug-in the
** right for the Qt/Eclipse Integration to link to functionality
** provided by Qt Designer and its related libraries.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
****************************************************************************/

#include "MySocketServer.h"
#include "MySocketClient.h"

#include <stdlib.h>

//! [0]
MySocketServer::MySocketServer(QObject *parent)
    : QTcpServer(parent)
{
    QFile* file = new QFile( "./public_html/ressources/404-error.jpg" );
    statistiques = new  Statistiques;
    if (!file->open(QIODevice::ReadOnly))
    {
            delete file;
            return;
    }

    QByteArray* temp = new QByteArray("");
    *temp = "HTTP/1.1 404 Not Found\r\nConnection: close\r\nContent-Type: image/jpeg\r\n\n";
    *temp += file->readAll();
    // avec QByteArray temp("HTTP...")
    // temp a été redimensionné automatiquement
    MyHash.insert("erreur404", temp);
    MyHashSize = temp->size();
}

MySocketServer::~MySocketServer()
{
    QHash<QString, QByteArray*>::const_iterator i = MyHash.constBegin();//On vide le cache
    while (i!=MyHash.constEnd())
    {
        delete i.value();
        ++i;
    }
    delete statistiques;
}

void MySocketServer::clearcache()
{
    QHash<QString, QByteArray*>::const_iterator i;
    while (MyHash.count()>1)
    {
        i=MyHash.constBegin()+1;
        MyHashSize-=i.value()->size();//on soustrait la taille du byte array supprimé
        MyHashKeys.removeAll(i.key());//on enleve le key de la liste
        delete i.value();//On desalloue la mémoire
        MyHash.remove(i.key());
    }

}
void MySocketServer::slotclearcache()
{
    this->clearcache();
}


void MySocketServer::incomingConnection(int socketDescriptor)
{
    cout << "(II) Launching the Network monitor process" << endl;

    // CREATION DE L'OBJET EN CHARGE DES REPONSES RESEAU
    MySocketClient *thread = new MySocketClient(socketDescriptor,&MyHash,statistiques,&MyHashSize, &MyHashKeys, this);

    // ON INDIQUE QUE LORSQU'UN CLIENT SE CONNECTE ON DELEGE LA REPONSE
    // AU PROCESSUS DEFINI CI DESSUS...
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    // ON DEMARRE LE PROCESSUS DE REPONSE POUR LE METTRE EN ATTENTE !
    thread->start();

    // PUIS ON REND LA MAIN EN ATTENTE D'UN CLIENT
    cout << "(II) Network monitor process launch : OK" << endl;
}
int MySocketServer::getCacheSize()
{
    return MyHashSize;
}
QString MySocketServer::getHashKeys()
{
    return MyHashKeys.join("<br />");
}
