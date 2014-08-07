/**
* @file            mailqueue.cpp
* @brief           CPP-file für die Headerdatei "mailqueue.h"
*/

#include "mailqueue.h"

#include <iostream>
#include <exception>
using namespace std;

/* Eigener Konstruktor mit Betreffzeile */
cMailQueue::cMailQueue(char* cSubject)
{
    /* Zeiger initialisieren */
    this->StartMsg = NULL;
    this->LastMsg  = NULL;

    /* Speicher für die Betreffzeile allokieren */
    try
    {
        this->cMailHeader = new char[strlen(MAIL_HEADER) + 1];
        strncpy(this->cMailHeader, MAIL_HEADER, strlen(MAIL_HEADER) + 1);

        this->cMailFooter = new char[strlen(MAIL_FOOTER) + 1];
        strncpy(this->cMailFooter, MAIL_FOOTER, strlen(MAIL_FOOTER) + 1);

        this->cSubject = new char[strlen(cSubject) + 1];

        strncpy(this->cSubject, cSubject, strlen(cSubject) + 1 );

        this->iInitResult = 1;
    }
    catch (bad_alloc &)
    {
        /* Freigeben des dynamisch allokierten Speichers der Klasse */
        if (this->cSubject != NULL)
        {
            delete [] this->cSubject;
            this->cSubject = NULL;
        }

        /* Klassen-Initialisierungs-Wert auf -1 setzen */
        this->iInitResult = -1;
    }
}

/* Eigener Konstruktor mit Betreffzeile und erster Meldung */
cMailQueue::cMailQueue(char* cSubject, cMailMsg* Msg)
{
    /* Zeiger initialisieren */
    this->StartMsg = NULL;
    this->LastMsg  = NULL;

    /* Speicher für die Betreffzeile allokieren */
    try
    {
        this->cMailHeader = new char[strlen(MAIL_HEADER) + 1];
        strncpy(this->cMailHeader, MAIL_HEADER, strlen(MAIL_HEADER));

        this->cMailFooter = new char[strlen(MAIL_FOOTER) + 1];
        strncpy(this->cMailFooter, MAIL_FOOTER, strlen(MAIL_FOOTER));

        this->cSubject = new char[strlen(cSubject) + 1];

        strcpy(this->cSubject, cSubject);

        this->iInitResult = 1;
    }
    catch (bad_alloc &)
    {
        /* Freigeben des dynamisch allokierten Speichers der Klasse */
        if (this->cSubject != NULL)
        {
            delete [] this->cSubject;
            this->cSubject = NULL;
        }

        /* Klassen-Initialisierungs-Wert auf -1 setzen */
        this->iInitResult = -1;
    }

    /* Adresse der ersten Meldung in den Start und Last-Adresse speichern */
    if (this->StartMsg == NULL)
    {
        this->StartMsg = Msg;
        this->LastMsg  = Msg;
    }
}

/* Hinzufügen einer neuen Meldung */
void cMailQueue::addMsg(cMailMsg* newMsg)
{
    /* Prüfen ob sich schon eine Meldung in der Liste befindet */
    /* entsprechend die Adressen setzen */
    if (this->StartMsg == NULL)
        this->StartMsg = newMsg;
    else
        this->LastMsg->NextMsg = newMsg;

    this->LastMsg = newMsg;
}

/* Anzeigen der Betreffzeile der Liste */
void cMailQueue::showSubject(void)
{
    if (this->cSubject != NULL)
        printf ("Betreff: %s\n", this->cSubject);
    else
        printf ("Kein Betreff vorhanden!\n");
}

/* Anzeigen der Meldungen in der Liste */
void cMailQueue::showQueue(void)
{
    /* Prüfen ob sich eine Meldung in der Liste befindet */
    if (this->StartMsg != NULL)
        StartMsg->showAllMsg();
    else
        printf ("Keine Meldungen vorhanden!\n");
}

/* Methode zum Erstellen eines MailContent-Strings */
char* cMailQueue::getAllMsg (char* cMailContent)
{
    if (this->StartMsg != NULL)
        return this->StartMsg->getAllMsg(cMailContent);
    else
    {
        return cMailContent;
    }
}

/* Methode zum Löschen aller Queue-Objekte */
int cMailQueue::delAllMsg (void)
{
    cMailMsg* cTempPointer = NULL;

    if (this->StartMsg != NULL)
    {
        cTempPointer = this->StartMsg;
        if (this->StartMsg->NextMsg != NULL)
            this->StartMsg = this->StartMsg->NextMsg;
        else
            this->StartMsg = NULL;
    }

    if (cTempPointer != NULL)
    {
        delete cTempPointer;
        cTempPointer = NULL;
    }

    if (this->StartMsg != NULL)
        this->delAllMsg();

    return 1;
}

/* Destruktor */
cMailQueue::~cMailQueue()
{
    /* Alles Nachrichten löschen */
    this->delAllMsg();

    /* Betreffzeile löschen */
    if (this->cSubject != NULL)
    {
        delete [] this->cSubject;
        this->cSubject = NULL;
    }

    /* Header löschen */
    if (this->cMailHeader != NULL)
    {
        delete [] this->cMailHeader;
        this->cMailHeader = NULL;
    }

    /* Footer löschen */
    if (this->cMailFooter != NULL)
    {
        delete [] this->cMailFooter;
        this->cMailFooter = NULL;
    }
}

