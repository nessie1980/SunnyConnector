 /**
* @file            mailmsg.h
* @brief           Diese Klasse dient als Eintrag/Meldung in einer Mailwarteschlange.
* @details         Ein Objekt vom Typ cMailMsg genötig die Meldung für die Mail.
*                  z.B. cMailMsg Objekt("Test-Mail");
*
*                  Die Variable iInitResultResult speichert das Ergebnis des Initialisierung
*                  &nbsp;1               = erfolgreich
* @details         -1              = Speicherallokierung für die Meldung schlug fehl
* @details         Der iInitResult-Wert ermöglicht es Ihnen im Hauptprogram zu prüfen,
*                  ob die Initialisierung erfolgreich war
*
*                  Die Klasse besitzt Zeigervariablen für den Typ cMailMsg, welche auf die
*                  mögliche nächste Meldung zeigt.
*                  Mit der Methode getMsg(void) kann man den Inhalt einer Meldung ermittelen.
*                  Durch die Methode showMsg(void) kann man eine Meldung anzeigen lassen.
*                  Ist die Nachricht leer, so wird eine Meldung dies bezüglich ausgegeben.
*/

#ifndef MAILMSG_H_
#define MAILMSG_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
* @class cMailMsg
* @brief           Diese Klasse dient als Eintrag/Meldung in einer Mailwarteschlange.
* @details         Ein Objekt vom Typ cMailMsg genötig die Meldung für die Mail.
*                  Die Meldungen werden vom SunnyConnector selbständig angelegt und
*                  zur Warteschlange hinzugefügt.
*
*                  Das Char-Array cMessage nimmt die Meldung auf, während NextMsg auf
*                  die evtl. folgende Meldung zeigt.
*                  Der iInitResult-Wert ermöglicht es Ihnen im Hauptprogram zu prüfen,
*                  ob die Initialisierung erfolgreich war
*
* @author          Thomas Barth
* @date            13.05.2012
* @note            Angelegt
* @version         1.0
*/
class cMailMsg
{
    private:
        int iInitResult;                    /* Ergebnis der Initisalisierung (konnte Speicher für Nachricht reserviert werden)  */
        char* cMessage;                     /* Zeiger auf das Array mit der Nachricht                                           */
        cMailMsg* NextMsg;                  /* Zeiger auf die nächste Meldung                                                   */

    public:
        /**
        * @fn         cMailMsg(char*)
        * @brief      Konstruktor mit Nachricht
        * @details    Speichert die Nachricht und setzt den Zeiger auf das nächste
        *             Element auf NULL
        * @param      char*        = Zeiger auf die Meldung
        * @return     -
        * @author     Thomas Barth
        * @date       13.05.2012
        * @note       Angelegt
        * @version    1.0
        */
        cMailMsg(char*);

        /**
        * @fn         char* getMsg(void)
        * @brief      Methode um eine Nachricht zu erhalten.
        * @details    Gibt die gespeichert Nachricht des Objekts zurück
        * @param      -
        * @return     char*        = Zeiger auf die gespeicherte Nachricht
        * @author     Thomas Barth
        * @date       13.05.2012
        * @note       Angelegt
        * @version    1.0
        */
        char* getMsg(void) { return this->cMessage; }

        /**
        * @fn         void showMsg(void)
        * @brief      Methode um eine Nachricht anzuzeigen.
        * @details    Gibt die gespeichert Nachricht des Objekts aus
        * @param      -
        * @return     -
        * @author     Thomas Barth
        * @date       13.05.2012
        * @note       Angelegt
        * @version    1.0
        */
        void showMsg (void);

        /**
        * @fn         void showAllMsg(void)
        * @brief      Methode um alle Nachrichten anzuzeigen.
        * @details    Gibt die gespeicherten Nachricht des Objekt
        *             und aller folgenden Objekte aus.
        * @param      -
        * @return     -
        * @author     Thomas Barth
        * @date       13.05.2012
        * @note       Angelegt
        * @version    1.0
        */
        void showAllMsg(void);

        /**
        * @fn         char* getAllMsg(char*)
        * @brief      Methode um die Meldungen zu einem Text zusammen zufassen.
        * @details    Liest die einzelen Meldungen in ein Char-Array ein.
        * @param      char*       = Char-Array für die Meldungen
        * @return     char*       = Char-Array mit der neuen Meldung
        * @return     char*       = NULL (Fehler beim Erstellen des MailContent. Fehlerhafte Speicherallokierung)
        * @author     Thomas Barth
        * @date       13.05.2012
        * @note       Angelegt
        * @version    1.0
        */
        char* getAllMsg (char*);

        /**
        * @fn         int getInitResult(void)
        * @brief      Methode um das InitResullt zu erhalten.
        * @details    Gibt das InitResult des Objekts zurück
        * @param      -
        * @return     int         = InitResult-Wert
        * @author     Thomas Barth
        * @date       13.05.2012
        * @note       Angelegt
        * @version    1.0
        */
        int getInitResult(void) { return this->iInitResult; }

        /**
        * @fn         ~cMailMsg(void)
        * @brief      Destruktor
        * @details    Gibt den dynamisch allokierten Speicher frei
        *             Der Destruktor prüft ob ein Zeiger auf reservierten Speicher zeigt und gibt diesen
        *             gegebenenfalls frei. Anschliessend wird der Zeiger noch auf NULL gesetzt.
        * @param      -
        * @return     -
        * @author     Thomas Barth
        * @date       13.05.2012
        * @note       Angelegt
        * @version    1.0
        */
        ~cMailMsg();

        /**
        * @fn         friend class cMailQueue
        * @brief      Friend-Klasse cMailQueue.
        * @details    Die cMailQueue-Klasse verwaltet eine verkettete Liste diese Klasse cMailMsg.
        * @param      -
        * @return     -
        * @author     Thomas Barth
        * @date       13.05.2012
        * @note       Angelegt
        * @version    1.0
        */
        friend class cMailQueue;
};

#endif /* MAILMSG_H_ */
