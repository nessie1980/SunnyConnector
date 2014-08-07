/**
* @file            segment.cpp
* @brief           CPP-Datei für die Headerdatei "segment.h"
*/

/* Includes */
#include "segment.h"

/* Standard-Konstruktor */
cSegment::cSegment(void)
{
    /* Initialisierung der Klassenvariablen */
    this->cConfigName = NULL;
    this->cSegmentName = NULL;
    this->KeyWords = NULL;
    this->Values = NULL;
    this->iSize = 0;
    this->iInitResult = 0;
}

/* Methode, welche den Inhalt eines Segments aus einer INI-Datei liest */
void cSegment::cSegmentINIT(void)
{
    /* Variablen*/
    ifstream fFile;                         /* Filepointer auf die INI-Datei                                                */
    int iFlag = 0;                          /* Flag pb das Segment verfügbar ist                                            */
    int iFound = 0;                         /* Flag ob ein Segment gefunden wurde                                           */
    int iCounter = 0;                       /* Zählervariable                                                               */
    size_t iKeyWordLength = 0;              /* Länge des längsten Keywords (Buffervariable)                                 */
    size_t iValueLength = 0;                /* Länge des längsten Werts (Buffervariable)                                    */
    char cString[1024];                     /* Char-Array für eine Zeile aus der INI-Datei                                  */
    char* cTemp = NULL;                     /* Tempvariable                                                                 */

    /* Initialisierung der Klassenvariablen */
    this->KeyWords = NULL;
    this->Values = NULL;
    this->iSize = 0;
    this->iInitResult = 0;

    /* CONFIG-Datei lesend öffnen */
    fFile.open(this->cConfigName , ios::in);

    /* Prüfen ob die Datei geöffnet werden konnte */
    if (fFile.fail())
    {
        this->iInitResult = -1;
    }
    else
    {
        try
        {
            /* INI-Datei Zeile für Zeile auslesen */
            while (!fFile.eof())
            {
                /* Lesen einer Zeile aus der INI-Datei */
                fFile.getline(cString, sizeof(cString));
                /* Prüfen ob das Segment vorhanden ist */
                if (!(strcmp(cString, this->cSegmentName)))
                {
                    iFlag = 1;
                    /* Flag setzen welches signalisiert, dass das Segment verfügbar ist */
                    iFound = 1;
                }

                /* Prüfen ob das Segment vorhanden ist */
                if (iFlag == 1 && strcmp(cString, this->cSegmentName))
                {
                    /* Prüfen ob die nächste Zeile ein neues Segment, Leerzeichen oder eine leere Zeile ist */
                    if ((cString[0] != '[') && (cString[0] != ' ') && (strlen(cString) > 0))
                    {
                        /* Speicher für die Tempvariable allokieren */
                        cTemp = new char [strlen(cString) + 1];
                        /* Zeile aus der INI-Datei in die Tempvariable kopieren */
                        strncpy(cTemp, cString, strlen(cString) + 1);
                        /* Tempvariable aufspliten, wobei der Seperator "=" ist */
                        cTemp = strtok (cTemp,"=");
                        if (cTemp != NULL)
                        {
                            /* Ermitteln der maximalen Länge der Keywords */
                            if (iKeyWordLength < strlen(cTemp))
                                iKeyWordLength = strlen(cTemp);

                            /* Tempvariable solange aufspliten bis kein aufspliten mehr möglich ist, wobei hier der Seperator auch "=" ist*/
                            while (cTemp != NULL)
                            {
                                cTemp = strtok (NULL, "=");
                                /* Prüfen ob die Adresse von cTemp NULL ist, wenn ist kein Spliten mehr möglich ist abbrechen. */
                                if (cTemp == NULL)
                                    break;
                                /* Ermitteln der maximalen Länge der Werte  */
                                if (iValueLength < strlen(cTemp))
                                    iValueLength = strlen(cTemp);
                            }
                        }
                        /* Speicher der Tempvariable freigeben */
                        if (cTemp != NULL)
                        {
                            delete [] cTemp;
                            cTemp = NULL;
                        }

                        /* Zähler der gefundenen Werte erhöhen */
                        this->iSize++;
                    }
                    else
                        iFlag = 0;
                }
            }

            /* Wenn ein Segment verfügbar ist, diese Segment mit seinen Keywords und dessen Werte aus der INI-Datei lesen. */
            if (iFound == 1)
            {
                /* Speicher für die Keywords allokieren*/
                /* Array von Char-Zeiger */
                this->KeyWords = new char* [this->iSize];
                for (unsigned int i = 0; i < this->iSize; i++)
                {
                    /* Array vom Typ Char mit der Länge des längsten Keywords anlegen. */
                    this->KeyWords[i] = new char [iKeyWordLength + 1];
                    *(this->KeyWords[i]) = NULL;
                }

                /* Speicher für die Werte allokieren */
                /* Array von Char-Zeiger */
                this->Values = new char* [this->iSize];
                for (unsigned int i = 0; i < this->iSize; i++)
                {
                    /* Array vom Typ Char mit der Länge des längsten Werts anlegen. */
                    this->Values[i] = new char [iValueLength + 1];
                    *(this->Values[i]) = NULL;
                }

                /* Filepointer an den Anfang der INI-Datei setzen */
                /* Zuerst alle bereits aufgetretenen Fehler löschen */
                fFile.clear();
                /* An den Anfang der INI-Datei springen */
                fFile.seekg(0, ios::beg);

                /* INI-Datei Zeile für Zeile auslesen */
                while (!fFile.eof())
                {
                    /* Lesen einer Zeile aus der INI-Datei */
                    fFile.getline(cString, sizeof(cString));
                    /* Prüfen ob das Segment vorhanden ist */
                    if (!(strcmp(cString, this->cSegmentName)))
                        iFlag = 1;

                    /* Prüfen ob das Segment vorhanden ist */
                    if (iFlag == 1 && strcmp(cString, this->cSegmentName))
                    {
                        /* Prüfen ob die nächste Zeile ein neues Segment, Leerzeichen oder eine leere Zeile ist */
                        if ((cString[0] != '[') && (cString[0] != ' ') && (strlen(cString) > 0))
                        {
                            /* Speicher für die Tempvariable allokieren */
                            cTemp = new char [strlen(cString) + 1];
                            /* Zeile aus der INI-Datei in die Tempvariable kopieren */
                            strncpy(cTemp, cString, strlen(cString) + 1);

                            /* Tempvariable aufspliten, wobei der Seperator "=" ist */
                            cTemp = strtok (cTemp,"=");
                            if (cTemp != NULL)
                            {
                                /* Das Keyword in das Keyword-Array kopieren */
                                strncpy(this->KeyWords[iCounter], cTemp, iKeyWordLength + 1);

                                /* Tempvariable solange aufspliten bis kein aufspliten mehr möglich ist, wobei hier der Seperator auch "=" ist*/
                                while (cTemp != NULL)
                                {
                                    cTemp = strtok (NULL, "=");
                                    /* Prüfen ob die Adresse von cTemp NULL ist, wenn ist kein Spliten mehr möglich ist abbrechen. */
                                    if (cTemp == NULL)
                                        break;
                                    /* Den Wert ins Werte-Array kopieren */
                                    strncpy(this->Values[iCounter], cTemp, iValueLength + 1);
                                }
                            }
                            /* Speicher der Tempvariable freigeben */
                            if (cTemp != NULL)
                            {
                                delete [] cTemp;
                                cTemp = NULL;
                            }
                        }
                        else
                            iFlag = 0;

                        iCounter++;
                    }
                }
            }
            else
                this->iInitResult = -2;
            fFile.close();
        }
        catch (exception &e)
        {
            /* Schreiben des Resultwerts (-3 = exception-failure) */
            this->iInitResult = -3;

            /* Freigeben des dynamisch allokierten Speichers der Klasse */
            /* Prüfen ob cTemp auf eine Speicheradresse zeigt, wenn ja freigeben und Zeiger auf NULL setzen */
            if (cTemp != NULL)
            {
                delete [] cTemp;
                cTemp = NULL;
            }

            /* Freigeben des dynamisch allokierten Speichers der Klasse */
            freeMemory();
        }
    }
}

/* Diese Methode gibt den dynamisch allokierten Speicher der Klasse cSegment frei (private) */
void cSegment::freeMemory (void)
{
    /* Prüfen ob ein Dateiname gespeichert wurde, wenn ja freigeben und Zeiger auf NULL setzen */
    if (this->cConfigName != NULL)
    {
        delete [] this->cConfigName;
        this->cConfigName = NULL;
    }

    /* Prüfen ob ein Segmentname gespeichert wurde, wenn ja freigeben und Zeiger auf NULL setzen */
    if (this->cSegmentName != NULL)
    {
        delete [] this->cSegmentName;
        this->cSegmentName = NULL;
    }

    /* Prüfen ob Keywords gespeichert wurden, wenn ja freigeben und Zeiger auf NULL setzen */
    if (this->KeyWords != NULL)
    {
        for(unsigned int i = 0; i < this->iSize; i++)
        {
            if (this->KeyWords[i] != NULL)
            {
                delete [] this->KeyWords[i];
                this->KeyWords[i] = NULL;
            }
        }
    }
    if (this->KeyWords != NULL)
    {
        delete [] this->KeyWords;
        this->KeyWords = NULL;
    }

    /* Prüfen ob Werte gespeichert wurden, wenn ja freigeben und Zeiger auf NULL setzen */
    if (this->Values != NULL)
    {
        for(unsigned int i = 0; i < this->iSize; i++)
        {
            if (this->Values[i] != NULL)
            {
                this->Values[i] = NULL;
                delete [] this->Values[i];
            }
        }
    }
    if (this->Values != NULL)
    {
        delete [] this->Values;
        this->Values = NULL;
    }

}

/* Destructor */
cSegment::~cSegment(void)
{
    /* Freigeben des dynamisch allokierten Speichers der Klasse */
    freeMemory();
}
