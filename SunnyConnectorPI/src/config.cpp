/**
* @file            config.cpp
* @brief           CPP-file für die Headerdatei "config.h"
*/

/* Includes */
#include "config.h"

/* Standard-Konstruktor */
cConfig::cConfig(void)
{
    /* Initialisierung der Klassenvariablen */
    this->cFileName = NULL;
    this->Segment = NULL;
    this->iSize = 0;
    this->iInitResult = 0;
}

/* Eigener Konstruktor */
cConfig::cConfig(const char* cFile)
{
    /* Variablen*/
    ifstream fFile;                         /* Filepointer auf die INI-Datei                                    */
    size_t iSegmentCounter=0;               /* Anzahl der gefundenen Segmente in der INI-Datei                  */
    char cSegmentStart='[';                 /* Buchstabe der den Beginn eines Segments kennzeichnet             */
    char cString[1024];                     /* Char-Array für eine Zeile aus der INI-Datei                      */

    /* Initialisierung der Klassenvariablen */
    this->cFileName = NULL;
    this->Segment = NULL;
    this->iSize = 0;
    this->iInitResult = 0;

    /* CONFIG-Datei lesend oeffnen */
    fFile.open(cFile, ios::in);

    /* Prüfen ob die Datei geoeffnet werden konnte */
    if (fFile.fail())
    {
        this->iInitResult = -1;
    }
    else
    {
        try
        {
            /* Speichern des Namen der INI-Datei in das cConfig-Objekt */
            this->cFileName = new char [strlen(cFile) + 1];
            strncpy (this->cFileName, cFile, strlen(cFile) + 1);

            /* INI-Datei Zeile für Zeile auslesen */
            while (!fFile.eof())
            {
                /* Lesen einer Zeile aus der INI-Datei */
                fFile.getline(cString, sizeof(cString));
                /* Prüfen ob die Zeile ein neues Segement kennzeichnet und die Anzahl der gefundenen Segmente erhoehen */
                if (cString[0] == cSegmentStart)
                {
                    this->iSize++;
                }
            }

            if (this->iSize > 0)
            {
                /* Speicher für die Anzahl der gefundenen Segmente vom Typ "cSegment" allokieren  */
                this->Segment = new cSegment [this->iSize];

                /* Filepointer an den Anfang der INI-Datei setzen */
                /* Zuerst alle bereits aufgetretenen Fehler loeschen */
                fFile.clear();
                /* An den Anfang der INI-Datei springen */
                fFile.seekg(0, ios::beg);

                /* INI-Datei Zeile für Zeile auslesen */
                while (!fFile.eof())
                {
                    /* Lesen einer Zeile aus der INI-Datei */
                    fFile.getline(cString, sizeof(cString));
                    /* Prüfen ob die Zeile ein neues Segement kennzeichnet */
                    if (cString[0] == cSegmentStart)
                    {
                        /* Speicher für den Dateinnamen der INI-Datei allokieren */
                        this->Segment[iSegmentCounter].cConfigName = new char [strlen(cFile) + 1];
                        /* Speichern des Dateinamen in das cSegment-Objekt */
                        strncpy(this->Segment[iSegmentCounter].cConfigName, cFile, strlen(cFile) + 1);

                        /* Speicher für den Namen des akutellen Segments allokieren */
                        this->Segment[iSegmentCounter].cSegmentName = new char [strlen(cString) + 1];
                        /* Speichern des aktuellen Segmentnamen in das cSegment-Objekt */
                        strncpy(this->Segment[iSegmentCounter].cSegmentName, cString, strlen(cString) + 1);
                        /* Einlesen der Keywords mit dem dazugehoerigen Wert */
                        this->Segment[iSegmentCounter].cSegmentINIT();

                        /* Zähler der Segmente erhoehen */
                        iSegmentCounter++;
                    }

                    /* Wenn die Anzahl der zuvor ermittelten Segmente erreicht ist hier abbrechen */
                    if (iSegmentCounter >= this->iSize)
                        break;
                }
            }
            else
                /* Schreiben des Resultwerts, dass die INI-Datei nicht erfolgreich geoeffnet werden konnte */
                this->iInitResult = -2;

            /* INI-Datei wieder schliessen */
            fFile.close();
        }
        catch (exception &e)
        {
            /* Schreiben des Resultwerts (-3 = exception-failure) */
            this->iInitResult = -3;

            /* Freigeben des dynamisch allokierten Speichers der Klasse */
            /* Prüfen ob ein Dateiname gespeichert wurde, wenn ja freigeben und Zeiger auf NULL setzen */
            if (this->cFileName != NULL)
            {
                delete [] this->cFileName;
                this->cFileName = NULL;
            }

            /* Prüfen ob bereits Objekte vom Typ "cSegment" gespeichert wurden, wenn ja freigeben und Zeiger auf NULL setzen */
            if (this->Segment != NULL)
            {
                for (size_t i = 0; i < this->iSize; i++)
                {
                    delete [] this->Segment;
                }
            }
        }
    }
}

/* Methode, welche die im angegebenen Segment gespeicherten Werte in einem Char-Array zurück gibt */
/* Sollte das angegebene Segment nicht gefunden werden wird NULL-Zeiger zurück gegeben */
const char** cConfig::getSegmentValues (const char* cSegment) const
{
    for (size_t i = 0; i < this->iSize; i++)
    {
        if (!strcmp(this->Segment[i].cSegmentName, cSegment))
        {
            return (const char**) this->Segment[i].Values;
        }
    }
    return NULL;
}

/* Methode, welche die im angegebenen Segment gespeicherten Keywords in einem Char-Array zurück gibt */
/* Sollte das angegebene Segment nicht gefunden werden wird NULL-Zeiger zurück gegeben */
const char** cConfig::getSegmentKeywords (const char* cSegment) const
{
    for (size_t i = 0; i < this->iSize; i++)
    {
        if (!strcmp(this->Segment[i].cSegmentName, cSegment))
        {
            return (const char**) this->Segment[i].KeyWords;
        }
    }
    return NULL;
}

/* Methode, welche die Groesse des angegebenen Segments zurück gibt. */
/* Sollte das angegebene Segment nicht gefunden werden wird "-1" zurück gegeben */
int cConfig::getSegmentValuesSize(const char* cSegment)
{
    for (size_t i = 0; i < this->iSize; i++)
    {
        if (!strcmp(this->Segment[i].cSegmentName, cSegment))
        {
            return this->Segment[i].iSize;
        }
    }
    return -1;
}

/* Methode, welche den Wert des angegeben Keywords aus dem angegebenen Segment zurück gibt. */
/* Sollte das Keyword im Segment nicht vorhanden sein, so wird ein NULL-Zeiger zurückgegeben. */
char* cConfig::getSegmentValueByKeyword (const char* cSegment, const char* cKeyWord)
{
    for (size_t i = 0; i < this->iSize; i++)
    {
        if (!strcmp(this->Segment[i].cSegmentName, cSegment))
        {
            for (size_t j = 0; j < this->Segment[i].iSize; j++)
            {
                if (!strcmp(this->Segment[i].KeyWords[j], cKeyWord))
                {
                    return this->Segment[i].Values[j];
                }
            }
        }
    }
    return NULL;
}

/* Methode, welche das Keyword des angegeben Values aus dem angegebenen Segment zurück gibt. */
/* Sollte das Value im Segment nicht vorhanden sein, so wird ein NULL-Zeiger zurückgegeben. */
char* cConfig::getSegmentKeywordByValue (const char* cSegment, const char* cValue)
{
    for (size_t i = 0; i < this->iSize; i++)
    {
        if (!strcmp(this->Segment[i].cSegmentName, cSegment))
        {
            for (size_t j = 0; j < this->Segment[i].iSize; j++)
            {
                if (!strcmp(this->Segment[i].Values[j], cValue))
                {
                    return this->Segment[i].KeyWords[j];
                }
            }
        }
    }
    return NULL;
}

/* Destruktor */
cConfig::~cConfig(void)
 {
    /* Freigeben des dynamisch allokierten Speichers der Klasse */
    /* Prüfen ob ein Dateiname gespeichert wurde, wenn ja freigeben und Zeiger auf NULL setzen */
    if (this->cFileName != NULL)
    {
        delete [] this->cFileName;
        this->cFileName = NULL;
    }

    /* Prüfen ob bereits Objekte vom Typ "cSegment" gespeichert wurden, wenn ja freigeben und Zeiger auf NULL setzen */
    if (this->Segment != NULL)
    {
        for (size_t i = 0; i < this->iSize; i++)
        {
                delete [] this->Segment;
        }
    }
 }
