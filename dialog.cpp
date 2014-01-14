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

#include <QtGui>
#include <QtNetwork>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <stdlib.h>
#include <iostream>
#include "dialog.h"
using namespace std;
Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
{
    statusLabel = new QLabel;
    cachelabel = new QLabel(tr("Fichiers du cache (commence par le Last Recently Used)"));
    quitButton = new QPushButton(tr("Quit"));
    clearcacheButton = new QPushButton(trUtf8("Libérer le cache"));
    stats = new QGroupBox(trUtf8("Statistiques"));
    nbrequetes = new QLabel(trUtf8("Nombre de requêtes :"));
    nbtraitees = new QLabel(trUtf8("Nombre de requêtes traitées :"));
    nboctets = new QLabel(tr("Nombre d'octets transmis :"));
    nberreurs= new QLabel(tr("Nombre d'erreurs 404 :"));

    CacheFiles = new QTextEdit;
    occupationcache = new QProgressBar();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(maj()));
    timer->start(100);

    quitButton->setAutoDefault(false);

    if (!server.listen(QHostAddress::Any, 8081)) {
        QMessageBox::critical(this, tr("Small WEB Server"),
                              tr("Unable to start the server: %1.")
                              .arg(server.errorString()));
        close();
        return;
    }

    statusLabel->setText(tr("The server is running on port %1.\n"
                            "Use internet navigator now.")
                         .arg(server.serverPort()));

    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(clearcacheButton, SIGNAL(clicked()), &server, SLOT(slotclearcache()));






    QVBoxLayout *statslayout = new QVBoxLayout;
    statslayout->addWidget(nbrequetes);
    statslayout->addWidget(nbtraitees);
    statslayout->addWidget(nboctets);
    statslayout->addWidget(nberreurs);
    statslayout->addWidget(occupationcache);

    stats->setLayout(statslayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(clearcacheButton);
    buttonLayout->addWidget(quitButton);
    buttonLayout->addStretch(1);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(statusLabel);
    leftLayout->addWidget(stats);
    leftLayout->addLayout(buttonLayout);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(cachelabel);
    rightLayout->addWidget(CacheFiles);


    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);

    setLayout(mainLayout);

    setWindowTitle(tr("Small Web Server"));

}

void Dialog::maj()
{

    nbrequetes->setText(trUtf8("Nombre de requêtes : ") + QString::number(server.statistiques->getNbRequetes()));
    nbtraitees->setText(trUtf8("Nombre de requêtes traitées : ") + QString::number(server.statistiques->getNbTraitees()));
    nboctets->setText(trUtf8("Nombre d'octets transmis : ") + QString::number(server.statistiques->getNbOctets()));
    nberreurs->setText(trUtf8("Nombre d'erreurs 404 : ") + QString::number(server.statistiques->getNbErreurs404()));
    occupationcache->setMaximum(CACHE_SIZE);
    occupationcache->setValue(server.getCacheSize());
    CacheFiles->setHtml(server.getHashKeys());

}
