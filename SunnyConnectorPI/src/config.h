/**
* @file            config.h
* @brief           Diese Klasse ermöglicht das Auslesen einer INI-Datei und speichert die Werte ab.
* @details         Ein Objekt vom Typ cConfig genötig den Namen der INI-Datei.
*                  z.B. cConfig Objekt("CONFIG.INI");
*                  Die Variable iInitResult speichert das Ergebnis des Initialisierung
*                  &nbsp;0               = erfolgreich
* @details         -1              = INI-Dateifehler (z.B. nicht vorhanden)
* @details         -2              = kein Segment vorhanden
* @details         -3              = Exceptionfehler
* @details         Der iInitResult-Wert ermöglicht es Ihnen im Hauptprogram zu prüfen
*                  ob die Initialisierung erfolgreich war
*                  INI-Datei-Syntax:
*                  [SEGEMENT]      = Segment-Tag
*                  KEY=VALUE&nbsp;       = Keyword mit dem dazugehörigen Wert
*
*                  Die Klasse besitzt eine Zeigervariable für den Typ cSegment.
*                  Die Klasse legt für jedes gefundene Segment in der INI-Datei
*                  ein Objekt vom Typ cSegment an. Das Objekt vom Typ cSegment
*                  beinhaltet die Keywords und die dazugehörigen Werte.
*                  Die Keyword und Werte werden in einem zweidimensionalen Array gespeichert.
*                  Der erste Zeiger char* zeigt auf die char-Arrays für die Keywords und deren Werte.
*                  Die Länge für die zweite Dimension, also die Keywords bzw. Werte wird dynamisch
*                  allokiert.
*
*                  Die Klasse stellt Variablen zur Verfügung, welche es einem erlauben
*                  Informationen über das Objekt zu ermittlen.
*                  Für weitere Informationen lesen Sie die Klasseninformationen.
*/
#ifndef CONFIG_H_
#define CONFIG_H_

/* Includes */
#include "segment.h"

/**
* @class cConfig
* @brief           Diese Klasse speichert Informationen über Segmente in einer INI-Datei.
* @details         Diese Klasse speichert ein Array vom Typ cSegment.
*                  (Für weitere Informationen siehe class cSegment).
*                  Die Objekte in dem Array werden mit dem Werten aus dem Segment der INI-Datei gefüllt.
*                  Die Größe des Arrays wird in der Variablen iSize gespeichert.
*                  Das Ergebnis der Initialisierung wird in der Variablen iInitResult abgelegt.
*                  Der Name der INI-Datei steht in der Variablen cFileName.
*                  Diese Variablen sind alle private.
*                  Die Variablen werden über GET-Methoden nach außen hin zur Verfügung gestellt.
*                  Nur der Zeiger auf das Array vom Typ cSegment besitzt keine GET-Methode.
* @author          Thomas Barth
* @date            24.11.2011
* @note            Angelegt
* @version         1.0
*/
class cConfig
{
private:
        char* cFileName;                    /* Name der INI-Datei                                                       */
        cSegment* Segment;                  /* Zeiger für ein Array, welches Objekte vom Typ cSegment aufnehmen kann    */
        size_t iSize;                       /* Größe des Array, welches die Objekte vom Typ cSegment aufnimmt.          */
        int iInitResult;                    /* Ergebnis der Konstruktor-Initialisierung                                 */

public:
        /**
        * @fn         cConfig(void)
        * @brief      Standard-Konstruktor
        * @details    Initialisiert die privaten Variablen
        * @param      -
        * @return     -
        * @author     Thomas Barth
        * @date       24.11.2011
        * @note       Angelegt
        * @version    1.0
        */
        cConfig(void);

        /**
        * @fn         cConfig(const char* cFile)
        * @brief      Eigener Konstruktor
        * @details    Liest alle Segmente aus der INI-Datei.
        *             Der Konstruktor legt für jedes gefundene Segment ein
        *             Objekt vom Typ cSegment im dynamisch allokierten Array an.
        *             In diesen cSegment-Objekten speichert die Klasse die Keywords und ihren Werten.
        *             Die Klasse speichert ebenfalls den Namen der INI-Datei und den Namen des gespeicherten Segments.
        *             Die Variable iInitResult speichert das Ergebnis der Initialisierung.
        * @param      cFile             = const char* (Name der INI-Datei)
        * @return     -
        * @author     Thomas Barth
        * @date       24.11.2011
        * @note       Angelegt
        * @version    1.0
        */
        cConfig(const char*);

        /**
        * @fn         const char** getSegmentValues (const char* cSegment) const
        * @brief      GET-Methode, welche ein Array mit den Segmentwerten des angegebenen Segments zurück gibt.
        * @details    Diese Methode sucht nach dem angegebenen Segment und gibt von angegebenen Segment die Werte in
        *             einem konstanten Char-Array zurück.
        * @param      cSegment          = const char* (Name des Segments)
        * @return     char**            = char** (Werte des Segments als const char**-Zeiger)
        * @return     NULL&nbsp;&nbsp;              = char** (Wenn das Segment nicht verfügbar ist wird ein NULL-Zeiger zurückgegeben)
        * @author     Thomas Barth
        * @date       26.11.2011
        * @note       Angelegt
        * @version    1.0
        */
        const char** getSegmentValues (const char* cSegment) const;

        /**
        * @fn         const char** getSegmentKeywords (const char* cSegment) const
        * @brief      GET-Methode, welche ein Array mit den Keywords des angegebenen Segments zurück gibt.
        * @details    Diese Methode sucht nach dem angegebenen Segment und gibt von angegebenen Segment die Keywords in
        *             einem konstanten Char-Array zurück.
        * @param      cSegment          = const char* (Name des Segments)
        * @return     char**            = char** (Keywords des Segments als const char**-Zeiger)
        * @return     NULL&nbsp;&nbsp;              = char** (Wenn das Segment nicht verfügbar ist wird ein NULL-Zeiger zurückgegeben)
        * @author     Thomas Barth
        * @date       27.12.2011
        * @note       Angelegt
        * @version    1.0
        */
        const char** getSegmentKeywords (const char* cSegment) const;

        /**
        * @fn         int getSegmentValuesSize (const char* cSegment)
        * @brief      GET-Methode, welche die Größe des Werte-Arrays eines Segments zurück gibt.
        * @details    Diese Methode stellt die Größe des angegebenen Segment-Array zur Verfügung.
        * @param      cSegment          = const char* (Name des Segments)
        * @return     int               = int (Größe des Array, gleich oder größer 0)
        * @return     int               = int (Wenn das Segment nicht verfügbar ist wird -1 zurückgegeben)
        * @author     Thomas Barth
        * @date       26.11.2011
        * @note       Angelegt
        * @version    1.0
        */
        int getSegmentValuesSize(const char* cSegment);

        /**
        * @fn         char* getSegmentValueByKeyword (const char* cSegment,const char* cKeyWord)
        * @brief      GET-Methode, welche den Wert eines angegebenen Keywords zurück gibt.
        * @details    Diese Methode sucht in einem Segment nach dem angegebenen Keyword und gibt
        *             einen Char-Zeiger auf den gefundenen Wert zurück.
        * @param      cSegment          = const char* (Name des Segments)
        * @param      cKeyWord          = const char* (Name des Keywords)
        * @return     char*             = char* (Wert des Keywords als Char-Zeiger)
        * @return     NULL              = char* (Wenn das Keyword nicht verfügbar ist wird ein NULL-Zeiger zurückgegeben)
        * @author     Thomas Barth
        * @date       24.11.2011
        * @note       Angelegt
        * @version    1.0
        */
        char* getSegmentValueByKeyword (const char* cSegment, const char* cKeyWord);

        /**
        * @fn         char* getSegmentKeywordByValue (const char* cSegment,const char* cValue)
        * @brief      GET-Methode, welche das Keyword eines angegebenen Values zurück gibt.
        * @details    Diese Methode sucht in einem Segment nach dem angegebenen Value und gibt
        *             einen Char-Zeiger auf das gefundene Keyword zurück.
        * @param      cSegment          = const char* (Name des Segments)
        * @param      cValue            = const char* (Wert des Values)
        * @return     char*             = char* (Keywords des Values als Char-Zeiger)
        * @return     NULL              = char* (Wenn das Value nicht verfügbar ist wird ein NULL-Zeiger zurückgegeben)
        * @author     Thomas Barth
        * @date       27.12.2011
        * @note       Angelegt
        * @version    1.0
        */
        char* getSegmentKeywordByValue (const char* cSegment, const char* cValue);

        /**
        * @fn         int getFileName (void)
        * @brief      GET-Methode, welche den Namen der INI-Datei zurück gibt
        * @param      -
        * @return     char*             = char* (Wenn ein Dateiname gesetzt wurde, wird diese als Char-Zeiger zurückgegeben)
        * @return     NULL              = char* (Wenn kein Dateiname gesetzt ist wird ein NULL-Zeiger zurückgegeben)
        * @author     Thomas Barth
        * @date       25.11.2011
        * @note       Angelegt
        * @version    1.0
        */
        char* getFileName (void)
        { return this->cFileName; }

        /**
        * @fn         int getSize (void)
        * @brief      GET-Methode, welche die Größe des cSegment-Array zurück gibt
        * @param      -
        * @return     int             = int (Größe des cSegment-Array)
        * @author     Thomas Barth
        * @date       24.11.2011
        * @note       Angelegt
        * @version    1.0
        */
        int getSize (void)
        { return this->iSize; }

        /**
        * @fn         int getInitResult (void)
        * @brief      GET-Methode, welche das Ergebnis der Initialisierung zurück gibt
        * @details    &nbsp;0               = int (erfolgreich)
        * @details    -1              = int (INI-Dateifehler (z.B. nicht vorhanden))
        * @details    -2              = int (Kein Segment verfügbar)
        * @details    -3              = int (Exceptionfehler)
        * @param      -
        * @return     int             = int (Ergebnis der Initialisierung)
        * @author     Thomas Barth
        * @date       24.11.2011
        * @note       Angelegt
        * @version    1.0
        */
        int getInitResult (void)
        { return this->iInitResult; }

        /**
        * @fn         ~cConfig(void)
        * @brief      Destruktor
        * @details    Gibt den dynamisch allokierten Speicher frei
        *             Der Destruktor prüft ob ein Zeiger auf reservierten Speicher zeigt und gibt diesen
        *             gegebenenfalls frei. Anschliessend wird der Zeiger noch auf NULL gesetzt.
        * @param      -
        * @return     -
        * @author     Thomas Barth
        * @date       24.11.2011
        * @note       Angelegt
        * @version    1.0
        */
        ~cConfig(void);
};

#endif /* CONFIG_H_ */
