
/**
* @file            defines.h
* @brief           Dies ist eine Headerdatei welche "defines" für den SunnyConnector32 beinhaltet
* @details
*/

#ifndef DEFINES_H_
#define DEFINES_H_

/* Defines für den SunnyConnector32 */
#define YASDIINI                "yasdi.ini"                        /**< Defaultname für die yasdi.ini-Datei                                             */
#define CONFIGINI               "config.ini"                       /**< Defaultname für die config.ini-Datei                                            */
#define DEFAULT_STATUSCHANNEL   "Status"                           /**< Defaultwert für den Statuschannell                                              */
#define DEFAULT_STATUSVALUE     "Mpp"                              /**< DefaultWert für den Status wenn eingespeist wird                                */
#define DEFAULT_INTERVAL        30                                 /**< Defaultwert für das Intervall zwischen den Anfragen                             */
#define DEFAULT_DEVICECOUNT     1                                  /**< Defaultwert für die zu suchenden Wechselrichter                                 */
#define SNLENGTH                11                                 /**< Maximale Länge für die Seriennummer "2000631068"                                */
#define LOGFILELENGTH           TIMELENGTH + SNLENGTH              /**< Maximale Länge für den Logdateinamen z.B. "01_01_2011_2000631068.log"           */
#define VALUELENGTH             20                                 /**< Maximale Länge für einen Rückgabewert vom Wecheslrichter                        */
#define DEVICES                 256                                /**< Maximale Länge von Geräten die gespeichert werden können                        */
#define MYSQLENGTH              1024                               /**< Maximale Länge für einen MySQL-String                                           */

/* Defines für den Mailversand von Systemmeldungen */
#define DEFAULT_MAIL_SEND       false                              /**< Defaultwert ob Mails versendet werden sollen oder nicht                         */
#define DEFAULT_MAIL_SRV        "smtp.example.com"                 /**< Defaultmailserver für den Mailversand                                           */
#define DEFAULT_MAIL_SRV_PORT   "25"                               /**< Defaultport für den Mailserver                                                  */
#define DEFAULT_MAIL_USE_TLS    "no"                               /**< Defaultwert für die TLS-Verschlüsselung                                         */
#define DEFAULT_MAIL_USER       ""                                 /**< Defaultbenutzer für die Mailserverauthentifizierung                             */
#define DEFAULT_MAIL_PASSWORD   ""                                 /**< Defaultpasswort für die Mailserverauthentifizierung                             */
#define DEFAULT_MAIL_FROM       ""                                 /**< Defaultwert für den Absender der Mail                                           */
#define DEFAULT_MAIL_TO         ""                                 /**< Defaultwert für den Empfänger                                                   */

/* Defines für das Speichern in Dateien */
#define DEFAULT_FILE_SAVE       false                              /**< Defaultwert ob in Dateien gespeichert wird oder nicht                           */
#define DEFAULT_FILE_SEPERATOR  ";"                                /**< Defaultseperatorzeichen zwischen den Werten in den Dateien                      */
#define DEFAULT_FILE_EXT        ".log"                             /**< Defaultextention für die Logdateien                                             */

/* Defines für das Speichern in die MySQL-Datenbank */
#define DEFAULT_MYSQL_SAVE      false                              /**< Defaultwert ob in die MySQL-Datenbank gespeichert werden soll oder nicht        */
#define DEFAULT_MYSQL_HOST      "localhost"                        /**< Defaultwert für den MySQL-Servernamen                                           */
#define DEFAULT_MYSQL_PORT      "3306"                             /**< Defaultwert für den MySQL-Serverport                                            */
#define DEFAULT_MYSQL_USER      "root"                             /**< Defaultwert für den MySQL-User                                                  */
#define DEFAULT_MYSQL_PASSWORD  "mysql"                            /**< Defaultwert für das MySQL-Passwort                                              */
#define DEFAULT_MYSQL_DB        "SCA"                              /**< Defaultwert für die MySQL-Datenbank                                             */
#define DEFAULT_MYSQL_DATA      "DATEN"                            /**< Defaultwert für die Tabelle der Daten der Wechselrichter                        */
#define DEFAULT_MYSQL_SN        "SERIENNUMMERN"                    /**< Defaultwert für die Tabelle der Seriennummern der Wechselrichter                */

/* Defines für das Logging */
#define SYSLENMSG               100                                /**< Maximale Länge für eine SYSLOG-Meldung                                          */
#define ERROR                   "[ ERROR ]"                        /**< [ERROR] für eine SYSLOG-Meldung                                                 */
#define FAILURE                 "[ FAILURE ]"                      /**< [FAILURE] für eine SYSLOG-Meldung                                               */
#define WARNING                 "[ WARNING ]"                      /**< [WARNING] für eine SYSLOG-Meldung                                               */
#define OK                      "[ OK ]"                           /**< [OK] für eine SYSLOG-Meldung                                                    */

/* Defines für den Daemon */
#define DAEMON_NAME             "SunnyConnector32"                 /**< Name des Daemon                                                                 */
#define DAEMON_UMASK            027                                /**< UMASK für die Daemondateien                                                     */
#define DAEMON_UMASK_MKDIR      0770                               /**< UMASK für neue Verzeichnisse                                                    */
#define DAEMON_DIR              "/srv/SunnyConnector32"            /**< Verzeichnis für die Daemondateien                                               */
#define DAEMON_PID_FILE         "/var/run/SunnyConnector32.pid"    /**< Datei für die PID des Daemon                                                    */

/* Defines für den Mailversand */
#define MAIL_TIMESTAMP          "%d.%m.%Y %H:%M:%S"                     /**< Format für den Zeitstempel (01.01.2012 12:12:00)                                                 */
#define MAIL_TIME_LENGTH        21                                      /**< Länge des Zeitstempels ( + 1 wegen Zeilenumbruch)                          */
#define MAIL_SUB_STOP           "SunnyConnector32 Stop-Mail"            /**< Betreffzeile für die Stop-Mail                                             */
#define MAIL_SUB_SYSTEM         "SunnyConnector32 System-Mail"          /**< Betreffzeile für eine System-Mail                                          */
#define MAIL_SUB_ERROR          "SunnyConnector32 Fehler-Mail"          /**< Betreffzeile für eine Fehler-Mail                                          */
#define MAIL_HEADER             "Dies ist eine Mail Ihres SunnyConnector32.\n\nEs folgen nun die evtl. vorhandenen Meldungen:\n\n"
#define MAIL_FOOTER             "\nDiese Mail wurde automatisch generiert und ein antworten auf diese Mail ist NICHT moeglich!\n\n Programm Version:"
#define MAIL_MSG_STOP           "Der SunnyConnector32 wurde auf Grund eines Fehlers gestoppt!!!\n\n Pruefen Sie umgehend die Logdatei \"/var/cache/daemon.log\" auf mögliche Fehler.\n\n Beheben Sie diese und starten Sie den SunnyConnector32 erneut!"
#define MAIL_SUNNY_STOP_ERROR   "SunnyConnector wurde beendet!"         /**< Meldung für eine Error-Mail das der SunnyConnector beendet wurde           */

/* ExitCodes für den Daemon */
#define DAEMON_EXIT_SUCCESS     0                                  /**< ExitCode für eine erfolgreiche Abarbeitung                                      */
#define DAEMON_EXIT_ERROR       1                                  /**< ExitCode wenn ein Fehler auftrat                                                */
#define DAEMON_EXIT_BADALLOC    2                                  /**< ExitCode wenn ein BadAlloc auftrat                                              */
#define DAEMON_EXIT_ERROR_CMD   3                                  /**< ExitCode wenn ein Fehler bei den CMD-Parameter auftrat                          */
#define DAEMON_EXIT_ERROR_LOG   4                                  /**< ExitCode wenn ein Fehler auftratt und man in den Logs nachsehen soll            */
#define DAEMON_EXIT_YASDI       11                                 /**< ExitCode wenn die YASDI-INI nicht gelesen werden konnte                         */
#define DAEMON_EXIT_INIT_YASDI  12                                 /**< ExitCode wenn die YASDI-Initialisierung fehlschlägt                             */
#define DAEMON_EXIT_CONFIG      21                                 /**< ExitCode wenn die CONFIG-INI nicht gelesen werden konnte                        */
#define DAEMON_EXIT_SIGTERM     31                                 /**< ExitCode wenn Daemon per start-stop-daemon gestoppt wurde                       */
#define DAEMON_EXIT_SIGQUIT     33                                 /**< ExitCode wenn der Kernel ein Quitsignal schickt                                 */
#define DAEMON_EXIT_SIGFPE      34                                 /**< ExitCode wenn ein Fliesskommaberechungsfehler auftritt                          */
#define DAEMON_EXIT_SIGILL      35                                 /**< ExitCode wenn eine fehlerhafte Instruktions ausgeführt wird                     */
#define DAEMON_EXIT_SIGSEGV     36                                 /**< ExitCode wenn auf einen Speicherbereich ausserhalb der Programmspeichers zugegriffen wird */

#endif /* DEFINES_H_ */
