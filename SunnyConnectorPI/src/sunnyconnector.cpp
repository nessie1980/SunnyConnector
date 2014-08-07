
/**
* @file            sunnyconnector.cpp
* @brief           CPP-Datei für die Headerdatei "sunnyconnector.h"
* @details
*/
/* Includes */
#include "sunnyconnector.h"

/* Methode, welche die Benutzung des Programm an zeigt */
void PrintUsage(int argc, char *argv[])
{
    /* Prüfen ob mehr als ein Argument angegeben wurde. */
    /* Das erste Argument ist immer der Programmname selbst */
    if (argc >=1)
    {
        printf("Benutzung: %s -c Datei -d Verzeichnis -n Name -m UMASK -p Datei -u UMASK -y Datei -D -h\n", argv[0]);
        printf("  Options:\n");
        printf("      -c\tVerzeichnis mit dem Name der CONFIG.INI-Datei. (z.B. /home/test/config.ini)\n");
        printf("      -d\tVerzeichnis des Arbeitsverzeichnisses des Daemons. (z.B. /srv/SunnyConnector32)\n");
        printf("      -n\tName des Daemon. (z.B. SunnyConnector32)\n");
        printf("      -m\tUMASK für neue Verzeichnisse des Daemons. (z.B. 0770)\n");
        printf("      -p\tVerzeichnis mit dem Name der PID-Datei des Daemons. (z.B. /var/run/SunnyConnector32.pid)\n");
        printf("      -u\tUMASK für neue Dateien des Daemons. (z.B. 027 der Wert ist die Invertierung von 720)\n");
        printf("      -y\tVerzeichnis mit dem Name der YASDI.INI-file. (z.B. /home/test/yasdi.ini)\n");
        printf("      -D\tNicht als Daemon starten.\n");
        printf("      -h\tHilfe anzeigen.\n");
        printf("\n");
    }
}

/* Methode, welche alle Variablen setzt die nötig sind und prüft ob alle Konfigurationsdaten vorhanden sind */
int initSunnyConnector(cConfig& cConfig, struct strTimers* strTimers, struct strStatus* strStatus, struct strFile* strFile, struct strMySQL* strMySQL, struct strConfigs* strConfigs, struct strDaemon* strDaemon, struct strMail* strMail, cStatusDevice* cStatusDevices)
{
    /* Variablen */
    int iRes = 0;                                       /* Ergebniswert                                                             */

    MYSQL* myConnection = NULL;                         /* MySQL-Connection                                                         */
    MYSQL_RES *myResult = NULL;                         /* MySQL-Result                                                             */
    char cResultMySQL[MYSQLENGTH] = "";                 /* Ergebnisstring der die Daten zum Speicher in einen MySQL-Server aufnimmt */

    /* Lesen des Updateintervals aus dem Config-Objekts.
     *  Diese Intervall bestimmt welche Zeit zwischen den Datenanfragen an die Wecchselrichter liegt */
    if (cConfig.getSegmentValueByKeyword("[TIMERS]","INTERVALDATA") != NULL)
    {
        stringstream ss(cConfig.getSegmentValueByKeyword("[TIMERS]","INTERVALDATA"));
        if ( ss >> strTimers->iTimerData)
        {
            syslog(LOG_INFO, "Pollinginterval: %d", strTimers->iTimerData);
        }
        else
        {
            syslog(LOG_WARNING,"%s%*s\n", "INTERVALDATA konnte nicht von der CONFIG.INI gelesen werden", SYSLENMSG - strlen("INTERVALDATA konnte nicht von der CONFIG.INI gelesen werden"), WARNING);
            syslog(LOG_INFO, "Default-Wert wird benutzt: %d", strTimers->iTimerData);
        }
    }
    else
    {
        strTimers->iTimerData = DEFAULT_INTERVAL;
        syslog(LOG_WARNING,"%s%*s\n", "INTERVALDATA konnte nicht von der CONFIG.INI gelesen werden", SYSLENMSG - strlen("INTERVALDATA konnte nicht von der CONFIG.INI gelesen werden"), WARNING);
        syslog(LOG_INFO, "Default-Wert wird benutzt: %d", strTimers->iTimerData);
    }

    /* Lesen des Statuschannel, welche zur Statusbestimmung des Wechselrichters hergenommen wird. */
    if (cConfig.getSegmentValueByKeyword("[STATUS]","STATUSCHANNEL") != NULL)
    {
        /* Speicher für den STATUSCHANNEL allokieren */
        try
        {
            strStatus->cStatusChannel = new char [strlen(cConfig.getSegmentValueByKeyword("[STATUS]","STATUSCHANNEL")) + 1];
            strcpy (strStatus->cStatusChannel, cConfig.getSegmentValueByKeyword("[STATUS]","STATUSCHANNEL"));
            syslog(LOG_INFO, "Status-Channel: %s", strStatus->cStatusChannel);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cStatusChannel\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cStatusChannel\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }
    else
    {
        /* Speicher für den STATUSCHANNEL allokieren */
        try
        {
            strStatus->cStatusChannel = new char [strlen(DEFAULT_STATUSCHANNEL) + 1];
            strcpy (strStatus->cStatusChannel, DEFAULT_STATUSCHANNEL);
            syslog(LOG_WARNING,"%s%*s\n", "STATUSCHANNEL konnte nicht von der CONFIG.INI gelesen werden", SYSLENMSG - strlen("STATUSCHANNEL konnte nicht von der CONFIG.INI gelesen werden"), WARNING);
            syslog(LOG_INFO, "Default-Wert wird benutzt : %s", strStatus->cStatusChannel);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cStatusChannel\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cStatusChannel\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }

    /* Lesen des Statuswerts, welcher festlegt ob ein Wechselrichter einspeist oder nicht */
    if (cConfig.getSegmentValueByKeyword("[STATUS]","STATUSVALUE") != NULL)
    {
        /* Speicher für den STATUSVALUE allokieren */
        try
        {
            strStatus->cStatusValue = new char [strlen(cConfig.getSegmentValueByKeyword("[STATUS]","STATUSVALUE")) + 1];
            strcpy (strStatus->cStatusValue, cConfig.getSegmentValueByKeyword("[STATUS]","STATUSVALUE"));
            syslog(LOG_INFO, "Status-Value: %s", strStatus->cStatusValue);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cStatusValue\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cStatusValue\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }
    else
    {
        /* Speicher für den STATUSVALUE allokieren */
        try
        {
            strStatus->cStatusValue = new char [strlen(DEFAULT_STATUSVALUE) + 1];
            strcpy (strStatus->cStatusValue, DEFAULT_STATUSVALUE);
            syslog(LOG_WARNING,"%s%*s\n", "STATUSVALUE konnte nicht von der CONFIG.INI gelesen werden", SYSLENMSG - strlen("STATUSVALUE konnte nicht von der CONFIG.INI gelesen werden"), WARNING);
            syslog(LOG_INFO, "Default-Wert wird benutzt: %s", strStatus->cStatusValue);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cStatusValue\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cStatusValue\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }

    /* Lesen des E-Mailversendewerts, welcher festlegt ob der SunnyConnector32 Systemmeldungen per Mail versendet */
    if (cConfig.getSegmentValueByKeyword("[MAIL]","USE") != NULL)
    {
            if (!strcmp("0", cConfig.getSegmentValueByKeyword("[MAIL]","USE")))
            {
                strMail->bMail = false;
                syslog(LOG_INFO, "Mailversand: %d", strMail->bMail);
            }
            else
            {
                strMail->bMail = true;
                syslog(LOG_INFO, "Mailversand: %d", strMail->bMail);
            }
    }
    else
    {
        strMail->bMail = DEFAULT_MAIL_SEND;
        syslog(LOG_WARNING,"%s%*s\n", "USE konnte nicht von der CONFIG.INI gelesen werden", SYSLENMSG - strlen("USE konnte nicht von der CONFIG.INI gelesen werden"), WARNING);
        syslog(LOG_INFO, "Default-Wert wird benutzt: %d", strMail->bMail);
    }

    /* Lesen des Mailservernamens über welchen die Systemmeldungen verschickt werden sollen */
    if (cConfig.getSegmentValueByKeyword("[MAIL]","SERVER") != NULL)
    {
        /* Speicher für den SERVER allokieren */
        try
        {
            strMail->cServer = new char [strlen(cConfig.getSegmentValueByKeyword("[MAIL]","SERVER")) + 1];
            strcpy (strMail->cServer, cConfig.getSegmentValueByKeyword("[MAIL]","SERVER"));
            syslog(LOG_INFO, "Mail-Server: %s", strMail->cServer);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cServer\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cServer\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }
    else
    {
        /* Speicher für den SERVER allokieren */
        try
        {
            strMail->cServer = new char [strlen(DEFAULT_MAIL_SRV) + 1];
            strcpy (strMail->cServer, DEFAULT_MAIL_SRV);
            syslog(LOG_WARNING,"%s%*s\n", "SERVER konnte nicht von der CONFIG.INI gelesen werden", SYSLENMSG - strlen("SERVER konnte nicht von der CONFIG.INI gelesen werden"), WARNING);
            syslog(LOG_INFO, "Default-Wert wird benutzt: %s", strMail->cServer);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cServer\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cServer\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }

    /* Lesen des Mailserverports über welchen der Mailserver kommuniziert */
    if (cConfig.getSegmentValueByKeyword("[MAIL]","PORT") != NULL)
    {
        /* Speicher für den PORT allokieren */
        try
        {
            strMail->cPort = new char [strlen(cConfig.getSegmentValueByKeyword("[MAIL]","PORT")) + 1];
            strcpy (strMail->cPort, cConfig.getSegmentValueByKeyword("[MAIL]","PORT"));
            syslog(LOG_INFO, "Mail-Serverport: %s", strMail->cPort);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cPort\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cPort\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }
    else
    {
        /* Speicher für den PORT allokieren */
        try
        {
            strMail->cPort = new char [strlen(DEFAULT_MAIL_SRV_PORT) + 1];
            strcpy (strMail->cPort, DEFAULT_MAIL_SRV_PORT);
            syslog(LOG_WARNING,"%s%*s\n", "PORT konnte nicht von der CONFIG.INI gelesen werden", SYSLENMSG - strlen("PORT konnte nicht von der CONFIG.INI gelesen werden"), WARNING);
            syslog(LOG_INFO, "Default-Wert wird benutzt: %s", strMail->cPort);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cPort\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cPort\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }

    /* Lesen des TLS-Verschlüsselungswert, soll TLS-Verschlüsselung verwendet werden */
    if (cConfig.getSegmentValueByKeyword("[MAIL]","TLS") != NULL)
    {
        /* Speicher für den TLS-Wert allokieren */
        try
        {
            strMail->cTls = new char [strlen(cConfig.getSegmentValueByKeyword("[MAIL]","TLS")) + 1];
            strcpy (strMail->cTls, cConfig.getSegmentValueByKeyword("[MAIL]","TLS"));
            syslog(LOG_INFO, "TLS-Verschlüsselung: %s", strMail->cTls);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cTls\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cTls\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }
    else
    {
        /* Speicher für den TLS-Wert allokieren */
        try
        {
            strMail->cTls = new char [strlen(DEFAULT_MAIL_USE_TLS) + 1];
            strcpy (strMail->cTls, DEFAULT_MAIL_USE_TLS);
            syslog(LOG_WARNING,"%s%*s\n", "TLS konnte nicht von der CONFIG.INI gelesen werden", SYSLENMSG - strlen("TLS konnte nicht von der CONFIG.INI gelesen werden"), WARNING);
            syslog(LOG_INFO, "Default-Wert wird benutzt: %s", strMail->cTls);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cTls\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cTls\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }

    /* Lesen der Absenderadresse über welchee der Mailserver versenden soll */
    if (cConfig.getSegmentValueByKeyword("[MAIL]","FROM") != NULL)
    {
        /* Speicher für den FROM allokieren */
        try
        {
            strMail->cFrom = new char [strlen(cConfig.getSegmentValueByKeyword("[MAIL]","FROM")) + 1];
            strcpy (strMail->cFrom, cConfig.getSegmentValueByKeyword("[MAIL]","FROM"));
            syslog(LOG_INFO, "Absenderadresse: %s", strMail->cFrom);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cFrom\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cFrom\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }
    else
    {
        /* Speicher für den FROM allokieren */
        try
        {
            strMail->cFrom = new char [strlen(DEFAULT_MAIL_FROM) + 1];
            strcpy (strMail->cFrom, DEFAULT_MAIL_FROM);
            syslog(LOG_WARNING,"%s%*s\n", "FROM konnte nicht von der CONFIG.INI gelesen werden", SYSLENMSG - strlen("FROM konnte nicht von der CONFIG.INI gelesen werden"), WARNING);
            syslog(LOG_INFO, "Default-Wert wird benutzt: %s", strMail->cFrom);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cFrom\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cFrom\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }

    /* Lesen der Empfängeradressen, welche die Mails empfangen sollen */
    if (cConfig.getSegmentValueByKeyword("[MAIL]","TO") != NULL)
    {
        /* Speicher für den TO allokieren */
        try
        {
            strMail->cTo = new char [strlen(cConfig.getSegmentValueByKeyword("[MAIL]","TO")) + 1];
            strcpy (strMail->cTo, cConfig.getSegmentValueByKeyword("[MAIL]","TO"));
            syslog(LOG_INFO, "Empfaengeraddresse/n: %s", strMail->cTo);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cTo\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cTo\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }
    else
    {
        /* Speicher für den TO allokieren */
        try
        {
            strMail->cTo = new char [strlen(DEFAULT_MAIL_TO) + 1];
            strcpy (strMail->cTo, DEFAULT_MAIL_TO);
            syslog(LOG_WARNING,"%s%*s\n", "TO konnte nicht von der CONFIG.INI gelesen werden", SYSLENMSG - strlen("TO konnte nicht von der CONFIG.INI gelesen werden"), WARNING);
            syslog(LOG_INFO, "Default-Wert wird benutzt: %s", strMail->cTo);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cTo\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cTo\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }

    /* Lesen des Mailbenutzer für die Authentifizierung */
    if (cConfig.getSegmentValueByKeyword("[MAIL]","USER") != NULL)
    {
        /* Speicher für den USER allokieren */
        try
        {
            strMail->cUser = new char [strlen(cConfig.getSegmentValueByKeyword("[MAIL]","USER")) + 1];
            strcpy (strMail->cUser, cConfig.getSegmentValueByKeyword("[MAIL]","USER"));
            syslog(LOG_INFO, "Mail-Benutzer: %s", strMail->cUser);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cUser\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cUser\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }
    else
    {
        /* Speicher für den USER allokieren */
        try
        {
            strMail->cUser = new char [strlen(DEFAULT_MAIL_USER) + 1];
            strcpy (strMail->cUser, DEFAULT_MAIL_USER);
            syslog(LOG_WARNING,"%s%*s\n", "USER konnte nicht von der CONFIG.INI gelesen werden", SYSLENMSG - strlen("USER konnte nicht von der CONFIG.INI gelesen werden"), WARNING);
            syslog(LOG_INFO, "Default-Wert wird benutzt: %s", strMail->cUser);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cUser\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cUser\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }

    /* Lesen des Mailbenutzerpassworts, über welches  sich der Benutzer am Mailserver anmeldet */
    if (cConfig.getSegmentValueByKeyword("[MAIL]","PW") != NULL)
    {
        /* Speicher für das PW allokieren */
        try
        {
            strMail->cPassword = new char [strlen(cConfig.getSegmentValueByKeyword("[MAIL]","PW")) + 1];
            strcpy (strMail->cPassword, cConfig.getSegmentValueByKeyword("[MAIL]","PW"));
            if (strMail->bMail)
                syslog(LOG_INFO, "Mail-Passwort: %s", "****");
            else
                syslog(LOG_INFO, "Mail-Passwort:");
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cPassword\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cPassword\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }
    else
    {
        /* Speicher für das PW allokieren */
        try
        {
            strMail->cPassword = new char [strlen(DEFAULT_MAIL_PASSWORD) + 1];
            strcpy (strMail->cPassword, DEFAULT_MAIL_PASSWORD);
            syslog(LOG_WARNING,"%s%*s\n", "PW konnte nicht von der CONFIG.INI gelesen werden", SYSLENMSG - strlen("PW konnte nicht von der CONFIG.INI gelesen werden"), WARNING);
            if (strMail->bMail)
                syslog(LOG_INFO, "MySQL-Passwort: %s", "****");
            else
                syslog(LOG_INFO, "MySQL-Passwort:");
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cPassword\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cPassword\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }

    /* Lesen des Dateispeicherwerts, welcher festlegt ob die Daten in Dateien gespeichert werden sollen oder nicht */
    if (cConfig.getSegmentValueByKeyword("[FILE]","USE") != NULL)
    {
            if (!strcmp("0", cConfig.getSegmentValueByKeyword("[FILE]","USE")))
            {
                strFile->bSave = false;
                syslog(LOG_INFO, "Datei-Speicherung: %d", strFile->bSave);
            }
            else
            {
                strFile->bSave = true;
                syslog(LOG_INFO, "Datei-Speicherung: %d", strFile->bSave);
            }
    }
    else
    {
        strFile->bSave = DEFAULT_FILE_SAVE;
        syslog(LOG_WARNING,"%s%*s\n", "USE konnte nicht von der CONFIG.INI gelesen werden", SYSLENMSG - strlen("USE konnte nicht von der CONFIG.INI gelesen werden"), WARNING);
        syslog(LOG_INFO, "Default-Wert wird benutzt: %d", strFile->bSave);
    }

    /* Lesen des Seperatorzeichens, welches zwischen den einzeln Werten in einer Datei steht. */
    if (cConfig.getSegmentValueByKeyword("[FILE]","SEPERATOR") != NULL)
    {
        /* Speicher für den SEPERATOR allokieren */
        try
        {
            strFile->cSeperatorFile = new char [strlen(cConfig.getSegmentValueByKeyword("[FILE]","SEPERATOR")) + 1];
            strcpy (strFile->cSeperatorFile, cConfig.getSegmentValueByKeyword("[FILE]","SEPERATOR"));
            syslog(LOG_INFO, "Seperator: %s", strFile->cSeperatorFile);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cSeperator\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cSeperator\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }
    else
    {
        /* Speicher für den SEPERATOR allokieren */
        try
        {
            strFile->cSeperatorFile = new char [strlen(DEFAULT_FILE_SEPERATOR) + 1];
            strcpy (strFile->cSeperatorFile, DEFAULT_FILE_SEPERATOR);
            syslog(LOG_WARNING,"%s%*s\n", "SEPERATOR konnte nicht von der CONFIG.INI gelesen werden", SYSLENMSG - strlen("SEPERATOR konnte nicht von der CONFIG.INI gelesen werden"), WARNING);
            syslog(LOG_INFO, "Default-Wert wird benutzt: %s", strFile->cSeperatorFile);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cSeperator\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cSeperator\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }

    /* Lesen der Dateierweiterung, welche für die Logdateien verwendet wird. */
    if (cConfig.getSegmentValueByKeyword("[FILE]","EXT") != NULL)
    {
        /* Speicher für den EXT allokieren */
        try
        {
            strFile->cExt = new char [strlen(cConfig.getSegmentValueByKeyword("[FILE]","EXT")) + 1];
            strcpy (strFile->cExt, cConfig.getSegmentValueByKeyword("[FILE]","EXT"));
            syslog(LOG_INFO, "Dateierweiterung: %s", strFile->cExt);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cExt\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cExt\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }
    else
    {
        /* Speicher für den EXT allokieren */
        try
        {
            strFile->cExt = new char [strlen(DEFAULT_FILE_EXT) + 1];
            strcpy (strFile->cExt, DEFAULT_FILE_EXT);
            syslog(LOG_WARNING,"%s%*s\n", "EXT konnte nicht von der CONFIG.INI gelesen werden", SYSLENMSG - strlen("EXT konnte nicht von der CONFIG.INI gelesen werden"), WARNING);
            syslog(LOG_INFO, "Default-Wert wird benutzt: %s", strFile->cExt);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cExt\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cExt\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }

    /* Lesen des Dateispeicherwerts, welcher festlegt ob die Daten in einen MySQL-Server gespeichert werden sollen oder nicht */
    if (cConfig.getSegmentValueByKeyword("[MYSQL]","USE") != NULL)
    {
            if (!strcmp("0", cConfig.getSegmentValueByKeyword("[MYSQL]","USE")))
            {
                strMySQL->bSave = false;
                syslog(LOG_INFO, "MySQL-Speicherung: %d", strMySQL->bSave);
            }
            else
            {
                strMySQL->bSave = true;
                syslog(LOG_INFO, "MySQL-Speicherung: %d", strMySQL->bSave);
            }
    }
    else
    {
        strMySQL->bSave = DEFAULT_MYSQL_SAVE;
        syslog(LOG_WARNING,"%s%*s\n", "MYSQL konnte nicht von der CONFIG.INI gelesen werden", SYSLENMSG - strlen("MYSQL konnte nicht von der CONFIG.INI gelesen werden"), WARNING);
        syslog(LOG_INFO, "Default-Wert wird benutzt: %d", strMySQL->bSave);
    }

    /* Lesen des MySQLServernamens, welcher zum Speichern der Daten verwendet werden soll. */
    if (cConfig.getSegmentValueByKeyword("[MYSQL]","HOST") != NULL)
    {
        /* Speicher für den HOST allokieren */
        try
        {
            strMySQL->cHost = new char [strlen(cConfig.getSegmentValueByKeyword("[MYSQL]","HOST")) + 1];
            strcpy (strMySQL->cHost, cConfig.getSegmentValueByKeyword("[MYSQL]","HOST"));
            syslog(LOG_INFO, "MySQL-Host: %s", strMySQL->cHost);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cMySQLHost\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cMySQLHost\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }
    else
    {
        /* Speicher für den HOST allokieren */
        try
        {
            strMySQL->cHost = new char [strlen(DEFAULT_MYSQL_HOST) + 1];
            strcpy (strMySQL->cHost, DEFAULT_MYSQL_HOST);
            syslog(LOG_WARNING,"%s%*s\n", "HOST konnte nicht von der CONFIG.INI gelesen werden", SYSLENMSG - strlen("HOST konnte nicht von der CONFIG.INI gelesen werden"), WARNING);
            syslog(LOG_INFO, "Default-Wert wird benutzt: %s", strMySQL->cHost);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cMySQLHost\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cMySQLHost\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }

    /* Lesen des MySQLPorts, welcher dem des Server entspricht. */
    if (cConfig.getSegmentValueByKeyword("[MYSQL]","PORT") != NULL)
    {
        /* Speicher für den PORT allokieren */
        try
        {
            strMySQL->cPort = new char [strlen(cConfig.getSegmentValueByKeyword("[MYSQL]","PORT")) + 1];
            strcpy (strMySQL->cPort, cConfig.getSegmentValueByKeyword("[MYSQL]","PORT"));
            syslog(LOG_INFO, "MySQL-Port: %s", strMySQL->cPort);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cMySQLPort\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cMySQLPort\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }
    else
    {
        /* Speicher für den PORT allokieren */
        try
        {
            strMySQL->cPort = new char [strlen(DEFAULT_MYSQL_PORT) + 1];
            strcpy (strMySQL->cPort, DEFAULT_MYSQL_PORT);
            syslog(LOG_WARNING,"%s%*s\n", "PORT konnte nicht von der CONFIG.INI gelesen werden", SYSLENMSG - strlen("PORT konnte nicht von der CONFIG.INI gelesen werden"), WARNING);
            syslog(LOG_INFO, "Default-Wert wird benutzt: %s", strMySQL->cPort);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cMySQLPort\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cMySQLPort\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }

    /* Lesen des MySQLUsers, welcher zur Verbindung zum Server verwendet werden soll. */
    if (cConfig.getSegmentValueByKeyword("[MYSQL]","USER") != NULL)
    {
        /* Speicher für den USER allokieren */
        try
        {
            strMySQL->cUser = new char [strlen(cConfig.getSegmentValueByKeyword("[MYSQL]","USER")) + 1];
            strcpy (strMySQL->cUser, cConfig.getSegmentValueByKeyword("[MYSQL]","USER"));
            syslog(LOG_INFO, "MySQL-Benutzer: %s", strMySQL->cUser);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cMySQLUser\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cMySQLUser\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }
    else
    {
        /* Speicher für den USER allokieren */
        try
        {
            strMySQL->cUser = new char [strlen(DEFAULT_MYSQL_USER) + 1];
            strcpy (strMySQL->cUser, DEFAULT_MYSQL_USER);
            syslog(LOG_WARNING,"%s%*s\n", "USER konnte nicht von der CONFIG.INI gelesen werden", SYSLENMSG - strlen("USER konnte nicht von der CONFIG.INI gelesen werden"), WARNING);
            syslog(LOG_INFO, "Default-Wert wird benutzt: %s", strMySQL->cUser);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cMySQLUser\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cMySQLUser\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }

    /* Lesen des MySQLPassword, welches zur Verbindung zum Server verwendet werden soll. */
    if (cConfig.getSegmentValueByKeyword("[MYSQL]","PW") != NULL)
    {
        /* Speicher für das PW allokieren */
        try
        {
            strMySQL->cPassword = new char [strlen(cConfig.getSegmentValueByKeyword("[MYSQL]","PW")) + 1];
            strcpy (strMySQL->cPassword, cConfig.getSegmentValueByKeyword("[MYSQL]","PW"));
            if (strMySQL->bSave)
                syslog(LOG_INFO, "MySQL-Passwort: %s", "****");
            else
                syslog(LOG_INFO, "MySQL-Passwort:");
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cMySQLPassword\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cMySQLPassword\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }
    else
    {
        /* Speicher für das PW allokieren */
        try
        {
            strMySQL->cPassword = new char [strlen(DEFAULT_MYSQL_PASSWORD) + 1];
            strcpy (strMySQL->cPassword, DEFAULT_MYSQL_PASSWORD);
            syslog(LOG_WARNING,"%s%*s\n", "PW konnte nicht von der CONFIG.INI gelesen werden", SYSLENMSG - strlen("PW konnte nicht von der CONFIG.INI gelesen werden"), WARNING);
            if (strMySQL->bSave)
                syslog(LOG_INFO, "MySQL-Passwort: %s", "****");
            else
                syslog(LOG_INFO, "MySQL-Passwort:");
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cMySQLPassword\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cMySQLPassword\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }

    /* Lesen des MySQLDB, welche zur Speicherung der Daten verwendet werden soll. */
    if (cConfig.getSegmentValueByKeyword("[MYSQL]","DB") != NULL)
    {
        /* Speicher für die DB allokieren */
        try
        {
            strMySQL->cDatabase = new char [strlen(cConfig.getSegmentValueByKeyword("[MYSQL]","DB")) + 1];
            strcpy (strMySQL->cDatabase, cConfig.getSegmentValueByKeyword("[MYSQL]","DB"));
            syslog(LOG_INFO, "MySQL-Datenbank: %s", strMySQL->cDatabase);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cMySQLDB\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cMySQLDB\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }
    else
    {
        /* Speicher für die DB allokieren */
        try
        {
            strMySQL->cDatabase = new char [strlen(DEFAULT_MYSQL_DB) + 1];
            strcpy (strMySQL->cDatabase, DEFAULT_MYSQL_DB);
            syslog(LOG_WARNING,"%s%*s\n", "DB konnte nicht von der CONFIG.INI gelesen werden", SYSLENMSG - strlen("DB konnte nicht von der CONFIG.INI gelesen werden"), WARNING);
            syslog(LOG_INFO, "Default-Wert wird benutzt: %s", strMySQL->cDatabase);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cMySQLDB\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cMySQLDB\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }

    /* Lesen des MySQLDB, welche zur Speicherung der Daten verwendet werden soll. */
    if (cConfig.getSegmentValueByKeyword("[MYSQL]","SNTAB") != NULL)
    {
        /* Speicher für die SNTAB allokieren */
        try
        {
            strMySQL->cSNTable = new char [strlen(cConfig.getSegmentValueByKeyword("[MYSQL]","SNTAB")) + 1];
            strcpy (strMySQL->cSNTable, cConfig.getSegmentValueByKeyword("[MYSQL]","SNTAB"));
            syslog(LOG_INFO, "SN-Tabelle: %s", strMySQL->cSNTable);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cSNTable\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cSNTable\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }
    else
    {
        /* Speicher für die SNTAB allokieren */
        try
        {
            strMySQL->cSNTable = new char [strlen(DEFAULT_MYSQL_SN) + 1];
            strcpy (strMySQL->cSNTable, DEFAULT_MYSQL_SN);
            syslog(LOG_WARNING,"%s%*s\n", "SNTAB konnte nicht von der CONFIG.INI gelesen werden", SYSLENMSG - strlen("SNTAB konnte nicht von der CONFIG.INI gelesen werden"), WARNING);
            syslog(LOG_INFO, "Default-Wert wird benutzt: %s", strMySQL->cSNTable);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cSNTable\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cSNTable\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }

    /* Lesen des MySQLDB, welche zur Speicherung der Daten verwendet werden soll. */
    if (cConfig.getSegmentValueByKeyword("[MYSQL]","DATATAB") != NULL)
    {
        /* Speicher für die DATATAB allokieren */
        try
        {
            strMySQL->cDataTable = new char [strlen(cConfig.getSegmentValueByKeyword("[MYSQL]","DATATAB")) + 1];
            strcpy (strMySQL->cDataTable, cConfig.getSegmentValueByKeyword("[MYSQL]","DATATAB"));
            syslog(LOG_INFO, "Daten-Tabelle: %s", strMySQL->cDataTable);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cDataTable\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cDataTable\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }
    else
    {
        /* Speicher für die DATATAB allokieren */
        try
        {
            strMySQL->cDataTable = new char [strlen(DEFAULT_MYSQL_DATA) + 1];
            strcpy (strMySQL->cDataTable, DEFAULT_MYSQL_DATA);
            syslog(LOG_WARNING,"%s%*s\n", "DATATAB konnte nicht von der CONFIG.INI gelesen werden", SYSLENMSG - strlen("DATATAB konnte nicht von der CONFIG.INI gelesen werden"), WARNING);
            syslog(LOG_INFO, "Default-Wert wird benutzt: %s", strMySQL->cDataTable);
        }
        catch (bad_alloc &)
        {
            syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung \"cDataTable\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung \"cDataTable\")"), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);

            /* Dynamisch allokierten Speicher freigeben */
            free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, cStatusDevices);

            exit(DAEMON_EXIT_BADALLOC);
        }
    }

    /* Prüfen ob auf einen MySQL-Server gespeichert werden soll */
    if (strMySQL->bSave)
    {

        myConnection = mysql_init(NULL);
        /* Verbindung zum MySQL-Server herstellen */
        if (!mysql_real_connect(myConnection, strMySQL->cHost, strMySQL->cUser, strMySQL->cPassword, strMySQL->cDatabase, 0, NULL, 0))
        {
            syslog(LOG_ERR,"%s%s%*s\n", "MySQL-Error: ", mysql_error(myConnection), SYSLENMSG - strlen("MySQL-Error: ") - strlen(mysql_error(myConnection)), ERROR);
        }
        else
        {
            /* MySQL-Query an den Server senden (Daten-Tabelle)*/
            strncpy(cResultMySQL ,"SHOW TABLES FROM `", MYSQLENGTH -1);
            strncat(cResultMySQL, strMySQL->cDatabase, MYSQLENGTH - strlen(cResultMySQL) - strlen(strMySQL->cDatabase) - 1);
            strncat(cResultMySQL, "` LIKE '", MYSQLENGTH - strlen(cResultMySQL) - strlen("` LIKE '") - 1);
            strncat(cResultMySQL, strMySQL->cSNTable, MYSQLENGTH - strlen(cResultMySQL) - strlen(strMySQL->cSNTable) - 1);
            strncat(cResultMySQL, "'", MYSQLENGTH - strlen(cResultMySQL) - strlen("'") - 1);
            if (mysql_query(myConnection, cResultMySQL))
            {
                syslog(LOG_ERR,"%s%s%*s\n", "MySQL-Error: ", mysql_error(myConnection), SYSLENMSG - strlen("MySQL-Error: ") - strlen(mysql_error(myConnection)), ERROR);
                syslog(LOG_ERR,"%s%s\n", "MySQL-Query: ", cResultMySQL );
            }
            else
            {
                myResult = mysql_store_result(myConnection);
                if (mysql_num_rows(myResult) > 0)
                {
                    syslog(LOG_INFO, "MySQL-Tabelle %s vorhanden", strMySQL->cSNTable);
                }
                else
                {
                    syslog(LOG_ERR,"%s%s%s%s%*s\n", "MySQL-Error: ", "Tabelle ",strMySQL->cSNTable, " nicht vorhanden!", SYSLENMSG - strlen("MySQL-Error: ") - strlen("Tabelle ") - strlen(strMySQL->cSNTable) - strlen(" nicht vorhanden!"), ERROR);
                }
            }

            /* MySQL-Verbindung schliessen */
            mysql_free_result(myResult);

            /* MySQL-Query an den Server senden (Seriennummern-Tabelle)*/
            strncpy(cResultMySQL ,"SHOW TABLES FROM `", MYSQLENGTH -1);
            strncat(cResultMySQL, strMySQL->cDatabase, MYSQLENGTH - strlen(cResultMySQL) - strlen(strMySQL->cDatabase) - 1);
            strncat(cResultMySQL, "` LIKE '", MYSQLENGTH - strlen(cResultMySQL) - strlen("` LIKE '") - 1);
            strncat(cResultMySQL, strMySQL->cDataTable, MYSQLENGTH - strlen(cResultMySQL) - strlen(strMySQL->cDataTable) - 1);
            strncat(cResultMySQL, "'", MYSQLENGTH - strlen(cResultMySQL) - strlen("'") - 1);
            if (mysql_query(myConnection, cResultMySQL))
            {
                syslog(LOG_ERR,"%s%s%*s\n", "MySQL-Error: ", mysql_error(myConnection), SYSLENMSG - strlen("MySQL-Error: ") - strlen(mysql_error(myConnection)), ERROR);
                syslog(LOG_ERR,"%s%s\n", "MySQL-Query: ", cResultMySQL );
            }
            else
            {
                myResult = mysql_store_result(myConnection);
                if (mysql_num_rows(myResult) > 0)
                {
                    syslog(LOG_INFO, "MySQL-Tabelle %s vorhanden", strMySQL->cDataTable);
                }
                else
                {
                    syslog(LOG_ERR,"%s%s%s%s%*s\n", "MySQL-Error: ", "Tabelle ",strMySQL->cDataTable, " nicht vorhanden!", SYSLENMSG - strlen("MySQL-Error: ") - strlen("Tabelle ") - strlen(strMySQL->cDataTable) - strlen(" nicht vorhanden!"), ERROR);
                }
            }

        }
        /* MySQL-Verbindung schliessen */
        mysql_free_result(myResult);
        mysql_close(myConnection);
    }

    /* Prüfen ob das Segment [VALUES] vorhanden ist und Werte beinhaltet */
    if (cConfig.getSegmentValuesSize("[VALUES]") != -1)
    {
        if (cConfig.getSegmentValuesSize("[VALUES]") > 1)
        {
            iRes = 1;
        }
        else
        {
            syslog(LOG_ERR,"%s%*s\n", "Segment [VALUES] ist verfuegbar, aber enthaelt keine Werte", SYSLENMSG - strlen("Segment [VALUES] ist verfuegbar, aber enthaelt keine Werte"), ERROR);
            iRes = 0;
        }
    }
    else
    {
        syslog(LOG_ERR,"%s%*s\n", "Segment [VALUES] ist nicht verfuegbar", SYSLENMSG - strlen("Segment [VALUES] ist nicht verfuegbar"), ERROR);
        iRes = 0;
    }

    /* Prüfen ob das Segment [SERIALNUMBER] vorhanden ist und Werte beinhaltet */
    if (iRes == 1)
    {
        if (cConfig.getSegmentValuesSize("[SERIALNUMBERS]") != -1 && iRes == 1)
        {
            if (cConfig.getSegmentValuesSize("[SERIALNUMBERS]") > 1)
            {
                iRes = 1;
            }
            else
            {
                syslog(LOG_ERR,"%s%*s\n", "Segment [SERIALNUMBERS] ist verfuegbar, aber enthaelt keine Werte", SYSLENMSG - strlen("Segment [SERIALNUMBERS] ist verfuegbar, aber enthaelt keine Werte"), ERROR);
                iRes = 0;
            }
        }
        else
        {
            syslog(LOG_ERR,"%s%*s\n", "Segment [SERIALNUMBERS] ist nicht verfuegbar", SYSLENMSG - strlen("Segment [SERIALNUMBERS] ist nicht verfuegbar"), ERROR);
            iRes = 0;
        }
    }

    return iRes;
}

/* Methode für die Initialisierung des YASDI-Treiber */
int initYasdi(DWORD* dwBusDriverCounter, DWORD* dwDeviceCounter, DWORD* dwDeviceSNCounter, DWORD* dwDeviceHandle, char* cYasdiINI, bool* bFirstInitFlag, cConfig& cConfig, cStatusDevice** cStatusDevices)
{
    /* Variablen */
    bool bFlagSN = false;                               /* Flag ob eine SerienNummer bereits einem Statusdeviceobjekt zugewiesen wurde  */
    int iRes = 0;                                       /* Ergebnis der Initialisierung                                                 */
    DWORD dwCounterDevices = 0;                         /* Zähler für eine FOR-Schleife für das Durchlaufen der DeviceHandles           */
    DWORD dwCounterDevicesSN = 0;                       /* Zähler für eine FOR-Schleife für das Durchlaufen des Seriennummernsegments   */
    DWORD dwCounterStatus = 0;                          /* Zähler für eine FOR-Schleife für das Durchlaufen der StatusDevicesobjekte    */
    DWORD dwSerNr = 0;                                  /* Seriennummer als unsigned int                                                */
    char cSerNr[SNLENGTH] = "";                         /* Char-Array für die Seriennummer eines Wechselrichters                        */
    cStatusDevice* cStatusDevicesBuffer = NULL;         /* Zeiger vom Typ "cStatusDevice", welcher später auf ein Array zeigt           */
    const char** cSerialNumbers = NULL;                 /* Char*-Array für die Seriennummern der Geräte                                 */

    /* Seriennummern der Geräte ermitteln */
    cSerialNumbers = cConfig.getSegmentValues("[SERIALNUMBERS]");

    /* Loggen der bereits gefundenen Geräten und wieviel verfügbar sein sollten */
    syslog(LOG_INFO, "Suche nach Geraeten: %d / aktuelle Anzahl der bekannten Geraete an Hand der Seriennummer: %d", cConfig.getSegmentValuesSize("[SERIALNUMBERS]"), (*cStatusDevices)->getDeviceCounter());

    /* Seriennummernzähler zurücksetzen für eine erneute Initialisierung */
    *dwDeviceSNCounter = 0;

    /* Daten von der YASDI.INI beim ersten Aufruf (Initialisierung) lesen */
    if (*bFirstInitFlag == false)
    {
        syslog(LOG_INFO,"Initialisierung des YASDI-Master");
        iRes = yasdiMasterInitialize(cYasdiINI, dwBusDriverCounter);
    }

    if (iRes == 0 || *bFirstInitFlag == true)
    {
        /* Bustreiber beim ersten Aufruf initialisieren */
        if (*bFirstInitFlag == false)
        {
            syslog(LOG_INFO,"Einschalten des YASDI-Treibers");
            for(dwCounterDevices = 0; dwCounterDevices < *dwBusDriverCounter; dwCounterDevices++)
                yasdiMasterSetDriverOnline( dwCounterDevices );
        }

        /* Wechselrichtersuche starten */
        syslog(LOG_INFO,"Start der Geraetesuche");

        /* Anzahl der Wechselrichter ermittlen */
        DoStartDeviceDetection(cConfig.getSegmentValuesSize("[SERIALNUMBERS]"), true);

        syslog(LOG_INFO,"Geraetesuche beendet");

        /* Handles der verfügbaren Geräte ermitteln */
        *dwDeviceCounter = GetDeviceHandles(dwDeviceHandle, DEVICES );
        syslog(LOG_INFO,"Anzahl der gefunden Geraete mit dem YASDI-Treiber: %d", *dwDeviceCounter);

        /* Prüfen ob weitere Wechselrichter gefunden wurden und dafür zusätzliche Statusobjekte erzeugen */
        if ((*cStatusDevices)->getDeviceCounter() > 0)
        {
            /* Speicher für die neue Anzahl von Statusobjekten reservieren (Temp) */
            try
            {
                cStatusDevicesBuffer = new cStatusDevice [(*cStatusDevices)->getDeviceCounter()];
                cStatusDevicesBuffer->setDeviceBufferCounter((*cStatusDevices)->getDeviceCounter());
            }
            catch (bad_alloc &)
            {
                /* Dynamisch allokierten Speicher freigeben */
                if (*cStatusDevices != NULL)
                {
                    delete [] (*cStatusDevices);
                    *cStatusDevices = NULL;
                }

                if (cStatusDevicesBuffer != NULL)
                {
                    delete [] (cStatusDevicesBuffer);
                    cStatusDevicesBuffer = NULL;
                }

                syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung  \"cStatusDevicesBuffer\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung  \"cStatusDevicesBuffer\")"), ERROR);
                syslog(LOG_ERR,"%s%*s\n", "Daemon beendet...  ", SYSLENMSG - strlen("Daemon beendet...  "), ERROR);
                exit(DAEMON_EXIT_BADALLOC);
            }

            /* Statusobjekte der bereits gefundenen Geräte sichern */
            for (dwCounterStatus= 0; dwCounterStatus < (*cStatusDevices)->getDeviceCounter(); dwCounterStatus++)
                cStatusDevicesBuffer[dwCounterStatus] = (*cStatusDevices)[dwCounterStatus];

            /* Alte Statusobjekte löschen */
            delete [] *cStatusDevices;
            *cStatusDevices = NULL;

            /* Speicher für die neue Anzahl von Statusobjekten reservieren */
            try
            {
                *cStatusDevices = new cStatusDevice [*dwDeviceCounter];
                (*cStatusDevices)->setDeviceCounter(*dwDeviceCounter);
            }
            catch (bad_alloc &)
            {
                /* Dynamisch allokierten Speicher freigeben */
                if (*cStatusDevices != NULL)
                {
                    delete [] (*cStatusDevices);
                    *cStatusDevices = NULL;
                }

                if (cStatusDevicesBuffer != NULL)
                {
                    delete [] (cStatusDevicesBuffer);
                    cStatusDevicesBuffer = NULL;
                }

                syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Nachallokierung \"cStatusDevices\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Nachallokierung \"cStatusDevices\")"), ERROR);
                syslog(LOG_ERR,"%s%*s\n", "Daemon beendet...  ", SYSLENMSG - strlen("Daemon beendet...  "), ERROR);
                exit(DAEMON_EXIT_BADALLOC);
            }
        }

        /* Prüfen ob Wechselrichter gefunden wurden */
        /* Wird nur nachdem ersten finden von Geräten aufgerufen. */
        if (dwDeviceCounter > 0 && (*cStatusDevices)->getDeviceCounter() == 0)
        {
            /* Status-Objekte für die gefundenen Geräte erstellen */
            try
            {
                *cStatusDevices = new cStatusDevice [*dwDeviceCounter];
                (*cStatusDevices)->setDeviceCounter(*dwDeviceCounter);
            }
            catch (bad_alloc &)
            {
                /* Dynamisch allokierten Speicher freigeben */
                if (*cStatusDevices != NULL)
                {
                    delete [] (*cStatusDevices);
                    *cStatusDevices = NULL;
                }

                if (cStatusDevicesBuffer != NULL)
                {
                    delete [] (cStatusDevicesBuffer);
                    cStatusDevicesBuffer = NULL;
                }

                syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung  \"cStatusDevices\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung  \"cStatusDevices\")"), ERROR);
                syslog(LOG_ERR,"%s%*s\n", "Daemon beendet...  ", SYSLENMSG - strlen("Daemon beendet...  "), ERROR);
                exit(DAEMON_EXIT_BADALLOC);
            }
        }

        /* Seriennummer des aktuellen Geräts ermitteln */
        for (dwCounterDevices = 0; dwCounterDevices < *dwDeviceCounter; dwCounterDevices++)
        {
            iRes = GetDeviceSN(dwDeviceHandle[dwCounterDevices], &dwSerNr);
            if (iRes > YE_UNKNOWN_HANDLE)
            {
                /* SerienNummer in einen String konvertieren */
                sprintf (cSerNr, "%u", dwSerNr);

                /* Prüfen ob die Seriennummer eine der Seriennummern in der CONFIG.INI ist */
                for (dwCounterDevicesSN = 0; dwCounterDevicesSN < (unsigned int)cConfig.getSegmentValuesSize("[SERIALNUMBERS]"); dwCounterDevicesSN++)
                {
                    if (!strcmp(cSerNr, cSerialNumbers[dwCounterDevicesSN]))
                    {
                        (*dwDeviceSNCounter)++;
                        break;
                    }
                }
                if (cStatusDevicesBuffer != NULL)
                {
                    /* Array der temporären Statusobjekte durchlaufen und prüfen ob die aktuelle SerienNummer
                     * einer der Seriennummern aus den temporären Statusobjekte entspricht. Dann diese Objekt an die Stelle
                     * des neuen Statusarrays kopieren.*/
                    for (dwCounterStatus = 0; dwCounterStatus < cStatusDevicesBuffer->getDeviceBufferCounter(); dwCounterStatus++)
                    {
                        if (cStatusDevicesBuffer[dwCounterStatus].getSN() == dwSerNr)
                        {
                            (*cStatusDevices)[dwCounterDevices] = cStatusDevicesBuffer[dwCounterStatus];
                            bFlagSN = true;
                            break;
                        }
                    }
                }

                /* Wenn für die akutelle Seriennummer noch kein Statusobjekt vorhanden ist, wird hier eine erzeugt */
                if (bFlagSN == false)
                {
                    (*cStatusDevices + dwCounterDevices)->setSN(dwSerNr);
                }

                bFlagSN = false;

                syslog(LOG_INFO,"Geraet \"%d\" verfuegbar...", dwSerNr);
            }
            else
            {
                syslog(LOG_INFO,"Geraet nicht verfuegbar...");
            }
        }
    }
    else
    {
        /* Setzen des bFirstInitFlag */
        *bFirstInitFlag = true;

        return -1;
    }

    /* Setzen bFirstInitFlag */
    *bFirstInitFlag = true;

    /* Speicher für die alten Objekte freigeben */
    if (cStatusDevicesBuffer != NULL)
    {
        delete [] cStatusDevicesBuffer;
        cStatusDevicesBuffer = NULL;
    }

    /* Zurück geben des Ergebnisses der Initialisierung */
    return iRes;
}

/* Methode für das Anfragen von Daten der der Wechselrichter */
int getYasdiData(const DWORD* dwDeviceHandle, const DWORD* dwDeviceCounter, cConfig& cConfig, struct strConfigs* strConfigs, struct strDaemon* strDaemon, cStatusDevice** cStatusDevices, struct strStatus* strStatus, struct strFile* strFile, struct strMySQL* strMySQL, struct strMail* strMail, cMailQueue* cMailQueueError, cMailQueue* cMailQueueSystem, cMailQueue* cMailQueueStop, char cVersionsstring[])
{
    /* Variablen */
    int iRes=0;                                         /* Ergebnis der Anfrage                                                     */
    const char** cValues = NULL;                        /* Char*-Array für die Werte eines Segments                                 */
    char* cFileName = NULL;                             /* Char-Array für den Dateinnamen der Logdatei                              */
    char cTimeFormatFile[] = "%d_%m_%Y_";               /* Zeitformat für die Logdateien                                            */
    char cTimeFormatMySQL[] = "%Y-%m-%d %H:%M:%S";      /* Zeitformat für einen Wert                                                */
    char cTimeMySQL[21] = "";                           /* Zeitstring für einen Wert                                                */
    int iTimeFormatFileLength = 11;                     /* Länge des Zeitformatstrings für den Dateinamen (01_01_2011_)             */
    int iTimeFormatMySQLLength = 20;                    /* Länge des Zeitformatstrings für einen Wert  (2001-01-01 12:12:00)        */
    char cSerNr[SNLENGTH] = "";                         /* Char-Array für die Seriennummer                                          */
    DWORD iCounterDevices = 0;                          /* Zähler für eine FOR-Schleife für das Durchlaufen der DeviceHandles       */
    DWORD iCounterValues = 0;                           /* Zähler für eine FOR-Schleife für das Durchlaufen der Values              */
    DWORD channelHandle=0;                              /* Channelhandle                                                            */
    DWORD dwTime=0;                                     /* Zeitstempel für einen Kanalwert                                          */
    DWORD SerNr=0;                                      /* Seriennummer eines Wechselrichters                                       */
    double value;                                       /* Channelvalue                                                             */
    char valuetext[VALUELENGTH];                        /* Channeltextvalue                                                         */
    char cTimeMail[MAIL_TIME_LENGTH] = "";              /* Zeitstring für den Mail- bzw. Messagezeitstempel (2012-01-01 12:12:00)   */

    char cResultFile[MYSQLENGTH] = "";                  /* Ergebnisstring der die Daten zum Speicher in einer Datei aufnimmt        */
    char cResultMySQL[MYSQLENGTH] = "";                 /* Ergebnisstring der die Daten zum Speicher in einen MySQL-Server aufnimmt */
    char cMySQLValues[1024] = "";                       /* Werte von den Wechselrichtern für den MySQL-Server                       */
    MYSQL* myConnection = NULL;                         /* MySQL-Connection                                                         */
    MYSQL_RES *myResult = NULL;                         /* MySQL-Result                                                             */

    fstream fFile;                                      /* File-pointer                                                             */
    tm *tmTimeStamp;                                    /* Struktur für einen Zeitstempel                                           */

    /* Jedes einzelen Geräte anfragen und Daten holen */
    for (iCounterDevices = 0; iCounterDevices < *dwDeviceCounter; iCounterDevices++)
	{
        /* Seriennummer des aktuellen Geräts */
        sprintf (cSerNr, "%u", (*cStatusDevices + iCounterDevices)->getSN());
        SerNr = (*cStatusDevices + iCounterDevices)->getSN();

        /* Channelhandle des "cStatusChannel" ermitteln */
        channelHandle = FindChannelName(dwDeviceHandle[iCounterDevices], strStatus->cStatusChannel);

        /* Prüfen pb der Statuschannel gültig bzw. verfügbar ist */
        if (channelHandle != INVALID_HANDLE)
        {
            /* Wert vom Channel "Status" ermitteln */
            iRes = GetChannelValue(channelHandle,                       /* Channelhandle                */
                                    dwDeviceHandle[iCounterDevices],    /* Devicehandle                 */
                                    &value,                             /* Value                        */
                                    valuetext,                          /* Textvalue                    */
                                    (VALUELENGTH - 1),                  /* Textvalue size               */
                                    0 );                                /* Maximales Alter des Values   */

            /* Zeitstempel des Werts ermitteln */
            dwTime = GetChannelValueTimeStamp(channelHandle, dwDeviceHandle[iCounterDevices]);
        }

        /* Prüfen ob der Wert gelesen werden konnte */
        if (iRes == YE_OK)
        {
            if ((*cStatusDevices + iCounterDevices)->getPowerOnFlag() == false)
            {
                syslog(LOG_INFO,"Geraet \"%d\" ist online...", SerNr);

                /* Mail versenden */
                try
                {
                    getTime(cTimeMail);
                    cMailQueueSystem->addMsg(new cMailMsg(cTimeMail));
                    cMailQueueSystem->addMsg(new cMailMsg((char*)"Geraet \""));
                    cMailQueueSystem->addMsg(new cMailMsg(cSerNr));
                    cMailQueueSystem->addMsg(new cMailMsg((char*)"\" ist online...\n"));
                }
                catch (...)
                {
                    sendMail(strMail, cVersionsstring, cMailQueueStop);
                }

                /* Den "Gerät an"-Status auf "an" (true) setzen */
                (*cStatusDevices + iCounterDevices)->setPowerOnFlag(true);
            }

            /* Prüfen ob der Wert ein String ist */
            if (strlen(valuetext))
            {
                /* Den Wert für "Gerät speist ein" gleich dem Wert vom Gerät ist */
                if (!strcmp(valuetext, strStatus->cStatusValue))
                {
                    /* Prüfen welchen Status das Gerät hat und gegebenfalls setzen und Log-Meldung schreiben */
                    if ((*cStatusDevices + iCounterDevices)->getFeedInFlag() == false)
                    {
                        syslog(LOG_INFO,"Geraet \"%d\" speist Strom ins Netz ein...", SerNr);

                        /* Mail versenden */
                        try
                        {
                            getTime(cTimeMail);
                            cMailQueueSystem->addMsg(new cMailMsg(cTimeMail));
                            cMailQueueSystem->addMsg(new cMailMsg((char*)"Geraet \""));
                            cMailQueueSystem->addMsg(new cMailMsg(cSerNr));
                            cMailQueueSystem->addMsg(new cMailMsg((char*)"\" speist Strom ins Netz ein...\n"));
                        }
                        catch (...)
                        {
                            sendMail(strMail, cVersionsstring, cMailQueueStop);
                        }

                        (*cStatusDevices + iCounterDevices)->setFeedInFlag(true);
                    }
                }
                else
                {
                    if ((*cStatusDevices + iCounterDevices)->getFeedInFlag() == true)
                    {
                        syslog(LOG_INFO,"Geraet \"%d\" speist keinen Strom ins Netz ein...", SerNr);

                        /* Mail versenden */
                        try
                        {
                            getTime(cTimeMail);
                            cMailQueueSystem->addMsg(new cMailMsg(cTimeMail));
                            cMailQueueSystem->addMsg(new cMailMsg((char*)"Geraet \""));
                            cMailQueueSystem->addMsg(new cMailMsg(cSerNr));
                            cMailQueueSystem->addMsg(new cMailMsg((char*)"\" speist keinen Strom ins Netz ein...\n\n"));
                        }
                        catch (...)
                        {
                            sendMail(strMail, cVersionsstring, cMailQueueStop);
                        }

                        (*cStatusDevices + iCounterDevices)->setFeedInFlag(false);
                    }
                }
            }
        }
        else
        {
            /* Prüfen ob das Gerät "an" also (true) markiert ist und eben der Wert nicht lesbar war
             * das Geräte auf aus (false) setzen */
            if ((*cStatusDevices + iCounterDevices)->getPowerOnFlag())
            {
                syslog(LOG_INFO,"Channelname \"Status\" des Geraets \"%d\" ist nicht verfuegbar...", SerNr);
                syslog(LOG_INFO,"Geraet \"%d\" ist offline...", SerNr);

                /* Mail versenden */
                try
                {
                    getTime(cTimeMail);
                    cMailQueueSystem->addMsg(new cMailMsg(cTimeMail));
                    cMailQueueSystem->addMsg(new cMailMsg((char*)"Channelname \"Status\" des Geraets \""));
                    cMailQueueSystem->addMsg(new cMailMsg(cSerNr));
                    cMailQueueSystem->addMsg(new cMailMsg((char*)"\" ist nicht verfuegbar...\n"));
                    getTime(cTimeMail);
                    cMailQueueSystem->addMsg(new cMailMsg(cTimeMail));
                    cMailQueueSystem->addMsg(new cMailMsg((char*)"Geraet \""));
                    cMailQueueSystem->addMsg(new cMailMsg(cSerNr));
                    cMailQueueSystem->addMsg(new cMailMsg((char*)"\" ist offline...\n"));
                }
                catch (...)
                {
                    sendMail(strMail, cVersionsstring, cMailQueueStop);
                }

                (*cStatusDevices + iCounterDevices)->setPowerOnFlag(false);
            }
        }

        /* Prüfen ob der aktuelle Wechselrichter Strom einspeist, wenn ja weitere Werte auslesen und speichern */
        if ((*cStatusDevices + iCounterDevices)->getFeedInFlag())
        {
            /* TimeStamp in die Struktur tmTimeStamp schreiben */
            tmTimeStamp = localtime((const long int*)&dwTime);
            strftime(cTimeMySQL, iTimeFormatMySQLLength,  cTimeFormatMySQL, tmTimeStamp);

            /* Values eines Segments ermitteln */
            cValues = cConfig.getSegmentValues("[VALUES]");

            /* Filestring zusammenfügen, wenn das Datei-Speichern aktiviert ist */
            if (strFile->bSave)
            {
                /* Zeitstempel des Werts und Seperator in den Resultstring für die Datei schreiben */
                strncpy(cResultFile, cTimeMySQL, MYSQLENGTH - 1);
                strncat(cResultFile, strFile->cSeperatorFile, MYSQLENGTH - strlen(cResultFile) - strlen(strFile->cSeperatorFile) - 1);
            }

            /* MySQL-Insert zusammenfügen, wenn das MySQL-Speichern aktiviert ist */
            if(strMySQL->bSave)
            {
                /* Zeitstempel des Werts formatieren */
                sprintf(valuetext, "%d", dwTime);

                strncpy(cResultMySQL ,"INSERT INTO `", MYSQLENGTH - 1);
                strncat(cResultMySQL , strMySQL->cDataTable, MYSQLENGTH - strlen(cResultMySQL) - strlen(strMySQL->cDataTable) - 1);
                strncat(cResultMySQL , "` ", MYSQLENGTH - strlen(cResultMySQL) - strlen("` ") - 1);
                strncat(cResultMySQL , "(`ID`, `ID_FK_SN`, `Zeitstempel`, `", MYSQLENGTH - strlen(cResultMySQL) - strlen("(`ID`, `ID_FK_SN`, `Zeitstempel`, `") - 1);

                strncpy(cMySQLValues, "VALUES (NULL, ", MYSQLENGTH - 1);

                /* Prüfen ob noch ein Wert folgt, wenn nicht INSERT abschliessen */
                if ((unsigned int)(cConfig.getSegmentValuesSize("[VALUES]") - 1) > 0)
                {
                    strncat(cMySQLValues, "(SELECT `ID_PK_SN` FROM ", MYSQLENGTH - strlen (cMySQLValues) - strlen("(SELECT `ID_PK_SN` FROM ") - 1);
                    strncat(cMySQLValues, strMySQL->cSNTable, MYSQLENGTH - strlen (cMySQLValues) - strlen(strMySQL->cSNTable) -1);
                    strncat(cMySQLValues, " WHERE `Seriennummer`= ", MYSQLENGTH - strlen (cMySQLValues)- strlen(" WHERE `Seriennummer`= "));
                    strncat(cMySQLValues, cSerNr, MYSQLENGTH - strlen (cMySQLValues) - strlen(cSerNr) -1);
                    strncat(cMySQLValues, "), '", MYSQLENGTH - strlen(cMySQLValues) - strlen(")', '") - 1);
                    strncat(cMySQLValues, cTimeMySQL, MYSQLENGTH - strlen(cMySQLValues) - strlen(cTimeMySQL) - 1);
                    strncat(cMySQLValues, "', '", MYSQLENGTH - strlen(cMySQLValues) - strlen("', '") - 1);
                }
                else
                {
                    strncat(cMySQLValues, "(SELECT `ID_PK_SN` FROM ", MYSQLENGTH - strlen (cMySQLValues) - strlen("(SELECT `ID_PK_SN` FROM ") - 1);
                    strncat(cMySQLValues, strMySQL->cSNTable, MYSQLENGTH - strlen (cMySQLValues) - strlen(strMySQL->cSNTable) -1);
                    strncat(cMySQLValues, " WHERE `Seriennummer`= ", MYSQLENGTH - strlen (cMySQLValues)- strlen(" WHERE `Seriennummer`= "));
                    strncat(cMySQLValues, cSerNr, MYSQLENGTH - strlen (cMySQLValues) - strlen(cSerNr) -1);
                    strncat(cMySQLValues, "))", MYSQLENGTH - strlen(cMySQLValues) - strlen("))") - 1);
                    strncat(cMySQLValues, cTimeMySQL, MYSQLENGTH - strlen(cMySQLValues) - strlen(cTimeMySQL) - 1);
                    strncat(cMySQLValues, "', '", MYSQLENGTH - strlen(cMySQLValues) - strlen("', '") - 1);
                }

            }

            /* Werte der Channel welche im Segment [VALUES] stehen auslesen */
            for (iCounterValues = 0; iCounterValues < (unsigned int)cConfig.getSegmentValuesSize("[VALUES]"); iCounterValues++)
            {
                /* MySQL-Insert zusammenfügen, wenn das MySQL-Speichern aktiviert ist */
                if(strMySQL->bSave)
                {
                    /* MySQL-Insert-Columns zusammenfügen */
                    /* Prüfen ob noch ein Wert folgt, wenn nicht INSERT abschliessen */
                    if (iCounterValues < (unsigned int)(cConfig.getSegmentValuesSize("[VALUES]") - 1))
                    {
                        strncat(cResultMySQL , (char*)cValues[iCounterValues], MYSQLENGTH - strlen(cResultMySQL) - strlen((char*)cValues[iCounterValues]) - 1);
                        strncat(cResultMySQL , "`, `", MYSQLENGTH - strlen(cResultMySQL) - strlen("`, `") - 1);
                    }
                    else
                    {
                        strncat(cResultMySQL , (char*)cValues[iCounterValues], MYSQLENGTH - strlen(cResultMySQL) - strlen((char*)cValues[iCounterValues]) - 1);
                        strncat(cResultMySQL , "`) ", MYSQLENGTH - strlen(cResultMySQL) - strlen("`) ") - 1);
                    }
                }

                /* Channelhandle des Kanals ermitteln */
                channelHandle = FindChannelName(dwDeviceHandle[iCounterDevices], (char*)cValues[iCounterValues]);

                /* Prüfen ob der ermittelte Channel ein gültiger Channel ist */
                if (channelHandle != INVALID_HANDLE)
                {
                    /* Wert vom Channel holen */
                    iRes = GetChannelValue(channelHandle,                       /* Channelhandle                */
                                           dwDeviceHandle[iCounterDevices],     /* Devicehandle                 */
                                           &value,                              /* Value                        */
                                           valuetext,                           /* Textvalue                    */
                                           (VALUELENGTH - 1),                   /* Textvaluegröße               */
                                           0 );                                 /* Maximales Alter des Values   */

                    /* Pruefen ob der Wert gelesen werden konnte */
                    if (iRes == YE_OK)
                    {
                        /* Prüfen ob der Wert ein String ist */
                        if (strlen(valuetext))
                        {
                            /* Prüfen ob noch ein Wert folgt */
                            if (iCounterValues < (unsigned int)(cConfig.getSegmentValuesSize("[VALUES]") - 1))
                            {
                                /* Filestring zusammenfügen, wenn das Datei-Speichern aktiviert ist */
                                if (strFile->bSave)
                                {
                                    strncat(cResultFile, valuetext, MYSQLENGTH - strlen(cResultFile) - strlen(valuetext) - 1);
                                    strncat(cResultFile, strFile->cSeperatorFile, MYSQLENGTH - strlen(cResultFile) - strlen(strFile->cSeperatorFile) - 1);
                                }

                                /* MySQL-Insert zusammenfügen, wenn das MySQL-Speichern aktiviert ist */
                                if(strMySQL->bSave)
                                {
                                    strncat(cMySQLValues, valuetext, MYSQLENGTH - strlen(cMySQLValues) - strlen(valuetext) - 1);
                                    strncat(cMySQLValues, "', '", MYSQLENGTH - strlen(cMySQLValues) - strlen("', '") - 1);
                                }
                            }
                            else
                            {
                                /* Filestring zusammenfügen, wenn das Datei-Speichern aktiviert ist */
                                if (strFile->bSave)
                                {
                                    strncat(cResultFile, valuetext, MYSQLENGTH - strlen(cResultFile) - strlen(valuetext) - 1);
                                }

                                /* MySQL-Insert zusammenfügen, wenn das MySQL-Speichern aktiviert ist */
                                if(strMySQL->bSave)
                                {
                                    strncat(cMySQLValues, valuetext, MYSQLENGTH - strlen(cMySQLValues) - strlen(valuetext) - 1);
                                    strncat(cMySQLValues, "')", MYSQLENGTH - strlen(cMySQLValues) - strlen("')") - 1);
                                }
                            }
                        }
                        else
                        {
                            /* Double-Wert in einen String mit 3 Nachkommastellen umwandeln */
                            sprintf(valuetext, "%0.3lf", value);

                            /* Prüfen ob noch ein Wert folgt */
                            if (iCounterValues < (unsigned int)(cConfig.getSegmentValuesSize("[VALUES]") - 1))
                            {
                                /* Filestring zusammenfügen, wenn das Datei-Speichern aktiviert ist */
                                if (strFile->bSave)
                                {
                                    strncat(cResultFile, valuetext, MYSQLENGTH - strlen(cResultFile) - strlen(valuetext) - 1);
                                    strncat(cResultFile, strFile->cSeperatorFile, MYSQLENGTH - strlen(cResultFile) - strlen(strFile->cSeperatorFile) - 1);
                                }

                                /* MySQL-Insert zusammenfügen, wenn das MySQL-Speichern aktiviert ist */
                                if(strMySQL->bSave)
                                {
                                    strncat(cMySQLValues, valuetext, MYSQLENGTH - strlen(cMySQLValues) - strlen(valuetext) - 1);
                                    strncat(cMySQLValues, "', '", MYSQLENGTH - strlen(cMySQLValues) - strlen("', '") - 1);
                                }
                            }
                            else
                            {
                                /* Filestring zusammenfügen, wenn das Datei-Speichern aktiviert ist */
                                if (strFile->bSave)
                                {
                                    strncat(cResultFile, valuetext, MYSQLENGTH - strlen(cResultFile) - strlen(valuetext) - 1);
                                }

                                /* MySQL-Insert zusammenfügen, wenn das MySQL-Speichern aktiviert ist */
                                if(strMySQL->bSave)
                                {
                                    strncat(cMySQLValues, valuetext, MYSQLENGTH - strlen(cMySQLValues) - strlen(valuetext) - 1);
                                    strncat(cMySQLValues, "')", MYSQLENGTH - strlen(cMySQLValues) - strlen("')") - 1);
                                }
                            }
                        }
                    }
                }
                else
                {
                    syslog(LOG_INFO,"%s%s%s%*s\n", "ChannelName \"", cValues[iCounterValues], "\" ist nicht verfuegbar", SYSLENMSG - (strlen("ChannelName \"") + strlen(cValues[iCounterValues]) + strlen("\" ist nicht verfuegbar")), WARNING);

                    try
                    {
                        getTime(cTimeMail);
                        cMailQueueSystem->addMsg(new cMailMsg(cTimeMail));
                        cMailQueueSystem->addMsg(new cMailMsg((char*)"Channelname \""));
                        cMailQueueSystem->addMsg(new cMailMsg((char*)cValues[iCounterValues]));
                        cMailQueueSystem->addMsg(new cMailMsg((char*)"\" ist nicht verfuegbar...\n\n"));
                    }
                    catch (...)
                    {
                        sendMail(strMail, cVersionsstring, cMailQueueStop);
                    }
                }
            }

            /* Prüfen ob in eine Datei gespeichert werden soll */
            if (strFile->bSave)
            {
                try
                {
                    cFileName = new char [iTimeFormatFileLength + SNLENGTH + strlen(strFile->cExt) + 1];

                    /* Logdateiname generieren */
                    tmTimeStamp = localtime((const long int*)&dwTime);
                    strftime(cFileName, iTimeFormatFileLength + SNLENGTH + strlen(strFile->cExt),  cTimeFormatFile, tmTimeStamp);
                    strncat (cFileName, cSerNr, iTimeFormatFileLength + SNLENGTH + strlen(strFile->cExt) - strlen(cFileName));
                    strncat (cFileName, strFile->cExt, iTimeFormatFileLength + SNLENGTH + strlen(strFile->cExt) - strlen(cFileName));
                }
                catch (bad_alloc &)
                {
                    syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (Erstellung  \"cFileName\")", SYSLENMSG - strlen("Speicherallokierung schlug fehl (Erstellung  \"cFileName\")"), ERROR);
                    syslog(LOG_ERR,"%s%*s\n", "Daemon beendet...  ", SYSLENMSG - strlen("Daemon beendet...  "), ERROR);

                    /* Mail versenden */
                    sendMail(strMail, cVersionsstring, cMailQueueStop);

                    /* Dynamisch allokierten Speicher freigeben */
                    free_memory(strStatus, strConfigs, strDaemon, strMySQL, strMail, *cStatusDevices);

                    exit(DAEMON_EXIT_BADALLOC);
                }


                /* Datei im Anfüge-Modus öffnen */
                fFile.open(cFileName, ios::out|ios::app);
                if (fFile.is_open())
                {
                    fFile << cResultFile << endl;

                    /* Datei wieder schliessen */
                    fFile.close();
                }

                if (cFileName != NULL)
                {
                    delete [] cFileName;
                    cFileName = NULL;
                }
            }

            /* Prüfen ob auf einen MySQL-Server gespeichert werden soll */
            if (strMySQL->bSave)
            {
                /* MySQL-Insert und MySQL-Values zusammenfügen */
                strncat(cResultMySQL, cMySQLValues, MYSQLENGTH - strlen(cResultMySQL) - strlen(cMySQLValues) - 1);

                myConnection = mysql_init(NULL);
                /* Verbindung zum MySQL-Server herstellen */
                if (!mysql_real_connect(myConnection, strMySQL->cHost, strMySQL->cUser, strMySQL->cPassword, strMySQL->cDatabase, 0, NULL, 0))
                {
                    syslog(LOG_ERR,"%s%s%*s\n", "MySQL-Error: ", mysql_error(myConnection), SYSLENMSG - strlen("MySQL-Error: ") - strlen(mysql_error(myConnection)), ERROR);

                    try
                    {
                        getTime(cTimeMail);
                        cMailQueueError->addMsg(new cMailMsg(cTimeMail));
                        cMailQueueError->addMsg(new cMailMsg((char*)"MySQL-Error:"));
                        cMailQueueError->addMsg(new cMailMsg((char*)mysql_error(myConnection)));
                        cMailQueueError->addMsg(new cMailMsg((char*)"\n\n"));
                        sendMail(strMail, cVersionsstring, cMailQueueError);
                    }
                    catch (...)
                    {
                        sendMail(strMail, cVersionsstring, cMailQueueStop);
                    }
                }
                else
                {
                    /* MySQL-Query an den Server senden*/
                    if (mysql_query(myConnection, cResultMySQL))
                    {
                        syslog(LOG_ERR,"%s%s%*s\n", "MySQL-Error: ", mysql_error(myConnection), SYSLENMSG - strlen("MySQL-Error: ") - strlen(mysql_error(myConnection)), ERROR);
                        syslog(LOG_ERR,"%s%s\n", "MySQL-Query: ", cResultMySQL );

                        try
                        {
                            getTime(cTimeMail);
                            cMailQueueError->addMsg(new cMailMsg(cTimeMail));
                            cMailQueueError->addMsg(new cMailMsg((char*)"MySQL-Error:"));
                            cMailQueueError->addMsg(new cMailMsg((char*)mysql_error(myConnection)));
                            cMailQueueError->addMsg(new cMailMsg((char*)"\n\n"));
                            sendMail(strMail, cVersionsstring, cMailQueueError);
                        }
                        catch (...)
                        {
                            sendMail(strMail, cVersionsstring, cMailQueueStop);
                        }
                    }
                }
                myResult = mysql_store_result(myConnection);

                /* MySQL-Speicher freigeben */
                if (myResult != NULL)
                {
                    mysql_free_result(myResult);
                    myResult = NULL;
                }

                /* MySQL-Verbindung schliessen */
                mysql_close(myConnection);
            }
        }
	}

    return iRes;
}

/* Methode für das Versenden von Systemmeldungen */ //TODO
int sendMail (struct strMail* strMail, char cVersionString[], cMailQueue* cMailQueue)
{
    char cTimeMail[MAIL_TIME_LENGTH] = "";              /* Zeitstring für den Mail- bzw. Messagezeitstempel (2012-01-01 12:12:00)   */
    char* cMailContent = NULL;
    int iMailContentLength = 0;

    /* Mail-Kommando Variablen */
    char cMailCmd1[] = "sendEmail -q -s \"";
    char cMailCmd2[] = ":";
    char cMailCmd3[] = "\" -o tls=\"";
    char cMailCmd4[] = "\" -f \"";
    char cMailCmd5[] = "\" -t \"";
    char cMailCmd6[] = "\" -u \"";
    char cMailCmd7[] = "\" -m \"";
    char cMailCmd8[] = "\" -xu \"";
    char cMailCmd9[] = "\" -xp \"";
    char cMailCmd10[] = "\"";
    char* cMailCmd = NULL;

    /* Komplettes Mail-Content auslesen */
    cMailContent = cMailQueue->getAllMsg(cMailContent);

    /* Prüfen ob in der Queue Meldungen befanden und deren Länge bestimmen */
    if (cMailContent != NULL)
        iMailContentLength = strlen (cMailContent);
    else
        iMailContentLength = strlen ("Keine Meldungen vorhanden!\n") + MAIL_TIME_LENGTH;

    /* Speicher für das MailKommando bestimmen */
    cMailCmd = new char [strlen(cMailCmd1) + strlen(strMail->cServer)
                       + strlen(cMailCmd2) + strlen(strMail->cPort)
                       + strlen(cMailCmd3) + strlen(strMail->cTls)
                       + strlen(cMailCmd4) + strlen(strMail->cFrom)
                       + strlen(cMailCmd5) + strlen(strMail->cTo)
                       + strlen(cMailCmd6) + strlen(cMailQueue->getSubject())
                       + strlen(cMailQueue->getHeader())
                       + strlen(cMailCmd7) + iMailContentLength
                       + strlen(cMailQueue->getFooter())
                       + strlen(cVersionString)
                       + strlen(cMailCmd8) + strlen(strMail->cUser)
                       + strlen(cMailCmd9) + strlen(strMail->cPassword)
                       + strlen(cMailCmd10) + 1];

    /* MailKommando erstellen */
    strcpy(cMailCmd, cMailCmd1);
    strcat(cMailCmd, strMail->cServer);
    strcat(cMailCmd, cMailCmd2);
    strcat(cMailCmd, strMail->cPort);
    strcat(cMailCmd, cMailCmd3);
    strcat(cMailCmd, strMail->cTls);
    strcat(cMailCmd, cMailCmd4);
    strcat(cMailCmd, strMail->cFrom);
    strcat(cMailCmd, cMailCmd5);
    strcat(cMailCmd, strMail->cTo);
    strcat(cMailCmd, cMailCmd6);
    strcat(cMailCmd, cMailQueue->getSubject());
    strcat(cMailCmd, cMailCmd7);
    strcat(cMailCmd, cMailQueue->getHeader());
    if (cMailContent != NULL)
        strcat(cMailCmd, cMailContent);
    else
    {
        getTime(cTimeMail);
        strcat(cMailCmd, cTimeMail);
        strcat(cMailCmd, "Keine Meldungen vorhanden!\n");
    }
    strcat(cMailCmd, cMailQueue->getFooter());
    strcat(cMailCmd, cVersionString);
    strcat(cMailCmd, cMailCmd8);
    strcat(cMailCmd, strMail->cUser);
    strcat(cMailCmd, cMailCmd9);
    strcat(cMailCmd, strMail->cPassword);
    strcat(cMailCmd, cMailCmd10);

    /* MailKommando loggen */
    syslog(LOG_INFO,"%s%s\n", "Mail-Command: ", cMailCmd);

    /* Mail versenden */
    system(cMailCmd);

    /* Meldungen in der Queue löschen */
    cMailQueue->delAllMsg();

    /* MailContent löschen */
    if (cMailContent != NULL)
    {
        delete [] cMailContent;
        cMailQueue = NULL;
    }

    return 0;
}

/* Aktuelle Zeit als Char-Array ermitteln (2012-01-01 12:12:00) */
void getTime(char cTimeMail[])
{
    /* Aktuelle Zeit ermitteln */
    mode_t dwCurrentTime = time (0);
    struct tm* tmCurrentTime;
   /* Aktuelle Zeit in die TimeStruktur schreiben */
    tmCurrentTime = localtime ((const long int*) &dwCurrentTime);
    /* Aktuelle Zeit in das Char-Array schreiben */
    strftime(cTimeMail, (MAIL_TIME_LENGTH - 1),  MAIL_TIMESTAMP, tmCurrentTime);
    /* Zeilenumbruch anfügen */
    strncat(cTimeMail, " ", 1);
}

/* Kommandozeilenparameter analysieren */
void analyzeCommandline(int argc, char* argv[], struct strDaemon* strDaemon, struct strConfigs* strConfigs)
{
    /* Variablen */
    int iCommandLineOption=0;                                   /* Index für den Parameter im Parameter-Array       */

    /* Ausschalten der Error-Ausgabe */
    opterr = 0;

    try
    {
        /* Durchlaufen der Kommandozeilenparameter */
        while( (iCommandLineOption = getopt(argc, argv, "Dh:c:d:n:m:p:u:y:?|help")) != -1)
        {
            switch(iCommandLineOption)
            {
                /* Lesen der Option CONFIG.INI */
                case 'c':
                    strConfigs->cConfigINI = new char [strlen(optarg) + 1];
                    strcpy(strConfigs->cConfigINI, optarg);
                    break;
                /* Lesen der Option des Arbeitsverzeichnisses des Daemon */
                case 'd':
                    strDaemon->cDaemonDir = new char [strlen(optarg) + 1];
                    strcpy(strDaemon->cDaemonDir, optarg);
                    break;
                /* Lesen der Option welche Namen der Daemon haben soll */
                case 'n':
                    strDaemon->cDaemonName = new char [strlen(optarg) + 1];
                    strcpy(strDaemon->cDaemonName, optarg);
                    break;
                /* Lesen der Option der UMASK für das Arbeitsverzeichnis des Daemon */
                case 'm':
                    strDaemon->iDaemonUmaskMkdir = strtoul(optarg, NULL ,8);
                    break;
                /* Lesen der Option für die PID-Datei */
                case 'p':
                    strDaemon->cDaemonPidFile = new char [strlen(optarg) + 1];
                    strcpy(strDaemon->cDaemonPidFile, optarg);
                    break;
                /* Lesen der Option UMASK */
                case 'u':
                    strDaemon->iDaemonUmask = strtoul(optarg, NULL ,8);
                    break;
                /* Lesen der Option YASDI.INI */
                case 'y':
                    strConfigs->cYasdiINI = new char [strlen(optarg) + 1];
                    strcpy(strConfigs->cYasdiINI, optarg);
                    break;
                /* Prüfen ob das Programm als Daemon oder im Debug-Modus gestartet werden soll */
                case 'D':
                    strDaemon->iDaemonize = 0;
                    break;
                /* Benutzungshinweis ausgeben */
                case 'h':
                    PrintUsage(argc, argv);
                    exit(DAEMON_EXIT_SUCCESS);
                    break;
                /* Fehler in die Datei "daemon.log" schreiben bzw. Benutzungshinweis */
                case '?':
                    if (optopt == 'c')
                        syslog(LOG_ERR,"%s%c%s%*s\n", "Kommandozeilen-Option -", optopt, " benoetigt ein Argument", SYSLENMSG - (strlen("Kommandozeilen-Option -") + 1 + strlen(" benoetigt ein Argument")), ERROR);
                    else if (optopt == 'd')
                        syslog(LOG_ERR,"%s%c%s%*s\n", "Kommandozeilen-Option -", optopt, " benoetigt ein Argument", SYSLENMSG - (strlen("Kommandozeilen-Option -") + 1 + strlen(" benoetigt ein Argument")), ERROR);
                    else if (optopt == 'n')
                        syslog(LOG_ERR,"%s%c%s%*s\n", "Kommandozeilen-Option -", optopt, " benoetigt ein Argument", SYSLENMSG - (strlen("Kommandozeilen-Option -") + 1 + strlen(" benoetigt ein Argument")), ERROR);
                    else if (optopt == 'm')
                        syslog(LOG_ERR,"%s%c%s%*s\n", "Kommandozeilen-Option -", optopt, " benoetigt ein Argument", SYSLENMSG - (strlen("Kommandozeilen-Option -") + 1 + strlen(" benoetigt ein Argument")), ERROR);
                    else if (optopt == 'p')
                        syslog(LOG_ERR,"%s%c%s%*s\n", "Kommandozeilen-Option -", optopt, " benoetigt ein Argument", SYSLENMSG - (strlen("Kommandozeilen-Option -") + 1 + strlen(" benoetigt ein Argument")), ERROR);
                    else if (optopt == 'u')
                        syslog(LOG_ERR,"%s%c%s%*s\n", "Kommandozeilen-Option -", optopt, " benoetigt ein Argument", SYSLENMSG - (strlen("Kommandozeilen-Option -") + 1 + strlen(" benoetigt ein Argument")), ERROR);
                    else if (optopt == 'y')
                        syslog(LOG_ERR,"%s%c%s%*s\n", "Kommandozeilen-Option -", optopt, " benoetigt ein Argument", SYSLENMSG - (strlen("Kommandozeilen-Option -") + 1 + strlen(" benoetigt ein Argument")), ERROR);
                    else if (isprint (optopt))
                        syslog(LOG_ERR,"%s%c%s%*s\n", "Unbekannte Kommandozeilen-Option '-", optopt, "'", SYSLENMSG - (strlen("Unbekannte Kommandozeilen-Option '-") + 1+ strlen("'")), ERROR);
                    else
                        syslog(LOG_ERR,"%s%x%s%*s\n", "Unbekannte Kommandozeilen-Option Zeichen '\\x", optopt, "'", SYSLENMSG - (strlen("Unbekannte Kommandozeilen-Option Zeichen '\\x") + 1 + strlen("'")), ERROR);

                    PrintUsage(argc, argv);
                    exit(DAEMON_EXIT_ERROR_CMD);
                    break;
                /* Benutzungshinweis ausgeben */
                default:
                    PrintUsage(argc, argv);
                    exit(DAEMON_EXIT_ERROR_CMD);
                    break;
            }
        }

        /* Prüfen ob die YASDI-Datei per Kommandozeile übergeben wurde, wenn nicht Defaultwert benutzen */
        if (strConfigs->cYasdiINI == NULL)
        {
            strConfigs->cYasdiINI = new char [strlen(YASDIINI) + 1];
            strcpy(strConfigs->cYasdiINI, YASDIINI);
        }

       /* Prüfen ob die CONFIG-Datei per Kommandozeile übergeben wurde, wenn nicht Defaultwert benutzen */
        if (strConfigs->cConfigINI == NULL)
        {
            strConfigs->cConfigINI = new char [strlen(CONFIGINI) + 1];
            strcpy(strConfigs->cConfigINI, CONFIGINI);
        }

        /* Prüfen ob der Daemonname per Kommandozeile übergeben wurde, wenn nicht Defaultwert benutzen */
        if (strDaemon->cDaemonName == NULL)
        {
            strDaemon->cDaemonName = new char [strlen(DAEMON_NAME) + 1];
            strcpy(strDaemon->cDaemonName, DAEMON_NAME);
        }

        /* Prüfen ob die PID-Datei per Kommandozeile übergeben wurde, wenn nicht Defaultwert benutzen */
        if (strDaemon->cDaemonPidFile == NULL)
        {
            strDaemon->cDaemonPidFile = new char [strlen(DAEMON_PID_FILE) + 1];
            strcpy(strDaemon->cDaemonPidFile, DAEMON_PID_FILE);
        }

        /* Prüfen ob das Daemonarbeitsverzeichnis per Kommandozeile übergeben wurde, wenn nicht Defaultwert benutzen */
        if (strDaemon->cDaemonDir == NULL)
        {
            strDaemon->cDaemonDir = new char [strlen(DAEMON_DIR) + 1];
            strcpy(strDaemon->cDaemonDir, DAEMON_DIR);
        }

        /* Prüfen ob die UMASK per Kommandozeile übergeben wurde, wenn nicht Defaultwert benutzen */
        if (strDaemon->iDaemonUmask == 0)
            strDaemon->iDaemonUmask = DAEMON_UMASK;

        /* Prüfen ob die UMASK für das Arbeitsverzeichnis per Kommandozeile übergeben wurde, wenn nicht Defaultwert benutzen */
        if (strDaemon->iDaemonUmaskMkdir== 0)
            strDaemon->iDaemonUmaskMkdir = DAEMON_UMASK_MKDIR;
    }
    catch (bad_alloc&)
    {
        /* Freigeben des dynamisch allokierten Speichers */
        /* YASDI.INI-Datei-Zeiger prüfen */
        if (strConfigs->cYasdiINI != NULL)
        {
            delete [] strConfigs->cYasdiINI;
            strConfigs->cYasdiINI = NULL;
        }

        /* CONFIG.INI-Datei-Zeiger prüfen */
        if (strConfigs->cConfigINI != NULL)
        {
            delete [] strConfigs->cConfigINI;
            strConfigs->cConfigINI = NULL;
        }

        /* Daemonname-Zeiger prüfen */
        if (strDaemon->cDaemonName != NULL)
        {
            delete [] strDaemon->cDaemonName;
            strDaemon->cDaemonName = NULL;
        }

        /* Daemon-PID-Dateiname-Zeiger prüfen */
        if (strDaemon->cDaemonPidFile != NULL)
        {
            delete [] strDaemon->cDaemonPidFile;
            strDaemon->cDaemonPidFile = NULL;
        }

        /* Daemonarbeitsverzeichnisname-Zeiger prüfen */
        if (strDaemon->cDaemonDir != NULL)
        {
            delete [] strDaemon->cDaemonDir;
            strDaemon->cDaemonDir = NULL;
        }

        syslog(LOG_ERR,"%s%*s\n", "Speicherallokierung schlug fehl (lesen der Kommandozeilen-Optionen)", SYSLENMSG - strlen("Speicherallokierung schlug fehl (lesen der Kommandozeilen-Optionen)"), ERROR);
        syslog(LOG_ERR,"%s%*s\n", "Daemon beendet...  ", SYSLENMSG - strlen("Daemon beendet...  "), ERROR);
        exit(DAEMON_EXIT_BADALLOC);
    }
    catch (...)
    {
        /* Freigeben des dynamisch allokierten Speichers */
        /* YASDI.INI-Datei-Zeiger prüfen */
        if (strConfigs->cYasdiINI != NULL)
        {
            delete [] strConfigs->cYasdiINI;
            strConfigs->cYasdiINI = NULL;
        }

        /* CONFIG.INI-Datei-Zeiger prüfen */
        if (strConfigs->cConfigINI != NULL)
        {
            delete [] strConfigs->cConfigINI;
            strConfigs->cConfigINI = NULL;
        }

        /* Daemonname-Zeiger prüfen */
        if (strDaemon->cDaemonName != NULL)
        {
            delete [] strDaemon->cDaemonName;
            strDaemon->cDaemonName = NULL;
        }

        /* Daemon-PID-Dateiname-Zeiger prüfen */
        if (strDaemon->cDaemonPidFile != NULL)
        {
            delete [] strDaemon->cDaemonPidFile;
            strDaemon->cDaemonPidFile = NULL;
        }

        /* Daemonarbeitsverzeichnisname-Zeiger prüfen */
        if (strDaemon->cDaemonDir != NULL)
        {
            delete [] strDaemon->cDaemonDir;
            strDaemon->cDaemonDir = NULL;
        }

        syslog(LOG_ERR,"%s%*s\n", "Unbekannter Fehler (lesen der Kommandozeilen-Optionen)", SYSLENMSG - strlen("Unbekannter Fehler (lesen der Kommandozeilen-Optionen)"), ERROR);
        syslog(LOG_ERR,"%s%*s\n", "Daemon beendet...  ", SYSLENMSG - strlen("Daemon beendet...  "), ERROR);
        exit(DAEMON_EXIT_ERROR);
    }
}

/* Signal-Handler */
void signal_handler(int sig)
{
    switch(sig)
    {
        case SIGHUP:
            syslog(LOG_WARNING,"%s%*s\n", "SIGHUP Signal empfangen... ", SYSLENMSG - strlen("SIGHUP Signal empfangen... "), WARNING);
            break;
        case SIGTERM:
            syslog(LOG_ERR,"%s%*s\n", "SIGTERM Signal empfangen... ", SYSLENMSG - strlen("SIGTERM Signal empfangen... "), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);
            exit(DAEMON_EXIT_SIGTERM);
            break;
        case SIGQUIT:
            syslog(LOG_ERR,"%s%*s\n", "SIGQUIT Signal empfangen... ", SYSLENMSG - strlen("SIGQUIT Signal empfangen... "), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);
            exit(DAEMON_EXIT_SIGQUIT);
            break;
        case SIGFPE:
            syslog(LOG_ERR,"%s%*s\n", "SIGFPE Signal empfangen... ", SYSLENMSG - strlen("SIGFPE Signal empfangen... "), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);
            exit(DAEMON_EXIT_SIGFPE);
            break;
        case SIGILL:
            syslog(LOG_ERR,"%s%*s\n", "SIGILL Signal empfangen... ", SYSLENMSG - strlen("SIGILL Signal empfangen... "), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);
            exit(DAEMON_EXIT_SIGILL);
            break;
        case SIGSEGV:
            syslog(LOG_ERR,"%s%*s\n", "Received SIGSEGV signal... ", SYSLENMSG - strlen("Received SIGSEGV signal... "), ERROR);
            syslog(LOG_ERR,"%s%*s\n", "Daemon beendet... ", SYSLENMSG - strlen("Daemon beendet... "), ERROR);
            exit(DAEMON_EXIT_SIGSEGV);
            break;
        default:
            syslog(LOG_WARNING, "Unbekanntes Signal (%d) %s", sig, strsignal(sig));
            break;
    }
}

/* Methode für die Freigabe des dynamisch allokierten Speichers */
void free_memory (struct strStatus* strStatus, struct strConfigs* strConfigs, struct strDaemon* strDaemon, struct strMySQL* strMySQL, struct strMail* strMail, cStatusDevice* cStatusDevices)
{
    /* Struktur strStatus freigeben */
    if (strStatus->cStatusChannel != NULL)
    {
        delete [] strStatus->cStatusChannel;
        strStatus->cStatusChannel = NULL;
    }

    if (strStatus->cStatusValue != NULL)
    {
        delete [] strStatus->cStatusValue;
        strStatus->cStatusValue = NULL;
    }

    /* Struktur strConfigs freigeben */
    if (strConfigs->cYasdiINI != NULL)
    {
        delete [] strConfigs->cYasdiINI;
        strConfigs->cYasdiINI = NULL;
    }

    if (strConfigs->cConfigINI != NULL)
    {
        delete [] strConfigs->cConfigINI;
        strConfigs->cConfigINI = NULL;
    }

    /* Struktur strDaemon freigeben */
    if (strDaemon->cDaemonName != NULL)
    {
        delete [] strDaemon->cDaemonName;
        strDaemon->cDaemonName = NULL;
    }

    if (strDaemon->cDaemonPidFile != NULL)
    {
        delete [] strDaemon->cDaemonPidFile;
        strDaemon->cDaemonPidFile = NULL;
    }

    if (strDaemon->cDaemonDir != NULL)
    {
        delete [] strDaemon->cDaemonDir;
        strDaemon->cDaemonDir = NULL;
    }

    /* Struktur MySQL freigeben */
    if (strMySQL->cHost != NULL)
    {
      delete [] strMySQL->cHost;
      strMySQL->cHost = NULL;
    }

    if (strMySQL->cPort != NULL)
    {
      delete [] strMySQL->cPort;
      strMySQL->cPort= NULL;
    }

    if (strMySQL->cUser != NULL)
    {
      delete [] strMySQL->cUser;
      strMySQL->cUser = NULL;
    }

    if (strMySQL->cPassword != NULL)
    {
      delete [] strMySQL->cPassword;
      strMySQL->cPassword = NULL;
    }

    if (strMySQL->cDatabase != NULL)
    {
      delete [] strMySQL->cDatabase;
      strMySQL->cDatabase = NULL;
    }

    if (strMySQL->cDataTable != NULL)
    {
      delete [] strMySQL->cDataTable;
      strMySQL->cDataTable = NULL;
    }

    if (strMySQL->cSNTable != NULL)
    {
      delete [] strMySQL->cSNTable;
      strMySQL->cSNTable = NULL;
    }

    /* Struktur strMail freigeben */
    if (strMail->cServer != NULL)
    {
      delete [] strMail->cServer;
      strMail->cServer = NULL;
    }

    if (strMail->cPort!= NULL)
    {
      delete [] strMail->cPort;
      strMail->cPort = NULL;
    }

    if (strMail->cTls != NULL)
    {
      delete [] strMail->cTls;
      strMail->cTls = NULL;
    }

    if (strMail->cFrom != NULL)
    {
      delete [] strMail->cFrom;
      strMail->cFrom = NULL;
    }

    if (strMail->cTo != NULL)
    {
      delete [] strMail->cTo;
      strMail->cTo = NULL;
    }

    if (strMail->cUser != NULL)
    {
      delete [] strMail->cUser;
      strMail->cUser = NULL;
    }

    if (strMail->cPassword != NULL)
    {
      delete [] strMail->cPassword;
      strMail->cPassword = NULL;
    }

    /* StatusDevices-Objekte freigeben */
    if (cStatusDevices != NULL)
    {
        delete [] cStatusDevices;
        cStatusDevices = NULL;
    }
}
