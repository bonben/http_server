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

#include "MySocketClient.h"
#include "MySocketServer.h"

#include <QtNetwork>
using namespace std;

MySocketClient::MySocketClient(int socketDescriptor, QHash <QString, QByteArray*>* MyHash,  Statistiques* statistiques, int* MyHashSize, QStringList* MyHashKeys, QObject *parent)
    : QThread(parent), socketDescriptor(socketDescriptor)
{
    this->statistiques=statistiques;
    this->MyHash=MyHash;
    this->MyHashKeys=MyHashKeys;
    this->MyHashSize=MyHashSize;
}

inline string removeEndLine(string s){
        while( s.at(s.length()-1) == '\r' || s.at(s.length()-1) == '\n' )
        {
                s = s.substr(0, s.length()-1);
        }
        return s;
}



void MySocketClient::run()
{
    cout << "Starting MySocketClient::run()" << endl;
    QTcpSocket tcpSocket;

    // ON RECUPERE LE LIEN DE COMMUNICATION AVEC LE CLIENT ET ON QUITTE EN CAS
    // DE PROBLEME...
    if (!tcpSocket.setSocketDescriptor(socketDescriptor)) {
        emit error(tcpSocket.error());
        return;
    }

    // SI LE CLIENT N'A PAS EU LE TEMPS DE NOUS TRANSMETTRE SA REQUETE,
    // ON SE MET EN ATTENTE PENDANT 1s
    while (tcpSocket.bytesAvailable() < (int)sizeof(quint16)) {
        if (!tcpSocket.waitForReadyRead( 1000 )) {
                cout << "(EE) Erreur de time out !" << endl;
                return;
        }
    }

    // LA PREMIERE REQUETE CORRESPOND AU GET NORMALEMENT
    char tampon[65536];

    // ON RECUPERE LA REQUETE ET SA TAILLE
    int lineLength = tcpSocket.readLine(tampon, 65536);

    // ON TRANSFORME LA REQUETE SOUS FORME DE STRING
    string ligne( tampon );
    ligne = removeEndLine( ligne );

    // ON AFFICHE LA COMMANDE A L'ECRAN...
    cout << "COMMANDE : =>" << ligne << "<=" << endl;

   int pos1 = ligne.find(" ");
   string cmde = ligne.substr(0, pos1);
   ligne = ligne.substr(pos1+1, ligne.length()-pos1);

   cout << "1. : " << cmde  << endl;
   cout << "2. : " << ligne << endl;

   int pos2 = ligne.find(" ");
   string file = ligne.substr(0, pos2);
   ligne = ligne.substr(pos2+1, ligne.length()-pos2);

   cout << "3. : " << file  << endl;
   cout << "4. : '" << ligne << "'" << endl;

        while( tcpSocket.bytesAvailable() > 0 ){
        lineLength = tcpSocket.readLine(tampon, 65536);
        if (lineLength != -1 &&  lineLength != 0) {
                //cout << "C" << lineLength << " :: " << tampon;
        }else if(lineLength != -1 ){
                cout << "Nothing for the moment !" << endl;
        }
    }

    QString str = tr("./public_html") + tr(file.c_str()); //Adresse du fichier ou dossier (./public_html//essai.html)
    statistiques->AddPeer(tcpSocket.peerAddress().toString());
    statistiques->AddRequete(str);
    QByteArray temp("");
    QFile f( str );
    QDir  d( str );
    bool found;

    if( f.exists() == true )
    {
        statistiques->AddFile(f.fileName());
    }


    found = str.contains("statistiques.html", Qt::CaseInsensitive) ||str.contains("???", Qt::CaseInsensitive) ;
    if(found==true)
    {
       temp = statistiques->GenerateHtml();

    }

    else
    {
        if(!MyHash->contains( str ))    /* si le hash ne contient pas ce fichier*/
        {                               /*alors on ajoute la page dans Hash*/

            cout << " - Chemin du fichier : " << str.toStdString() << endl;
            cout << " - isFile :          : " << f.exists() << endl;
            cout << " - isDirectory       : " << d.exists() << endl;

            if( f.exists() == false &&  d.exists() == false )
            {
                str = tr("erreur404");
            }
            else if( d.exists() == true )//LE CHAMP SAISI EST UN REPERTOIRE
            {
                QByteArray* NewByteArray = new QByteArray;

                *NewByteArray = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\n\n";
                for(int i = 0 ; i < d.entryList().size() ; i++)
                {
                    *NewByteArray+=QByteArray("<p>") + QByteArray("<a href=\"")+QString::fromStdString(file);
                    if(QString::compare(QString::fromStdString(file),QString("/")))//Dans le cas d'un sous dossier il faut rajouter un / dans le path
                        *NewByteArray+=QByteArray("/");
                    *NewByteArray+=d.entryList()[i]+QByteArray("\">")+d.entryList()[i] +QByteArray("</a></p>");
                }
                MyHash->insert(str, NewByteArray);
                *MyHashSize+=NewByteArray->size();
                MyHashKeys->append(str);
            }
            else if( f.exists() == true )/* On ajoute le fichier dans le Hash en ByteArray*/
            {

                QFile* file = new QFile( str );

                 if (!file->open(QIODevice::ReadOnly))
                 {
                         delete file;
                         return;
                 }

                 /*concatener des types QByteArray? avec le +
                 utiliser l'en tête ci-dessous
                 HTTP/1.1 200 OK
                 Connection: close
                 Content-Type: text/html
                 construction de cette en-tête HTTP, il est nécessaire de la faire suivre par un saut de ligne
                 avant la partie html*/

                 QByteArray* NewByteArray = new QByteArray;

                 found = str.contains(".jpg", Qt::CaseInsensitive);
                 if (found==true)
                 {
                     *NewByteArray = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: image/jpeg\r\n\n";
                     // creation d'un QbyteArray indiquant une image au format jpeg
                 }

                 found = str.contains(".html", Qt::CaseInsensitive);
                 if (found==true)
                 {
                     *NewByteArray = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\n\n";
                     // creation d'un QbyteArray indiquant un texte au format html
                 }

                 found = str.contains(".pdf", Qt::CaseInsensitive);
                 if (found==true)
                 {
                     *NewByteArray = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: application/pdf\r\n\n";
                     // creation d'un QbyteArray indiquant un texte au format html
                 }

                 found = str.contains(".mp3", Qt::CaseInsensitive);
                 if (found==true)
                 {
                     *NewByteArray = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: audio/mp3\r\n\n";
                     // creation d'un QbyteArray indiquant un texte au format html
                 }
                 found = str.contains(".doc", Qt::CaseInsensitive) || str.contains(".docx", Qt::CaseInsensitive);
                 if (found==true)
                 {
                     *NewByteArray = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: application/vnd.openxmlformats-officedocument.wordprocessingml.document\r\n\n";
                 }
                 found = str.contains(".pptx", Qt::CaseInsensitive);
                 if (found==true)
                 {
                     *NewByteArray = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: application/vnd.ms-powerpoint\r\n\n";
                 }


                 *NewByteArray += file->readAll();


                 // temp a été redimensionné automatiquement

                 file->close();
                 MyHash->insert(str, NewByteArray);/*insère bytearray dans Hash*/
                 *MyHashSize+=NewByteArray->size();
                 MyHashKeys->append(str);
            }
            else
            {
                 str =tr("erreur404");
            }


        }
        else //requete deja dans le hash        
            MyHashKeys->append(MyHashKeys->takeAt(MyHashKeys->lastIndexOf(str))); //on met la key en derniere position de la liste (premiere pos = LRU)

        temp = *MyHash->value(str);

    }
    if(str.contains("erreur404"))
        statistiques->AddErreur404();
    else
        statistiques->AddTraitee();
    statistiques->AddOctets(temp.size());
    tcpSocket.write(temp);


    //Gestion de la taille du cache, suppression du LRU (last recently used)
    while(*MyHashSize > CACHE_SIZE && MyHash->count()>1 )//Tant qu'on ne dépasse pas la taille max et qu'il reste plus d'un item dans le Hash (on ne veut pas supprimer erreur 404)
    {
        *MyHashSize-=MyHash->value(MyHashKeys->first())->size();//On soustrait la taille du fichier qu'on va enlever
        delete MyHash->value(MyHashKeys->first());
        MyHash->remove(MyHashKeys->first());//On retire le LRU du cache
        MyHashKeys->removeFirst();//on enlève la LRU key

    }

//    for(int i = 0;i<MyHashKeys->size();i++)
//        cout<<MyHashKeys->value(i).toStdString()<<endl;
//    cout<<*MyHashSize<<endl;

    statistiques->SetCacheSize(*MyHashSize);
    cout << "Finishing MySocketClient::run()" << endl;

//! [2] //! [3]
    tcpSocket.disconnectFromHost();
    tcpSocket.waitForDisconnected();


}
//! [4]
