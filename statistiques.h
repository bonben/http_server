#ifndef STATISTIQUES_H
#define STATISTIQUES_H

#include <QString>
#include <QByteArray>
#include <QStringList>

#define REQUETES_ARRAY_SIZE 200
#define FILES_ARRAY_SIZE 200

class Statistiques
{
private:
    int NbRequetes;
    int NbTraitees; //Nombre de requetes traitees
    int NbErreurs404;
    int NbClients;
    int NbOctets;
    int CacheSize;
    QStringList* Fichiers;
    QStringList* Requetes;
    QStringList* Peers;

public:
    Statistiques();
    void AddRequete(QString Requete);
    void AddTraitee();
    void AddErreur404();
    void AddPeer(QString Peer);
    void AddOctets(int octets);
    void AddFile(QString Fichier);
    void SetCacheSize(int octets);
    QByteArray GenerateHtml();

    int getNbRequetes();
    int getNbTraitees();
    int getNbErreurs404();
    int getNbClients();
    int getNbOctets();

};

#endif // STATISTIQUES_H
