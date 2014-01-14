#include "statistiques.h"
#include "MySocketServer.h"

Statistiques::Statistiques()
{

    Requetes = new QStringList();
    Peers = new QStringList();
    Fichiers = new QStringList();
    NbRequetes=0;
    NbTraitees=0; //Nombre de requetes traitees
    NbErreurs404=0;
    NbClients=0;
    NbOctets=0;
    CacheSize=0;
}

void Statistiques::AddRequete(QString Requete)
{
    NbRequetes++;
    Requetes->append(Requete);
}

void Statistiques::AddTraitee()
{
    NbTraitees++;
}
void Statistiques::AddErreur404()
{
   NbErreurs404++;
}
void Statistiques::AddPeer(QString Peer)
{
    Peers->append(Peer);
}

void Statistiques::AddOctets(int octets)
{
    NbOctets+=octets;
}

void Statistiques::AddFile(QString Fichier)
{
    Fichiers->append(Fichier);
}

void Statistiques::SetCacheSize(int octets)
{
    CacheSize=octets;
}

int Statistiques::getNbRequetes()
{
    return NbRequetes;
}

int Statistiques::getNbTraitees()
{
    return NbTraitees;
}

int Statistiques::getNbErreurs404()
{
    return NbErreurs404;
}

int Statistiques::getNbClients()
{
    return NbClients;
}

int Statistiques::getNbOctets()
{
    return NbOctets;
}

QByteArray Statistiques::GenerateHtml()
{
    QByteArray temp("");
    temp = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\n\n";
    temp += "<html>\r\n";
    temp += "<head><title>Statistiques</title></head><body>";
    temp += "<CENTER><FONT COLOR=#800080 SIZE=7>Statistiques</FONT></CENTER>";
    temp += "<p>Nombre de requetes : ";
    temp += QString::number(NbRequetes);
    temp += "</p>";
    temp += "<p>Nombre de requetes trait&eacutees : ";
    temp += QString::number(NbTraitees);
    temp += "</p>";
    temp += "<p>Nombre de Octets transmis : ";
    temp += QString::number(NbOctets);
    temp += "</p>";
    temp += "<p>Nombre d'erreurs 404 : ";
    temp += QString::number(NbErreurs404);
    temp += "</p>";
    temp+="<p>Occupation du cache : "+ QString::number(CacheSize) +"/"+QString::number(CACHE_SIZE)+"</p>";
    temp+="<br />";
    temp+="<TABLE BORDER=\"1\">";
    temp+="<CAPTION>Liste des requ&ecirctes</CAPTION>";
    temp+="<TR><TH>Requ&ecirctes</TH><TH>Nombre d'it&eacuterations</TH></TR>";

    for(int i = 0;i<Requetes->size();i++)
    {
        Requetes->sort();
        i=Requetes->lastIndexOf(Requetes->value(i));


        temp+="<TR><TD>";
        temp+= Requetes->value(i);
        temp+="</TD><TD>";
        temp+=QString::number(Requetes->count(Requetes->value(i)));
        temp+="</TD></TR>";
    }
    temp+="</TABLE>";
    temp+="<TABLE BORDER=\"1\">";
    temp+="<br /><CAPTION>Liste des fichiers t&eacutel&eacutecharg&eacutes</CAPTION>";
    temp+="<TR><TH>Adresse</TH><TH>Nombre de fichiers t&eacutel&eacutecharg&eacutes</TH></TR>";

    for(int i = 0;i<Fichiers->size();i++)
    {
        Fichiers->sort();
        i=Fichiers->lastIndexOf(Fichiers->value(i));


        temp+="<TR><TD>";
        temp+= Fichiers->value(i);
        temp+="</TD><TD>";
        temp+=QString::number(Fichiers->count(Fichiers->value(i)));
        temp+="</TD></TR>";
    }
    temp+="</TABLE>";


    temp+="<TABLE BORDER=\"1\">";
    temp+="<br /><CAPTION>Liste des clients</CAPTION>";
    temp+="<TR><TH>Adresse</TH><TH>Nombre de requ&ecirctes</TH></TR>";

    for(int i = 0;i<Peers->size();i++)
    {
        Peers->sort();
        i=Peers->lastIndexOf(Peers->value(i));


        temp+="<TR><TD>";
        temp+= Peers->value(i);
        temp+="</TD><TD>";
        temp+=QString::number(Peers->count(Peers->value(i)));
        temp+="</TD></TR>";
    }
    temp+="</TABLE>";

    temp += "</body></html> ";



    return temp;
}

