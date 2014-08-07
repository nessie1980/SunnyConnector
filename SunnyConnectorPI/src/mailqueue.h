 /**
* @file            mailqueue.h
* @brief           Diese Klasse dient zum Erstellen einer Mailwarteschlange.
* @details         Ein Objekt vom Typ cMailQueue genötig den Betreff für die Mail.
*                  z.B. cMailQueue Objekt("Test-Betreff");
*
*                  Die Variable iInitResult speichert das Ergebnis des Initialisierung
*                  &nbsp;1               = erfolgreich
* @details         -1              = Speicherallokierung für die Betreffzeile schlug fehl
* @details         Der iInitResult-Wert ermöglicht es Ihnen im Hauptprogram zu prüfen,
*                  ob die Initialisierung erfolgreich war
*
*                  Die Klasse besitzt zwei Zeigervariablen für den Typ cMailMsg.
*                  StartMsg stellt dabei den Startpunkt für die Liste dar, während
*                  LastMsg auf das letzte Objekt in der Liste zeigt.
*                  Mit der Methode addMsg(cMailMsg) kann man eine neue MailMessage hinzufügen.
*                  Durch die Methode showQueue(void) kann man die Liste der Messages anzeigen lassen.
*                  Befindet sich keine Nachricht in der Warteschlange, so wird eine Meldung dies bezüglich ausgegeben.
*/

#ifndef MAILQUEUE_H_
#define MAILQUEUE_H_

#include "mailmsg.h"
#include "defines.h"

/**
* @class cMailQueue
* @brief           Diese Klasse dient zum Erstellen einer Mailwarteschlange.
* @details         Ein Objekt vom Typ cMailQueue genötig den Betreff für die Mail.
* @details         Diese Klasse speichert eine Liste aus Mailinhalten für eine Info-Mail
*                  des SunnyConnector32.
*                  Im Zeiger StartMsg steht die Startadresse der Liste.
*                  Der Zeiger LastMsg enthält die Adresse des letzten Listenelements.
*                  Der Betreff für die Mail wird in einem Char-Array gespeichert, worauf
*                  der Zeiger cSubject zeigt.
*                  Der iInitResult-Wert ermöglicht es Ihnen im Hauptprogram zu prüfen,
*                  ob die Initialisierung erfolgreich war
*
* @author          Thomas Barth
* @date            13.05.2012
* @note            Angelegt
* @version         1.0
*/
class cMailQueue
{
    private:
        char* cSubject;                     /* Zeiger auf das Array mit der Betreffzeile                                        */
        char* cMailHeader;                  /* Header für die E-Mail                                                            */
        char* cMailFooter;                  /* Footer für die E-Mail                                                            */
        cMailMsg* StartMsg;                 /* Startadresse der Liste                                                           */
        cMailMsg* LastMsg;                  /* Zeiger auf das letzte Listenelement                                              */
        int iInitResult;                    /* Ergebnis der Initisalisierung (konnte Speicher für Betreff reserviert werden)    */

    public:
        /**
        * @fn         cMailQueue(char*)
        * @brief      Konstruktor
        * @details    Speichert die Betreffzeile und setzt die Startadresse
        *             und Adresse des letzten Elements auf NULL
        * @param      char*        = Zeiger auf die Betreffzeile
        * @return     -
        * @author     Thomas Barth
        * @date       13.05.2012
        * @note       Angelegt
        * @version    1.0
        */
        cMailQueue(char*);

        /**
        * @fn         cMailQueue(char*, cMailMsg*)
        * @brief      Konstruktor
        * @details    Speichert die Betreffzeile und die erste Meldung
        * @param      char*        = Zeiger auf die Betreffzeile
        * @param      cMailMsg*    = Zeiger auf ein Meldungsobjekt
        * @return     -
        * @author     Thomas Barth
        * @date       13.05.2012
        * @note       Angelegt
        * @version    1.0
        */
        cMailQueue(char*, cMailMsg*);

        /**
        * @fn         void addMsg(cMailMsg*)
        * @brief      Methode zum Hinzufügen einer neuen Meldung.
        * @details    Die neue Meldung wird ans Ende der Liste eingefügt.
        *             Es wird geprüft, ob sich bereits eine Meldung in der Liste befindet
        *             und fügt entsprechend das Element ein.
        * @param      cMailMsg*        = Zeiger auf das Meldungsobjekt
        * @return     -
        * @author     Thomas Barth
        * @date       13.05.2012
        * @note       Angelegt
        * @version    1.0
        */
        void addMsg(cMailMsg*);

        /**
        * @fn         char* getSubject(void)
        * @brief      Methode um die Betreff zu erhalten.
        * @details    Gibt die gespeicherte Betreffzeile des Objekts zurück
        * @param      -
        * @return     char*        = Zeiger auf die gespeicherte Betreffzeile
        * @author     Thomas Barth
        * @date       13.05.2012
        * @note       Angelegt
        * @version    1.0
        */
        char* getSubject(void) { return this->cSubject; }

        /**
        * @fn         char* getHeader(void)
        * @brief      Methode um den Header zu erhalten.
        * @details    Gibt den gespeicherten Header des Objekts zurück
        * @param      -
        * @return     char*        = Zeiger auf den gespeicherte Header
        * @author     Thomas Barth
        * @date       22.05.2012
        * @note       Angelegt
        * @version    1.0
        */
        char* getHeader(void) { return this->cMailHeader; }

        /**
        * @fn         char* getFooter(void)
        * @brief      Methode um den Footer zu erhalten.
        * @details    Gibt den gespeicherten Footer des Objekts zurück
        * @param      -
        * @return     char*        = Zeiger auf den gespeicherte Footer
        * @author     Thomas Barth
        * @date       22.05.2012
        * @note       Angelegt
        * @version    1.0
        */
        char* getFooter(void) { return this->cMailFooter; }

        /**
        * @fn         void showSubject(void)
        * @brief      Methode zum Anzeigen der Betreffzeile.
        * @details    Es wird die Betreffzeile für die Mail der Meldungsliste.
        * @param      -
        * @return     -
        * @author     Thomas Barth
        * @date       13.05.2012
        * @note       Angelegt
        * @version    1.0
        */
        void showSubject (void);

        /**
        * @fn         void showQueue(void)
        * @brief      Methode zum Anzeigen der Liste.
        * @details    Es wird bei der Anfang der Liste begonnen und diese
        *             bis zum Ende durchlaufen.
        * @param      -
        * @return     -
        * @author     Thomas Barth
        * @date       13.05.2012
        * @note       Angelegt
        * @version    1.0
        */
        void showQueue (void);

        /**
        * @fn         char* getAllMsg(char*)
        * @brief      Methode um die Meldungen zu einem Text zusammen zufassen.
        * @details    Liest die einzelen Meldungen in ein Char-Array ein.
        * @param      char*       = Char-Array für die Meldungen
        * @return     char*       = Char-Array mit der neuen Meldung
        * @return     char*       = NULL (Fehler beim Erstellen des MailContent. Fehlerhafte Speicherallokierung)
        * @author     Thomas Barth
        * @date       14.05.2012
        * @note       Angelegt
        * @version    1.0
        */
        char* getAllMsg (char*);

        /**
        * @fn         int delAllMsg(void)
        * @brief      Methode um alle Meldungen aus einer Queue zu löschen.
        * @details    Löscht alle Meldungen aus einer Queue.
        *             &nbsp;1         = int (erfolgreich)
        *             0               = int (Fehler beim Löschen)
        *
        * @param      -
        * @return     int         = Rückgabewert ob das Löschen erfolgreich war (1 = Ok / 0 = Fehler)
        * @author     Thomas Barth
        * @date       16.05.2012
        * @note       Angelegt
        * @version    1.0
        */
        int delAllMsg (void);

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
        * @fn         ~cConfig(void)
        * @brief      Destruktor
        * @details    Gibt den dynamisch allokierten Speicher frei
        *             Der Destruktor prüft, ob ein Zeiger auf reservierten Speicher zeigt und gibt diesen
        *             gegebenenfalls frei. Anschliessend wird der Zeiger noch auf NULL gesetzt.
        * @param      -
        * @return     -
        * @author     Thomas Barth
        * @date       13.05.2012
        * @note       Angelegt
        * @version    1.0
        */
        ~cMailQueue();
};

#endif /* MAILQUEUE_H_ */
