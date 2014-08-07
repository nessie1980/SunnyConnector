/**
* @file            main.cpp
* @brief           Dies ist die "main" des Programms "SunnyConnector32".
* @details
*/

/* Includes */
#include "defines.h"
#include "sunnyconnector.h"

/* Namespaces */
using namespace std;

/**
* @fn         int main(int argc, char *argv[])
* @brief      Einstiegspunkt für das Programm
* @details    In der main.cpp wird zu erst der Daemon initialisiert, d.h. es werden Signal-Handler eingestellt,
*             der Logger eingestellt und alle nötigen Umgebungsvariablen gesetzt.
*             Anschliessend wird die config.ini eingelesen und geprüft ob alle nötigen Angaben eingelesen wurden.
*             Nun wird das Programm in eine while-Schleife geschickt, in welcher der YASDI-Treiber initialisiert wird
*             und danach Daten von den Wechselrichtern holt.
* @param      argc             = int (Anzahl der Übergabeparameter, der Programmname ist ebenfalls ein Parameter)
* @param      argv             = char* [] (Array, welches die Übergabeparameter beinhaltet)
* @return     int              = int (Rückgabewert, welche an den Elternprozess übergeben wird)
* @author     Thomas Barth
* @date       01.12.2011
* @note       Angelegt
* @version    1.0
*/
int main(int argc, char *argv[])
{
    /* Variablen für das Hauptprogramm "SunnyConnector" */
    bool bFirstInitFlag = false;                                /* Flag, ob die erste Initialisierung des YASDI-Treiber vorgenommen wurde   */
    struct strTimers strTimers;                                 /* Struktur mit diversen Timern                                             */
    struct strStatus strStatus;                                 /* Struktur mit dem Statuschannel und dem Statusvalue                       */
    struct strFile strFile;                                     /* Struktur mit Angaben zur Speicherung in Dateien                          */
    struct strMySQL strMySQL;                                   /* Struktur mit Angaben zur Speicherung in eine MySQL-Server                */
    struct strConfigs strConfigs;                               /* Struktur mit Angaben zu den Konfigurationsdateien                        */
    struct strMail strMail;                                     /* Struktur mit den Angaben für den Mailversand                             */
    cStatusDevice* cStatusDevices = NULL;                       /* Zeiger vom Typ "cStatusDevice", welcher später auf ein Array zeigt       */
    DWORD dwBusDriverCounter = 0;                               /* Anzahl der Bus-Treiber, sprich wieviel serielle Geräte verwendet werden  */
    DWORD dwDeviceCounter = 0;                                  /* Anzahl der gefunden Wechselrichter                                       */
    DWORD dwDeviceSNCounter = 0;                                /* Anzahl der gefunden Wechselrichter laut Seriennummer                     */
    DWORD dwDeviceHandle[DEVICES];                              /* Array für 256 Gerätehandles                                              */
    char cVersionString[16] = "";                               /* Array für den Versionsstring                                             */
    char cMajorString[6] = "";                                  /* Array für die MAJOR-Version                                              */
    char cMinorString[4] = "";                                  /* Array für die MINOR-Version                                              */
    char cBuildString[5] = "";                                  /* Array für die BUILD-Version                                              */

    /* Variables für den Daemon */
    struct strDaemon strDaemon;                                 /* Struktur für Angaben des Daemon                                          */
    mode_t iOldUmask = 0;                                       /* Buffer für die "alte" UMASK beim neuen Setzen                            */
    pid_t iPid;                                                 /* Prozess-ID                                                               */
    pid_t iSid;                                                 /* Session-ID                                                               */
    fstream fPidUpdate;                                         /* Dateistream für das Updaten der PID-DateiFilestream                      */

    /* Variablen für den Mailversand */ //TODO
    struct tm* tmCurrentTime;
    struct tm tmLastTime = {0};
    char cTimeMail[MAIL_TIME_LENGTH] = "";                      /* Zeitstring für den Mail- bzw. Messagezeitstempel (2012-01-01 12:12:00)    */
    bool bMailSendFlag = false;

    /* Zeit in eine TimeStrukturkopie schreiben */
    mode_t dwCurrentTime = time (0);                                /* Aktuelle Zeit ermitteln                                               */
    tmCurrentTime = localtime ((const long int*) &dwCurrentTime);   /* Aktuelle Zeit in die Stuktur schreiben                                */
    tmLastTime = *tmCurrentTime;                                    /* Aktuelle Zeitstruktur kopieren                                        */

    /* Stop-Error-Mail erstellen */
    cMailMsg cMsgStop((char*)MAIL_MSG_STOP);
    cMailQueue cMailQueueStop ((char*)MAIL_SUB_STOP);
    cMailQueueStop.addMsg(&cMsgStop);

    /* Versionsstring erstellen */
    sprintf (cMajorString, "% 4d.", MAJOR);
    sprintf (cMinorString, "%02d.", MINOR);
    sprintf (cBuildString, "%03d", BUILD);
    strcpy (cVersionString, cMajorString);
    strcat (cVersionString, cMinorString);
    strcat (cVersionString, cBuildString);

    /* Einstellen des Signalhandlings */
    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);

    /* Einstellen des Loggings */
    openlog(DAEMON_NAME, LOG_CONS | LOG_PID, LOG_DAEMON);

    /* Programm als Daemon starten, falls kein Argument "D" auf der Kommandozeile übergeben wird */
    strDaemon.iDaemonize = 1;

    /* Analysieren der übergebenen Parameter der Kommandozeile */
    analyzeCommandline(argc, argv, &strDaemon, &strConfigs);

    syslog(LOG_INFO, "%s Daemon wird gestartet", strDaemon.cDaemonName);

    /* Versionsnummer ausgeben */
    syslog(LOG_INFO, "Version des %s:%s", strDaemon.cDaemonName, cVersionString);

    /* Schliessen des Loggings zum Neueinstellen */
    closelog ();

    /* Daemonize */
    syslog(LOG_INFO, "Daemonize: %d", strDaemon.iDaemonize);

    /* Erstellen des Daemons für das Programm */
    if (strDaemon.iDaemonize)
    {
        /* Einstellen des Loggings - siehe SETLOGMASK(3) */
        if (strDaemon.iDaemonize == 0)
        {
            setlogmask(LOG_UPTO(LOG_DEBUG));
            openlog(strDaemon.cDaemonName, LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_USER);
        }
        else
            openlog(strDaemon.cDaemonName, LOG_CONS | LOG_PID, LOG_DAEMON);

        syslog(LOG_INFO, "%s Daemon-Prozess wird gestartet", strDaemon.cDaemonName);

        if(cMsgStop.getInitResult() < 1 || cMailQueueStop.getInitResult() < 1)
        {
            syslog(LOG_ERR, "Stop-Errormessage konnte nicht angelegt werden");
            exit(DAEMON_EXIT_ERROR_LOG);
        }

        /* Abspaltung vom Elternprozess */
        iPid = fork();
        if (iPid < 0)
        {
            syslog(LOG_ERR,"%s%*s\n", "Eltern-Prozess konnte nicht erstellen werden", SYSLENMSG - strlen("Eltern-Prozess konnte nicht erstellen werden"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);
            free_memory(&strStatus, &strConfigs, &strDaemon, &strMySQL, &strMail, cStatusDevices);
            exit(DAEMON_EXIT_ERROR_LOG);
        }

        /* Wenn wir eine "gute" PID für den Kindprozess erhalten kann der Elternprozess beendet werden. */
        if (iPid > 0)
        {
            exit(DAEMON_EXIT_SUCCESS);
        }

        /* Ändern der UMASK für das Erzeugen von Dateien, wobei die alte UMASK gespeichert wird */
        syslog(LOG_INFO, "Setzen der UMASK: %03o", strDaemon.iDaemonUmask);
        iOldUmask = umask(strDaemon.iDaemonUmask);

        /* Erstellen einer neuen Session-ID für den Kindprozess */
        iSid = setsid();
        if (iSid < 0)
        {
            syslog(LOG_ERR,"%s%*s\n", "Session fuer den Kind-Prozess konnte nicht erstellen werden", SYSLENMSG - strlen("Session fuer den Kind-Prozess konnte nicht erstellen werden"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);
            free_memory(&strStatus, &strConfigs, &strDaemon, &strMySQL, &strMail, cStatusDevices);
            exit(DAEMON_EXIT_ERROR_LOG);
        }

        /* Erstellen des Arbeitsverzeichnisses für den Daemon bzw. Programm, wobei hier die alte UMASK gespeichert wird */
        iOldUmask = umask(0);
        syslog(LOG_INFO, "Setzen der UMASK-MkDir: %04o", strDaemon.iDaemonUmaskMkdir);
        mkdir(strDaemon.cDaemonDir,strDaemon.iDaemonUmaskMkdir);
        umask(iOldUmask);

        /* Ins Arbeitsverzeichnis des Daemon wechseln */
        syslog(LOG_INFO, "Wechsel ins Arbeitsverzeichnis: %s", strDaemon.cDaemonDir);
        if ((chdir(strDaemon.cDaemonDir) < 0))
        {
            syslog(LOG_ERR,"%s%*s\n", "Arbeitsverzeichnis konnte nicht gewechselt werden", SYSLENMSG - (strlen("Arbeitsverzeichnis konnte nicht gewechselt werden")), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);
            free_memory(&strStatus, &strConfigs, &strDaemon, &strMySQL, &strMail, cStatusDevices);
            exit(DAEMON_EXIT_ERROR_LOG);
        }

        /* Die Standardkanäle schliessen */
        /* Da der Daemon keine Ausgabe auf diesen erzeugen soll, sondern nur per "SYSLOG" Meldungen schreiben soll */
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        /* Updaten der PID-Datei, welche vom "start-stop-daemon" erzeugt wurde */
        iPid = getpid();
        syslog(LOG_INFO, "Update der PID-Datei: %s", strDaemon.cDaemonPidFile);
        fPidUpdate.open(strDaemon.cDaemonPidFile, ios::out|ios::trunc);
        if (fPidUpdate.is_open())
        {
            fPidUpdate << iPid;
            fPidUpdate.close();
        }
        else
        {
            syslog(LOG_ERR,"%s%*s\n", "PID-Datei konnte nicht geupdatet werden", SYSLENMSG - (strlen("PID-Datei konnte nicht geupdatet werden")), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);
            free_memory(&strStatus, &strConfigs, &strDaemon, &strMySQL, &strMail, cStatusDevices);
            exit(DAEMON_EXIT_ERROR_LOG);
        }

        /* Ein paar Sekunden warten bis der Daemon gestartet wurde */
        sleep(3);
    }

    syslog(LOG_INFO, "YASDI.INI: %s", strConfigs.cYasdiINI);
    syslog(LOG_INFO, "CONFIG.INI: %s", strConfigs.cConfigINI);

    /* Erzeugen eines config-Objekts, welches die CONFIG.INI einliest */
    cConfig cConfig(strConfigs.cConfigINI);

    /* Erzeugen eines mailqueue-Objekts, welches Systemmeldungen aufnimmt */
    cMailQueue cMailQueueSystem((char*)MAIL_SUB_SYSTEM);

    /* Erzeugen eines mailqueue-Objekts, welches Errormeldungen aufnimmt */
    cMailQueue cMailQueueError((char*)MAIL_SUB_ERROR);

    /****************************/
    /* Haupt-Progamm des Daemon */
    /****************************/

    /* Prüfen ob das Einlesen der CONFIG.INI erfolgreich war */
    if (cConfig.getInitResult() == 0)
    {
        /* Prüfen ob alles nötigen Konfigurationen vorhanden sind */
        if (initSunnyConnector(cConfig, &strTimers, &strStatus, &strFile, &strMySQL, &strConfigs, &strDaemon, &strMail, cStatusDevices))
        {
            while (1)
            {
                if (dwDeviceSNCounter < (unsigned int)cConfig.getSegmentValuesSize("[SERIALNUMBERS]"))
                {
                    /* Function für die YASDI-Initialisierung */
                    if ( initYasdi(&dwBusDriverCounter, &dwDeviceCounter, &dwDeviceSNCounter, dwDeviceHandle, strConfigs.cYasdiINI, &bFirstInitFlag, cConfig, &cStatusDevices) == YE_UNKNOWN_HANDLE)
                    {
                        syslog(LOG_ERR,"%s%*s\n", "YASDI-INI-Datei existiert nicht oder ist nicht lesbar", SYSLENMSG - strlen("YASDI-INI-Datei existiert nicht oder ist nicht lesbar"), ERROR);
                        syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

                        try
                        {
                            /* Mail versenden */
                            cMailQueueError.addMsg(new cMailMsg((char*)cTimeMail));
                            cMailQueueError.addMsg(new cMailMsg((char*)"YASDI-INI-Datei existiert nicht oder ist nicht lesbar\n\n"));
                            cMailQueueError.addMsg(new cMailMsg((char*)MAIL_SUNNY_STOP_ERROR));
                            sendMail(&strMail, cVersionString, &cMailQueueError);
                        }
                        catch (...)
                        {
                            sendMail(&strMail, cVersionString, &cMailQueueStop);
                        }

                        free_memory(&strStatus, &strConfigs, &strDaemon, &strMySQL, &strMail, cStatusDevices);
                        exit(DAEMON_EXIT_YASDI);
                    }
                }

                /* Daten von einem oder mehreren Wechselrichtern lesen und anschliessend in eine Textdatei bzw MySQL-Datenbank schreiben */
                getYasdiData(dwDeviceHandle, &dwDeviceCounter, cConfig, &strConfigs, &strDaemon,  &cStatusDevices, &strStatus, &strFile, &strMySQL, &strMail, &cMailQueueError, &cMailQueueSystem, &cMailQueueStop, cVersionString);
                sleep(strTimers.iTimerData);

                /* Mail versenden */
                if ((tmCurrentTime->tm_min >= 0) && (tmCurrentTime->tm_min < 59) && bMailSendFlag == false)
                {
                    sendMail(&strMail, cVersionString, &cMailQueueSystem);

                    dwCurrentTime = time(0);
                    tmCurrentTime = localtime ((const long int*) &dwCurrentTime);
                    tmLastTime = *tmCurrentTime;
                    bMailSendFlag = true;
                }

                /* Prüfen ob das Intervall, welches konfiguriert wurde abgelaufen ist */
                //if (tmCurrentTime->tm_hour != tmLastTime.tm_hour)
                if (tmCurrentTime->tm_mday != tmLastTime.tm_mday)
                    bMailSendFlag = false;

                dwCurrentTime = time(0);
                tmCurrentTime = localtime ((const long int*) &dwCurrentTime);
            }

            syslog(LOG_INFO, "YASDI-driver wird beendet");
            yasdiMasterShutdown();
        }
    }
    else
    {
        syslog(LOG_ERR,"%s%*s\n", "CONFIG-INI-Datei existiert nicht oder ist nicht lesbar", SYSLENMSG - strlen("CONFIG-INI-Datei existiert nicht oder ist nicht lesbar"), ERROR);
        syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

        try
        {
            /* Mail versenden */
            cMailQueueError.addMsg(new cMailMsg((char*)cTimeMail));
            cMailQueueError.addMsg(new cMailMsg((char*)"CONFIG-INI-Datei existiert nicht oder ist nicht lesbar\n\n"));
            cMailQueueError.addMsg(new cMailMsg((char*)MAIL_SUNNY_STOP_ERROR));
            sendMail(&strMail, cVersionString, &cMailQueueError);
        }
        catch (...)
        {
            sendMail(&strMail, cVersionString, &cMailQueueStop);
        }

        free_memory(&strStatus, &strConfigs, &strDaemon, &strMySQL, &strMail, cStatusDevices);
        exit(DAEMON_EXIT_CONFIG);
    }

    syslog(LOG_INFO, "%s Daemon wird beendet", strDaemon.cDaemonName);

    free_memory(&strStatus, &strConfigs, &strDaemon, &strMySQL, &strMail, cStatusDevices);

    exit(DAEMON_EXIT_SUCCESS);
}
