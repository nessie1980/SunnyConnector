/**
* @file            segment.h
* @brief           Diese Klasse arbeitet nur in Zusammenhang mit der Calsse cConfig-Klasse.
* @details         Die Variable iInitResult speichert das Ergebnis der Initialisierung.
*                  &nbsp;0               = erfolgreich
* @details         -1              = INI-Dateifehler (z.B. nicht vorhanden)
* @details         -2              = kein Segment vorhanden
* @details         -3              = Exceptionfehler
* @details         Der iInitResult-Wert ermöglicht es Ihnen im Hauptprogramm zu prüfen
*                  ob die Initialisierung erfolgreich war
*                  INI-Datei-Syntax:
*                  [SEGEMENT]      = Segment-Tag
*                  KEY=VALUE&nbsp;       = Keyword mit dem dazugehörigen Wert
*
*                  Die Klasse cSegment speichert Keywords mit den dazugehörigen Werten.
*                  Die Keyword und Werte werden in einem zweidimensionalen Array gespeichert.
*                  Der erste Zeiger char* zeigt auf die char-Arrays für die Keywords und deren Werte.
*                  Die Länge für die zweite Dimension, also die Keywords bzw. Werte, wird dynamisch
*                  allokiert.
*
*                  Die Variablen sind alle private, weil die Variablen nur von der Klasse cConfig
*                  benötigt werden, welche die eigentliche Hauptklasse ist.
*/

#ifndef SEGMENT_H_
#define SEGMENT_H_

#include <fstream>
#include <iostream>
#include <exception>
#include <string.h>

using namespace std;

/**
* @class cSegment
* @brief           Diese Klasse speichert Informationen eines Segments aus einer INI-Datei.
* @details         Diese Klasse speichert Informationen eines Segments aus einer INI-Datei.
*                  Die Keywords und die dazu gehörigen Werte werden in zwei seperaten zweidimensonalen Arrays gespeichert.
*                  Die Größe des Arrays wird in der Variablen iSize gespeichert.
*                  Die Größe der beiden Arrays ist gleich, denn jedes Keyword hat einen Wert.
*                  Sollte ein Keyword keinen Wert haben ist der Wert im Array ebenfalls leer.
*                  Das Ergebnis der Initialisierung wird in der Variablen iInitResult abgelegt.
*                  Der Segmentname wird in der Variablen cSegmentName gespeichert.
*                  Der Name der INI-Datei steht in der Variablen cFileName.
*
*                  Die Variablen sind alle private, weil die Variablen nur von der Klasse cConfig
*                  benötigt werden, welche die eigentliche Hauptklasse ist.
*
* @author          Thomas Barth
* @date            24.11.2011
* @note            Angelegt
* @version         1.0
*/
class cSegment
{
private:
        char* cConfigName;                      /* Name der INI-Datei                                                                       */
        char* cSegmentName;                     /* Name des Segemnts in der INI-Datei                                                       */
        char** KeyWords;                        /* Zeiger auf ein Array, welche die Keywords eines Segments aus der INI-Datei aufnimmt      */
        char** Values;                          /* Zeiger auf ein Array, welches die Werte eines Segments aus der INI-Datei aufnimmt        */
        size_t iSize;                           /* Größe des Array, welches die Keywords und ihre Werte aus der INI-DAtei speichert         */
        int iInitResult;                        /* Ergebnis der Konstruktorinitialisierung                                                  */

        /**
        * @fn         void freeMemory (void)
        * @brief      Diese Methode gibt den dynamisch allokierten Speicher der Klasse cSegment frei.
        * @details    Die Methode prüft ob ein Zeiger auf reservierten Speicher zeigt und gibt diesen
        *             gegebenenfalls frei. Anschliessend wird der Zeiger noch auf NULL gesetzt.
        * @param      -
        * @return     -
        * @author     Thomas Barth
        * @date       24.11.2011
        * @note       Angelegt
        * @version    1.0
        */
        void freeMemory (void);

public:
        /**
        * @fn         cSegment(void)
        * @brief      Standard-Konstruktor
        * @details    Initialisiert die privaten Variablen
        * @param      -
        * @return     -
        * @author     Thomas Barth
        * @date       24.11.2011
        * @note       Angelegt
        * @version    1.0
        */
        cSegment(void);

        /**
        * @fn         void cSegmentINIT(void)
        * @brief      Diese Methode liest den Inhalt eines Segments aus einer INI-Datei.
        * @details    Aufgerufen wird diese Methode nach dem ein Segment von der cConfig-Klasse in der
        *             INI-Datei gefunden wird. Danach werden die Keywords und die jeweiligen Werte des
        *             Segments gelesen und im Objekt gespeichert.
        * @param      -
        * @return     -
        * @author     Thomas Barth
        * @date       24.11.2011
        * @note       Angelegt
        * @version    1.0
        */
        void cSegmentINIT(void);

        /**
        * @fn         friend class cConfig
        * @brief      Friend-Klasse cConfig.
        * @details    Die cConfig-Klasse verwaltet ein Array vom Typ dieser Klasse cSegment.
        * @param      -
        * @return     -
        * @author     Thomas Barth
        * @date       24.11.2011
        * @note       Angelegt
        * @version    1.0
        */
        friend class cConfig;

        /**
        * @fn        ~cSegment(void)
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
        ~cSegment(void);
};

#endif /* SEGMENT_H_ */
