/**
* @file            mailmsg.cpp
* @brief           CPP-file für die Headerdatei "mailmsg.h"
*/

#include "mailmsg.h"

#include <iostream>
#include <exception>
using namespace std;

/* Eigener Konstruktor mit der Nachricht */
cMailMsg::cMailMsg(char cMessage[]): NextMsg(NULL)
{
    /* Prüfen ob der Speicher für die Nachricht allokiert werden kann */
    try
    {
        this->cMessage = new char[strlen(cMessage) + 1];

        strcpy(this->cMessage, cMessage);

        this->iInitResult = 1;
    }
    catch (bad_alloc &)
    {
        /* Freigeben des dynamisch allokierten Speichers der Klasse */
        if (this->cMessage != NULL)
        {
            delete [] this->cMessage;
            this->cMessage = NULL;
        }

        /* Klassen-Initialisierungs-Wert auf -1 setzen */
        this->iInitResult = -1;
    }
}

/* Methode zum Anzeigen einer Nachricht */
void cMailMsg::showMsg(void)
{
    /* Prüfen ob eine Nachrichtentext vorhanden ist */
    if (this->cMessage != NULL)
        printf ("Message: %s\n", this->cMessage);
    else
        printf ("Keine Message vorhanden!\n");
}

/* Methode zum Anzeigen aller Nachricht */
void cMailMsg::showAllMsg(void)
{
    /* Prüfen ob eine Nachrichtentext vorhanden ist */
    if (this->cMessage != NULL)
        printf ("Message: %s\n", this->cMessage);
    else
        printf ("Keine Message vorhanden!\n");

    /* Prüfen ob ein weiteres Objekt folgt, wenn ja ausgeben */
    if (this->NextMsg != NULL)
        this->NextMsg->showAllMsg();
}

/* Methode zum Erstellen eines MailContent-Strings */
char* cMailMsg::getAllMsg (char* cMailContent)
{
    int iMsgLength = 0;                 /* Länge der akutellen Nachricht                                                    */
    int iContentLength = 0;             /* Länge der bereits zusammen gesetzten Nachricht                                   */
    char* cMsgTemp = NULL;              /* Temporäres Char-Array für das Zusammensetzen                                     */

    /* Länge der akutellen Nachricht bestimmen */
    iMsgLength = strlen(this->getMsg());
    /* Prüfen ob bereits ein MailContent vorhanden ist, wenn ja des Länge bestimmen */
    if (cMailContent != NULL)
        iContentLength = strlen (cMailContent);

    /* Prüfen ob der Speicher allokiert werden konnte */
    try
    {
        /* Char-Array allokieren für das Zusammensetzen */
        cMsgTemp = new char[iContentLength + iMsgLength + 1 ];
    }
    catch (bad_alloc&)
    {
        if (cMsgTemp != NULL)
        {
            delete [] cMsgTemp;
            cMsgTemp = NULL;
        }

        if (cMailContent != NULL)
        {
            delete [] cMailContent;
            cMailContent = NULL;
        }

        return cMailContent;
    }

    /* Prüfen ob schon eine MailContent vorhanden ist */
    if (cMailContent != NULL)
    {
        strcpy(cMsgTemp, cMailContent);
        strcat(cMsgTemp, this->getMsg());
    }
    else
    {
        strcpy(cMsgTemp, this->getMsg());
    }

    /* Akutellen MailContent löschen */
    if (cMailContent != NULL)
    {
        delete [] cMailContent;
        cMailContent = NULL;
    }

    /* Prüfen ob der Speicher allokiert werden konnte */
    try
    {
        /* Neuen Speicher für den aktualisierten MailContent allokieren */
        cMailContent = new char[strlen(cMsgTemp) + 1 ];
    }
    catch (bad_alloc&)
    {
        if (cMsgTemp != NULL)
        {
            delete [] cMsgTemp;
            cMsgTemp = NULL;
        }

        if (cMailContent != NULL)
        {
            delete [] cMailContent;
            cMailContent = NULL;
        }

        return cMailContent;
    }

    /* Aktualisierten MailContent in den eigentlichen Speicher kopieren */
    strcpy (cMailContent, cMsgTemp);

    /* Temporären Speicher freigeben */
    if (cMsgTemp != NULL)
    {
        delete [] cMsgTemp;
        cMsgTemp = NULL;
    }

    /* Rekrusiver Funktionsaufruf, für das Durchlaufen bis an das Ende der Liste */
    if (this->NextMsg != NULL)
        return this->NextMsg->getAllMsg(cMailContent);

    return cMailContent;
}

/* Destruktor */
cMailMsg::~cMailMsg()
{
    /* Prüfen ob Speicher für die Nachricht allokiert wurde */
    if (this->cMessage != NULL)
    {
        delete [] this->cMessage;
        this->cMessage = NULL;
    }
}

