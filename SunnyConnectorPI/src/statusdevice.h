/**
* @file            statusdevice.h
* @brief           Dies ist die Headerdatei für die Klasse "cStatusDevice".
* @details         Diese Klasse speichert Informationen über einen Wechselrichter.
*                  Jede Information ist über SET- und GET-Methoden erreichbar.
*/

#ifndef STATUSDEVICE_H_
#define STATUSDEVICE_H_

/* Includes */
#include <iostream>
#include <string.h>
#include <libyasdimaster.h>
#include "defines.h"

/* Namespaces */
using namespace std;

/**
* @class cStatusDevice
* @brief           Die Klasse speichert Informationen über einen Wechselrichter.
* @details         Die Klasse speichert Informationen über einen Wechselrichter.
*                  Die Informationen sind z.B. ob der Wechselrichter online ist oder nicht usw.
*                  Für weitere Informationen siehe "Private Attribute".
*                  Die Attribute können per GET-Methode gelesen und per SET-Methode gesetzt werden.
* @author          Thomas Barth
* @date            08.12.2011
* @note            Angelegt
* @version         1.0
*/
class cStatusDevice
{
private:
        bool bPowerOnFlag;                              /**< Flag für den Status on/off eines Geräts (on = true / off = false)          */
        bool bFeedInFlag;                               /**< Flag für den Status ob ein Wechselrichter Strom einspeist oder nicht       */
        DWORD iDeviceSN ;                               /**< Seriennnummer deines Geräts                                                */
        static unsigned int  iDeviceCounter;            /**< Anzahl der Objekte für die gefundenen Geräte                               */
        static unsigned int  iDeviceBufferCounter;      /**< Anzahl der Objekte für die gepufferten Objekte                             */

public:
        /**
        * @fn         cStatusDevice(void)
        * @brief      Standard-Konstruktor
        * @details    Initialisiert die privaten Variablen
        * @param      -
        * @return     -
        * @author     Thomas Barth
        * @date       08.12.2011
        * @note       Angelegt
        * @version    1.0
        */
        cStatusDevice(void);

        /**
        * @fn         cStatusDevice(int DeviceSN)
        * @brief      Eigener Konstruktor
        * @details    Initialisiert die privaten Variablen
        * @param      DeviceSN   = int (Seriennummer eines Wechselrichteres)
        * @return     -
        * @author     Thomas Barth
        * @date       08.12.2011
        * @note       Angelegt
        * @version    1.0
        */
        cStatusDevice(int DeviceSN);

        /**
        * @fn         bool getPowerOnFlag (void)
        * @brief      GET-Methode, welche den Status zurück gibt ob ein Gerät an (true) oder aus (off) ist
        * @param      -
        * @return     bool              = bool (Status ob ein Geräte an oder aus ist)
        * @author     Thomas Barth
        * @date       08.12.2011
        * @note       Angelegt
        * @version    1.0
        */
        bool getPowerOnFlag (void) {return this->bPowerOnFlag;}

        /**
        * @fn         cStatusDevice &operator= (const cStatusDevice &Referenz)
        * @brief      Methode, welche es ermöglicht ein StatusDevice-Objekt zu kopieren per "="-Operator
        * @param      Referenz      = cStatusDevice& (Referenz des alten Objekts)
        * @return     cStatusDevice = cStatusDevice& (Referenz des neuen Objekts)
        * @author     Thomas Barth
        * @date       17.12.2011
        * @note       Angelegt
        * @version    1.0
        */
        cStatusDevice& operator= (const cStatusDevice &Referenz);

        /**
        * @fn         bool getFeedInFlag (void)
        * @brief      GET-Methode, welche zurück gibt ob ein Wechselrichter Strom einspeist oder nicht
        * @param      -
        * @return     bool              = bool (Status ob ein Wecheslrichter Strom einspeist oder nicht)
        * @author     Thomas Barth
        * @date       08.12.2011
        * @note       Angelegt
        * @version    1.0
        */
        bool getFeedInFlag (void) {return this->bFeedInFlag;}

        /**
        * @fn         int getSN (void)
        * @brief      GET-Methode, welche die Seriennummer eines Wechselrichters zurück gibt
        * @param      -
        * @return     DWORD              = DWORD (Seriennummer als unsigned int)
        * @author     Thomas Barth
        * @date       08.12.2011
        * @note       Angelegt
        * @version    1.0
        */
        DWORD getSN (void) {return this->iDeviceSN;}

        /**
        * @fn         unsigned int getDeviceCounter (void)
        * @brief      GET-Methode, welche die Anzahl der gefundenen Geräte zurück gibt
        * @param      -
        * @return     unsigned int      = unsigned int (Anzahl der Objekte)
        * @author     Thomas Barth
        * @date       08.12.2011
        * @note       Angelegt
        * @version    1.0
        */
        unsigned int getDeviceCounter (void) {return this->iDeviceCounter;}

        /**
        * @fn         unsigned int getDeviceBufferCounter (void)
        * @brief      GET-Methode, welche die Anzahl der gepufferten Geräte zurück gibt
        * @param      -
        * @return     unsigned int      = unsigend int (Anzahl der gepufferten Objekte)
        * @author     Thomas Barth
        * @date       08.12.2011
        * @note       Angelegt
        * @version    1.0
        */
        unsigned int getDeviceBufferCounter (void) {return this->iDeviceBufferCounter;}

        /**
        * @fn         void setPowerOnFlag (bool)
        * @brief      SET-Methode, welche den Status ob ein Wechselrichter an oder aus ist setzt.
        * @param      PowerOnFlag             = bool (Status ob das Geräte an (true) oder aus (false) ist)
        * @return     -
        * @author     Thomas Barth
        * @date       08.12.2011
        * @note       Angelegt
        * @version    1.0
        */
        void setPowerOnFlag (bool PowerOnFlag) {this->bPowerOnFlag = PowerOnFlag;}

        /**
        * @fn         void setFeedInFlag (bool)
        * @brief      SET-Methode, welche den Status setzt ob der Wechselrichter Strom einspeist oder nicht.
        * @param      FeedInFlag             = bool (Status ob ein Gerät Strom einspeist oder nicht)
        * @return     -
        * @author     Thomas Barth
        * @date       08.12.2011
        * @note       Angelegt
        * @version    1.0
        */
        void setFeedInFlag (bool FeedInFlag) {this->bFeedInFlag = FeedInFlag;}

       /**
        * @fn         void setSN (const DWORD dwSerNr)
        * @brief      SET-Methode, welche die Seriennummer des Wechselrichters setzt.
        * @param      dwSerNr               = dwSerNr (Seriennummer als unsigned int)
        * @return     -
        * @author     Thomas Barth
        * @date       08.12.2011
        * @note       Angelegt
        * @version    1.0
        */
        void setSN (const DWORD dwSerNr){this->iDeviceSN = dwSerNr;}

       /**
        * @fn         void setDeviceCounter (const DWORD dwDeviceCounter)
        * @brief      SET-Methode, welche die Anzahl der gefundenen Geräte setzt.
        * @param      dwDeviceCounter      = dwDeviceCounter (Anzahl der gefundenen Geräte)
        * @return     -
        * @author     Thomas Barth
        * @date       25.12.2011
        * @note       Angelegt
        * @version    1.0
        */
        void setDeviceCounter (const DWORD dwDeviceCounter){this->iDeviceCounter = dwDeviceCounter;}

       /**
        * @fn         void setDeviceBufferCounter (const DWORD dwDeviceBufferCounter)
        * @brief      SET-Methode, welche die Anzahl der zu puffernden Geräte setzt.
        * @param      dwDeviceBufferCounter  = dwDeviceCounter (Anzahl der zu puffernden Geräte)
        * @return     -
        * @author     Thomas Barth
        * @date       25.12.2011
        * @note       Angelegt
        * @version    1.0
        */
        void setDeviceBufferCounter (const DWORD dwDeviceCounter){this->iDeviceBufferCounter = dwDeviceCounter;}

        /**
        * @fn         ~cStatusDevice(void)
        * @brief      Destruktor, welcher dynamisch allokierten Speicher frei gibt.
        * @param      -
        * @return     -
        * @author     Thomas Barth
        * @date       08.12.2011
        * @note       Angelegt
        * @version    1.0
        */
        ~cStatusDevice(void);
};

#endif /* STATUSDEVICE_H_ */
