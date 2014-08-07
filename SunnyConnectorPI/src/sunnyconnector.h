/**
* @file            sunnyconnector.h
* @brief           Dies ist Headerdatei für den SunnyConnector32
* @details         Sie stellt Methoden für die Kommunikation zwischen PC und SMA-Wechselrichter zur Verfügung.
*/

#ifndef SUNNYHEADER_H_
#define SUNNYHEADER_H_

/* Includes */
#include <iomanip>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <libyasdimaster.h>
#include "defines.h"
#include "version.h"
#include "config.h"
#include "statusdevice.h"
#include "mailmsg.h"
#include "mailqueue.h"

/* Namespaces */
using namespace std;

/* Strukturen */

/**
* @struct strConfigs
* @brief           Struktur für die Konfigurationsdateien.
* @details         Struktur, welche die Namen der Konfigurationsdateien aufnimmt
* @author          Thomas Barth
* @date            27.12.2011
* @note            Angelegt
* @version         1.0
*/
struct strConfigs
{
    char* cYasdiINI;                                       /**< Name der YASDI.INI-Datei                                                  */
    char* cConfigINI;                                      /**< Name der CONFIG.INI-Datei                                                 */
};

/**
* @struct strTimers
* @brief           Struktur für diverse Timer.
* @details         Struktur, welche diverse Timer beinhaltet
* @author          Thomas Barth
* @date            27.12.2011
* @note            Angelegt
* @version         1.0
*/
struct strTimers
{
    int iTimerData;                                        /**< Zeitintervall zwischen den Anfragen                                       */
};

/**
* @struct strStatus
* @brief           Struktur für die Statusvariablen.
* @details         Struktur, welche diverse Statuswerte aufnimmt.
* @author          Thomas Barth
* @date            27.12.2011
* @note            Angelegt
* @version         1.0
*/
struct strStatus
{
    char* cStatusChannel;                                  /**< Name des Statuschannel                                                    */
    char* cStatusValue;                                    /**< Wert für den Statuschannel wenn Strom eingespeist wird                    */
};

/**
* @struct strFile
* @brief           Struktur für Einstellungen der Dateispeicherung.
* @details         Struktur, welche Einstellungen zur Dateispeicherung aufnimmt.
* @author          Thomas Barth
* @date            27.12.2011
* @note            Angelegt
* @version         1.0
*/
struct strFile
{
    bool bSave;                                            /**< Flag, ob die Werte in Dateien gespeichert werden sollen                     */
    char* cSeperatorFile;                                  /**< Seperatorzeichen zwischen den Werten in den Dateien                         */
    char* cExt;                                            /**< Dateierweiterung für die Logdateien                                         */
};

/**
* @struct strMySQL
* @brief           Struktur für Einstellungen der MySQL-Speicherung.
* @details         Struktur, welche Einstellungen zur MySQL-Speicherung aufnimmt.
* @author          Thomas Barth
* @date            27.12.2011
* @note            Angelegt
* @version         1.0
*/
struct strMySQL
{
    bool bSave;                                            /**< Flag, ob die Werte in eine MySQL-Datenbank gespeichert werden sollen       */
    char* cHost;                                           /**< Name des MySQL-Servers                                                     */
    char* cPort;                                           /**< Port des MySQL-Servers                                                     */
    char* cUser;                                           /**< User für den MySQL-Server                                                  */
    char* cPassword;                                       /**< Passwort für den MySQL-Server                                              */
    char* cDatabase;                                       /**< Datenbank auf dem MySQL-Server                                             */
    char* cSNTable;                                        /**< Tabelle für die Seriennummern auf dem MySQL-Server                         */
    char* cDataTable;                                      /**< Tabelle für die Daten auf dem MySQL-Server                                 */
};

/**
* @struct strDaemon
* @brief           Struktur für Einstellungen des Daemon.
* @details         Struktur, welche Einstellungen für dem Daemon aufnimmt.
* @author          Thomas Barth
* @date            27.12.2011
* @note            Angelegt
* @version         1.0
*/
struct strDaemon
{
    mode_t iDaemonUmask;                                   /**< UMASK für die vom Daemon erstellten Dateien                                */
    mode_t iDaemonUmaskMkdir;                              /**< UMASK für die vom Daemon erstellen Verzeichnisse                           */
    int iDaemonize;                                        /**< Variable, ob das Programm als Daemon laufen soll oder nicht                */
    char* cDaemonName;                                     /**< Name für den Daemon                                                        */
    char* cDaemonPidFile;                                  /**< Datei für die PID des Programms                                            */
    char* cDaemonDir;                                      /**< Verzeichnis für die Daemondateien                                          */
};

/**
* @struct strMail
* @brief           Struktur für Einstellungen des Mailversands.
* @details         Struktur, welche Einstellungen für den Mailversand aufnimmt.
* @author          Thomas Barth
* @date            17.05.2012
* @note            Angelegt
* @version         1.0
*/
struct strMail
{
    bool bMail;                                            /**< Flag, ob Systemmeldungen per Mail versendet werden sollen                  */
    char* cServer;                                         /**< Name des SMTP-Servers                                                      */
    char* cPort;                                           /**< Port des SMTP-Servers                                                      */
    char* cTls;                                            /**< Tls benutzen oder nicht (auto/no/yes)                                      */
    char* cUser;                                           /**< Benutzer für die SMTP-Anmeldung                                            */
    char* cPassword;                                       /**< Passwort des Benutzers für die SMTP-Anmeldung                              */
    char* cFrom;                                           /**< Name des Senders                                                           */
    char* cTo;                                             /**< Name des Empfängers                                                        */
};

/**
* @fn         void PrintUsage(int argc, char *argv[])
* @brief      Methode, welche die Benutzung des Programms anzeigt.
* @details    Es prüft ob der Benutzer beim Start des Programms Parameter mit angegeben hat.
*             Wenn der Benutzer keine(n) Parameter angegeben hat wird die Benutzung
*             des Programms angezeigt.
* @param      argc             = int (Anzahl der Parameter, der Programmname ist ebenfalls ein Parameter)
* @param      argv             = char* [] (Array mit dem Übergabeparameteren)
* @return     -
* @author     Thomas Barth
* @date       01.12.2011
* @note       Angelegt
* @version    1.0
*/
void PrintUsage(int argc, char* argv[]);

/**
* @fn         int initSunnyConnector(cConfig& cConfig, struct strTimers* strTimers, struct strStatus* strStatus, struct strFile* strFile, struct strMySQL* strMySQL, struct strConfigs* strConfigs, struct strDaemon* strDaemon, struct strMail, cStatusDevice* cStatusDevices)
* @brief      Methode, welche alle Variablen setzt die nötig sind und prüft ob alle Konfigurationsdaten vorhanden sind
* @details    Diese Methode prüft ob alle nötigen Parameter in der CONFIG.INI
*             vorhanden sind. Sollte ein Parameter nicht vorhanden sein, wird dieser
*             mit einem Defaultwert aus der Datei defines.h belegt.
* @param      cConfig         = cConfig& (Referenz auf die Klasse cConfig)
* @param      strTimers       = struct strTimers (Struktur mit diversen Timern)
* @param      strStatus       = struct strStatus* (Struktur mit den Statuschannel und Statusvalue)
* @param      strFile         = struct strFile* (Struktur mit Einstellung zur Speicherung von Daten in Dateien)
* @param      strMySQL        = struct strMySQL* (Struktur mit Einstellung zur Speicherung von Daten auf einem MySQL-Server)
* @param      strConfigs      = struct strConfigs* (Struktur mit den Konfigurationsdateien)
* @param      strDaemon       = struct strDaemon* (Struktur mit Einstellungen für den Daemon)
* @param      strMail         = struct strMail* (Struktur mit den Einstellungen für den Mailversand)
* @param      cStatusDevices  = cStatusDevices (Zeiger auf die Statusobjekte der einzelnen Geräte)
* @return     -
* @author     Thomas Barth
* @date       27.12.2011
* @note       Angelegteugt
* @version    1.0
*/
int initSunnyConnector(cConfig& cConfig, struct strTimers* strTimers, struct strStatus* strStatus, struct strFile* strFile, struct strMySQL* strMySQL, struct strConfigs* strConfigs, struct strDaemon* strDaemon, struct strMail*, cStatusDevice* cStatusDevices);

/**
* @fn         int initYasdi(DWORD* dwBusDriverCounter, DWORD* dwDeviceCounter, DWORD* dwDeviceSNCounter, DWORD* dwDeviceHandle, char* cYasdiINI, bool* bFirstInitFlag, cConfig& cConfig, cStatusDevice** cStatusDevices)
* @brief      Methode für das Initialisiern des YASDI-Treiber per YASDI.INI.
* @details    Es werden die Bustreiber gestartet, welche in der YASDI.INI gefunden werden.
*             Ermitteln der erreichbaren Wechselrichter.
*             Ermitteln der Devicehandles für die gefundenen Wechselrichter.
* @param      dwBusDriverCounter    = DWORD* (Zeiger auf den Wert der gefundenen Bustreiber in der YASDI.INI)
* @param      dwDeviceCounter       = DWORD* (Zeiger auf den Wert der gefundenen Wechselrichter)
* @param      dwDeviceSNCounter     = DWORD* (Zeiger auf den Wert der gefundenen Wechselrichter laut Seriennummern)
* @param      dwDeviceHandle        = DWORD* (Zeiger auf ein Array der Devicehandles)
* @param      cYasdiINI             = char*  (Name der YASDI-INI-Datei)
* @param      bFirstInitFlag        = bool   (Flag für die erste YASDI-Initialisation)
* @param      cConfig               = cConfig& (Array der Seriennummern)
* @param      cStatusDevices        = cStatusDevices** (Zeiger auf ein Array vom Typ "statusdevice*", damit werden Objekte für die Stati gespeichert)
* @return     &nbsp;0                     = int (Initialisation war erfoglreich)
* @return     -1                    = int (Etwas ging schief)
* @author     Thomas Barth
* @date       01.12.2011
* @note       Angelegt
* @version    1.0
*/
int initYasdi(DWORD* dwBusDriverCounter, DWORD* dwDeviceCounter, DWORD* dwDeviceSNCounter, DWORD* dwDeviceHandle, char* cYasdiINI, bool* bFirstInitFlag, cConfig& cConfig, cStatusDevice** cStatusDevices);

/**
* @fn         int getYasdiData(const DWORD* dwDeviceHandle, const DWORD* dwDeviceCounter, cConfig& cConfig, cStatusDevice** cStatusDevices, struct strStatus* strStatus, struct strFile* strFile, struct strMySQL* strMySQL, struct strMail* strMail, char cVersionsstring[])
* @brief      Methode für das Anfragen von Daten aus einem Wechselrichter und dem Zurückgeben des Anfragestatus
* @details    Es werden von den einzelnen Wechselrichtern Daten geholt und in die Datei bzw./und MySQL-Server gespeichert
*             Das Speichern erfolgt nur wenn der Wechselrichter auch wirklich Strom einspeist.
* @param      dwDeviceHandle        = const DWORD* (Array für die Devicehandlers)
* @param      dwDeviceCounter       = const DWORD* (Wert der verfügbaren Geräte)
* @param      cConfig               = cConfig& (Array der anzufragenden Werte)
* @param      strConfigs      		= struct strConfigs* (Struktur mit den Konfigurationsdateien)
* @param      strDaemon       		= struct strDaemon* (Struktur mit Einstellungen für den Daemon)
* @param      cStatusDevices        = cStatusDevice** (Zeiger auf ein Array vom Typ "statusdevice*", damit werden Objekte für die Stati gespeichert)
* @param      strStatus             = struct strStatus* (Struktur mit den Statuschannel und Statusvalue)
* @param      strFile               = struct strFile* (Struktur mit Einstellung zur Speicherung von Daten in Dateien)
* @param      strMySQL              = struct strMySQL* (Struktur mit Einstellung zur Speicherung von Daten auf einem MySQL-Server)
* @param      strMail               = struct strMail* (Struktur mit den Einstellungen für den Mailversand)
* @param      cMailQueueError       = cMailQueueError* (Liste der Systemfehler)
* @param      cMailQueueInfo        = cMailQueueInfo* (Liste der Systemmeldungen)
* @param      cMailQueueStop        = cMailQueueStop (Systemmeldung das der SunnyConnecotr32 beendet wurde)
* @param      cVersionsstring       = char[] (Versionsstring des Programms)
* @return     &nbsp;0                     = int (Anfragekanal war ungültig)
* @return     -1                    = int (Channelhandle war ungültig)
* @return     -2                    = int (YASDI-Treiber ist im "ShutDown"-Mode. Sie können keine Werte anfragen, wenn  YASDI "schläft...". Wert in ungültig)
* @return     -3                    = int (Timeout vom Gerät. Gerät ist nicht erreichbar. Wert ist ungültig)
* @return     -4                    = int (Unbekannter Fehler. Wert ungültig)
* @return     -5                    = int (Nicht ausreichendes Zugriffsrecht zum Lesen des Kanalwerts)
* @return     -6                    = int (Devicehandle ist ungültig)
* @author     Thomas Barth
* @date       18.05.2014
* @note       Geändert
* @version    1.01
*/
int getYasdiData(const DWORD* dwDeviceHandle, const DWORD* dwDeviceCounter, cConfig& cConfig, struct strConfigs* strConfigs, struct strDaemon* strDaemon, cStatusDevice** cStatusDevices, struct strStatus* strStatus, struct strFile* strFile, struct strMySQL* strMySQL, struct strMail*, cMailQueue* cMailQueueError, cMailQueue* cMailQueueSystem, cMailQueue* cMailQueueStop, char cVersionsstring[]);

// TODO
/**
* @fn         int sendMail(struct strMail* strMail, char cVersionsstring[], cMailQueue* cMailQueue)
* @author     Thomas Barth
* @date       17.05.2012
* @note       Angelegt
* @version    1.0
*/
int sendMail (struct strMail*, char[], cMailQueue*);

/**
* @fn         void analyzeCommandline(int argc, char* argv[], struct strDaemon* strDaemon, struct strConfigs* strConfigs)
* @brief      Methode zum Analysieren der übergebenen Parameter der Kommandozeile
* @details    Wenn irgendein Parameter nicht übergeben wird, wird vom Programm ein Defaultwert eingesetzt.
*             In der Sektion "defines for the daemon" können diese eingesehen werden.
*             Wenn das Einlesen fehl schlägt, wird das Programm abgebrochen.
* @param      argc                  = int (Größe des argv[]-Arrays)
* @param      argv                  = char*[] (Array mit dem Kommandozeilenparametern)
* @param      strDaemon             = struct strDaemon* (Struktur mit Einstellungen für den Daemon)
* @param      strConfigs            = struct strConfigs* (Struktur mit den Konfigurationsdateien)
* @return     -
* @author     Thomas Barth
* @date       03.12.2011
* @note       Angelegt
* @version    1.0
*/
void analyzeCommandline(int argc, char* argv[], struct strDaemon* strDaemon, struct strConfigs* strConfigs);

/**
* @fn         void getTime(char[])
* @brief      Methode zum Ermitteln der aktuellen Zeit als Char-Array
* @details    Es wird hier die akutelle Zeit ermittelt und in ein
*             Char-Array geschrieben, welches in Hauptprogramm verwendet werden kann.
* @param      cTimeMail[]           = char[] (Array für die aktuelle Zeit)
* @return     -
* @author     Thomas Barth
* @date       20.05.2012
* @note       Angelegt
* @version    1.0
*/
void getTime (char[]);

/**
* @fn         void signal_handler(int sig)
* @brief      Methode, welche die Signale des Daemon verwaltet
* @details    Diese Methode verwaltet die Signale die der Daemon möglicherweise empfängt.
*             Dies gibt dem Daemon die Möglichkeit das Programm zu beenden wenn eine
*             kritische Situation auftritt.
* @param      sig             = int (Signalwert, welcher auftrat)
* @return     -
* @author     Thomas Barth
* @date       01.12.2011
* @note       Angelegt
* @version    1.0
*/
void signal_handler(int sig);

/**
* @fn         void free_memory (struct strStatus* strStatus, struct strConfigs* strConfigs, struct strDaemon* strDaemon, struct strMySQL* strMySQL*, struct strMail* strMail, cStatusDevice* cStatusDevices)
* @brief      Methode für die Freigabe von dynamisch allokiertem Speicher
* @details    Diese Methode gibt dynamisch allokierten Speicher frei. Sie sollte aufgerufen werden wenn das
*             Programm beendet wird.
* @param      strStatus       = struct strStatus* (Struktur mit den Statuschannel und Statusvalue)
* @param      strConfigs      = struct strConfigs* (Struktur mit den Konfigurationsdateien)
* @param      strDaemon       = struct strDaemon* (Struktur mit Einstellungen für den Daemon)
* @param      strMySQL        = struct strMySQL* (Struktur mit Einstellungen für die MySQL-Verbindung)
* @param      strMail         = struct strMail* (Struktur mit den Einstellungen für den Mailversand)
* @param      cStatusDevices  = cStatusDevices (Zeiger auf die Statusobjekte der einzelnen Geräte)
* @return     -
* @author     Thomas Barth
* @date       10.12.2011
* @note       Angelegteugt
* @version    1.0
*/
void free_memory (struct strStatus* strStatus, struct strConfigs* strConfigs, struct strDaemon* strDaemon, struct strMySQL* strMySQL, struct strMail* strMail, cStatusDevice* cStatusDevices);

#endif /* SUNNYHEADER_H_ */
