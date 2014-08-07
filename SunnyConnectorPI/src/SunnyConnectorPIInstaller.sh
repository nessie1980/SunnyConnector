#!/bin/bash
###################################################################################################
#
# Diese Script installiert den YASDI-Treiber der Firma "SMA"
#
# Es prueft ob alles noetigen Programme vorhanden sind.
# Falls dies nicht der Fall sein sollte, laed das Script die noetigen Programme
# per "apt-get" herunter und installiert diese.
# Anschliessend wird das YASDI-Treiber-Archiv entpackt und der YASDI-Treiber installiert.
# Die komplette Installation wird per Logdatei dokumentiert.
#
###################################################################################################

###################################################################################################
# Variablen                                                                                       #
###################################################################################################
ARCHIV="yasdi-1.8.1build9-src.zip"                      # Archiv mit dem SMA-Treiber  (veraenderbar)
SCRIPTPATH=$(cd -P "$( dirname "$0" )" && pwd )"/"      # Verzeichnis des Scripts   (unveraenderbar)
DEST=$SCRIPTPATH"yasdi"                                 # Verzeichnis zum Entpacken   (veraenderbar)
LOGFILE=$SCRIPTPATH"SunnyConnectorPIInstaller.log"      # Name der Logdatei         (unveraenderbar)
DATE=`date +"%d.%b.%Y um %H:%M:%S"`                     # Datum (formatiert)        (unveraenderbar)
URL="www.google.com"                                    # URL fuer den I-Net-Test     (veraenderbar)
SUDO=0                                                  # Flag ob sudo installiert	(unveraenderbar)
UNZIP="unzip"                                           # Programm unzip            (unveraenderbar)
GCC="gcc"                                               # Compiler                  (unveraenderbar)
CMAKE="cmake"                                           # Make                      (unveraenderbar)
YASDI_LD="yasdi"                                        # YASDI ldconfig-Wert       (unveraenderbar)
YASDI=""                                                # YASDI ldconfig-Result     (unveraenderbar)
MYSQL_APT="libmysqlclient16"                            # Libary fuer die MySQL-Verb. (veraenderbar)
MYSQL_LIB="libmysqlclient.so.16"                        # Libary zum Pruefen          (veraenderbar)
MYSQL_LD="libmysqlclient.so.16"                         # MYSQL ldconfig-Wert		  (veraenderbar)
MYSQL=""                                                # MYSQL ldconfig-Result     (unveraenderbar)
ERROR_COUNTER=0                                         # Zaehlervariable           (unveraenderbar)
INPUT=""                                                # Eingabevariable           (unveraenderbar)
FLAGINPUT=0                                             # Flag ob Eingabe gemacht   (unveraenderbar)
DIRCETROY_FLAG=0                                        # Flag,Verzeichnis existiert(unveraenderbar)
DESCRIPTION="   Diese Script installiert den SunnyConnector zur Datenerfassung von SMA-Wechselrichtern."
BUILD=1
MINOR=1
MAJOR=1

###################################################################################################
# Pruefen ob das Script erfolgreich ausgefuehrt wurde                                               #
###################################################################################################
function_summary ()
{
 echo "### Installtion fertig                                                           ###" | tee -a $LOGFILE
 echo "" | tee -a $LOGFILE
 echo "####################################################################################" | tee -a $LOGFILE
 echo "" | tee -a $LOGFILE
 if [ "$ERROR_COUNTER" = "0" ]
    then
      echo "   Es trat kein Fehler auf..." | tee -a $LOGFILE
      echo "" | tee -a $LOGFILE
      echo "   Das Installations-Script wurde erfolgreich ausgefuehrt!" | tee -a $LOGFILE
      echo "" | tee -a $LOGFILE
      echo "####################################################################################" | tee -a $LOGFILE
      echo "" | tee -a $LOGFILE
 else
      echo "   $ERROR_COUNTER Fehler aufgetreten." | tee -a $LOGFILE
      echo "   Siehe Bildschirmausgabe oder Log-Datei \"$LOGFILE\"." | tee -a $LOGFILE
      echo "" | tee -a $LOGFILE
      echo "   Das Installations-Script wurde nicht erfolgreich ausgefuehrt!" | tee -a $LOGFILE
      echo "" | tee -a $LOGFILE
      echo "####################################################################################" | tee -a $LOGFILE
      echo "" | tee -a $LOGFILE
      exit 1
 fi
}

###################################################################################################
# Pruefen ob das Script als ROOT ausgefuehrt wird                                                   #
###################################################################################################
if [ ! `whoami | grep root` ]
   then
     echo ""
     echo "   Das Installations-Script muss als ROOT ausgefuehrt werden \"sudo ./SunnyConnectorPIInstaller.sh\"."
     echo "   Script wird beendet!"
     echo ""
     exit 0
fi

###################################################################################################
# Pruefen ob "sudo" vorhanden ist                                                                  #
###################################################################################################
if [ `find /usr/bin | grep "sudo" | wc -c` == "0" ]
   then
 	 SUDO="0"
fi

###################################################################################################
# Pruefen ob ein Parameter mituebergeben wurde                                                      #
###################################################################################################
if [[ "$1" -ne 0 || "$1" = "help" || "$1" != "install" && "$1" != "version" ]]
   then
     echo ""
     echo "   Benutzung: sudo ./SunnyconnectorPIInstaller.sh | install version help"
     echo ""
     echo "$DESCRIPTION"
     echo ""
     exit 0
fi

###################################################################################################
# Pruefen ob die Version ausgegeben werden soll                                                     #
###################################################################################################
if [[ "$1" -ne 0 || "$1" = "version" || "$1" != "install" ]]
   then
     echo ""
     printf '   Version des Installations-Scripts: %d.%03d.%04d\n' "$MAJOR" "$MINOR" "$BUILD";
     echo ""
     exit 0
fi


###################################################################################################
# Logdatei anlegen                                                                                #
###################################################################################################
echo "####################################################################################"
echo ""
echo "### Log-File wird angelegt                                                       ###"
touch $LOGFILE 2> /dev/null
if [ "$?" != "0" ]
   then
     echo "   Log-File konnte nicht angelegt werden!"
     echo "   Script wird beendet!"
     echo ""
     exit 1
fi

echo "" | tee -a $LOGFILE
echo "####################################################################################" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "   Log-File vom "$DATE"" | tee $LOGFILE
echo "   $LOGFILE" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "####################################################################################" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE

###################################################################################################
# Pruefen ob eine Internet-Verbindung besteht                                                      #
###################################################################################################

echo "### Internetkonnektivitaet wird getestet                                         ###" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "####################################################################################" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "   ping -c 3 $URL" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
if [[ `ping -c 3 $URL | grep ttl` ]]
   then
     echo "   Internetverbindung vorhanden..." | tee -a $LOGFILE
else
     echo "   Keine Internetverbindung vorhanden!" | tee -a $LOGFILE
     INPUT=""
     FLAGINPUT=0
     while [ "$INPUT" != "JA" -a "$INPUT" != "J" -a "$INPUT" != "NEIN" -a "$INPUT" != "N" ]
      do
        if [ "$INPUT" != "JA" -a "$INPUT" != "J" -a "$INPUT" != "NEIN" -a "$INPUT" != "N" -a "$FLAGINPUT" == "1" ]
           then
             echo "   Fehlerhafte Eingabe!"
        fi
        echo -n "   Moechten Sie dennoch mit dem Script fortfahren? [Ja|Nein]: "
        read INPUT
        INPUT=`echo "$INPUT" | tr '[:lower:]' '[:upper:]'`
        FLAGINPUT=1
     done
     if [ "$INPUT" == "NEIN" -o "$INPUT" == "N"  -o "$INPUT" == "" ]
        then
          echo "   Moechten Sie dennoch mit dem Script fortfahren? [Ja|Nein]: $INPUT" >> $LOGFILE
          if [ "$INPUT" == "" ]
             then
               echo "" | tee -a $LOGFILE
          fi
          echo "" | tee -a $LOGFILE
          echo "####################################################################################" | tee -a $LOGFILE
          echo "" | tee -a $LOGFILE
          ERROR_COUNTER="1"
          function_summary
     else
          echo "   Moechten Sie dennoch mit dem Script fortfahren? [Ja|Nein]: $INPUT" >> $LOGFILE
          echo "" | tee -a $LOGFILE
     fi
fi
echo "" | tee -a $LOGFILE
echo "####################################################################################" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE

###################################################################################################
# Pruefen ob die ZIP-Datei existiert                                                               #
###################################################################################################
echo "### Pruefen ob die Datei \"$ARCHIV\" vorhanden ist               ###" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "####################################################################################" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
if [ -f "$ARCHIV" ]
   then
     echo "   Datei \"$ARCHIV\" existiert..." | tee -a $LOGFILE
else
     echo "   Datei \"$ARCHIV\" existiert nicht!" | tee -a $LOGFILE
     echo "   Script wird beendet!" | tee -a $LOGFILE
     echo "" | tee -a $LOGFILE
     echo "####################################################################################" | tee -a $LOGFILE
     echo "" | tee -a $LOGFILE
     ERROR_COUNTER="1"
     function_summary
fi
echo "" | tee -a $LOGFILE
echo "####################################################################################" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE

###################################################################################################
# Installations-Quellen aktualisieren                                                             #
###################################################################################################
echo "### Installations-Quellen aktualisieren                                          ###" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "####################################################################################" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
if [ "$SUDO" == "1" ]
   then
     echo "   sudo apt-get update" | tee -a $LOGFILE
     echo "" | tee -a $LOGFILE
     sudo apt-get update | tee -a $LOGFILE
else
     echo "   apt-get update" | tee -a $LOGFILE
     echo "" | tee -a $LOGFILE
     apt-get update | tee -a $LOGFILE
fi
echo "" | tee -a $LOGFILE
echo "####################################################################################" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE


###################################################################################################
# Pruefen ob Programme installiert sind                                                            #
###################################################################################################
echo "### Pruefen ob alle noetigen Programme installiert sind                          ###" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "####################################################################################" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
for PROG in $UNZIP $GCC $CMAKE
 do
  if [ `which $PROG | wc -c` == "0" ]
     then
       echo "   Programm \"$PROG\" ist nicht installiert!" | tee -a $LOGFILE
       INPUT=""
       FLAGINPUT=0
       while [ "$INPUT" != "JA" -a "$INPUT" != "J" -a "$INPUT" != "NEIN" -a "$INPUT" != "N" ]
        do
          if [ "$INPUT" != "JA" -a "$INPUT" != "J" -a "$INPUT" != "NEIN" -a "$INPUT" != "N" -a "$FLAGINPUT" == "1" ]
             then
               echo "   Fehlerhafte Eingabe!"
          fi
          echo -n "   Moechten Sie das Programm jetzt installieren? [Ja|Nein]:"
          read INPUT
          INPUT=`echo "$INPUT" | tr '[:lower:]' '[:upper:]'`
          FLAGINPUT=1
       done
       if [ "$INPUT" == "JA" -o "$INPUT" == "J" ]
        then
          echo "   Moechten Sie das Programm jetzt installieren? [Ja|Nein]: $INPUT" >> $LOGFILE
          if [ "$SUDO" == "1" ]
             then
               echo "   sudo apt-get -y install $PROG" | tee -a $LOGFILE
               echo "" | tee -a $LOGFILE
               sudo apt-get -y install "$PROG" | tee -a $LOGFILE
          else
               echo "   apt-get install $PROG" | tee -a $LOGFILE
               echo "" | tee -a $LOGFILE
               apt-get -y install "$PROG" | tee -a $LOGFILE
          fi
          echo "" | tee -a $LOGFILE
	      if [ "$?" != "0" ]
             then
               echo "" | tee -a $LOGFILE
               echo "####################################################################################" | tee -a $LOGFILE
               echo "" | tee -a $LOGFILE
               ERROR_COUNTER="1"
               function_summary
	      else
	           echo "   Programm \"$PROG\" wurde erfolgreich installiert..." | tee -a $LOGFILE
          fi
       else
          echo "   Moechten Sie das Programm jetzt installieren? [Ja|Nein]: $INPUT" >> $LOGFILE
          if [ "$INPUT" == "" ]
             then
               echo "" | tee -a $LOGFILE
          fi
          echo "   Script wird beendet!" | tee -a $LOGFILE
          echo "" | tee -a $LOGFILE
          echo "####################################################################################" | tee -a $LOGFILE
          echo "" | tee -a $LOGFILE
          ERROR_COUNTER="1"
          function_summary
       fi
  else
      echo "   Programm \"$PROG\" ist bereits installiert..." | tee -a $LOGFILE
  fi
done
echo "" | tee -a $LOGFILE
echo "####################################################################################" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE

###################################################################################################
# ZIP-Datei entpacken                                                                             #
###################################################################################################
echo "### Entpacken der ZIP-Datei \"$ARCHIV\"                          ###" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "####################################################################################" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "   unzip -o -d $DEST $ARCHIV" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
unzip -o -d $DEST $ARCHIV | tee -a $LOGFILE 2> /dev/null
if [ "$?" != "0" ]
 then
   echo "" | tee -a $LOGFILE
   echo "   Das Entpacken schlug fehl!" | tee -a $LOGFILE
   echo "   Script wird beendet!" | tee -a $LOGFILE
   echo "" | tee -a $LOGFILE
   echo "####################################################################################" | tee -a $LOGFILE
   echo "" | tee -a $LOGFILE
   ERROR_COUNTER="1"
   function_summary
 else
   echo "" | tee -a $LOGFILE
   echo "   Das Entpacken war erfolgreich..." | tee -a $LOGFILE
fi
echo "" | tee -a $LOGFILE
echo "####################################################################################" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE

###################################################################################################
# Installieren des YASDI-Treibers                                                                 #
###################################################################################################
echo "### Installation des YASDI-Treibers der Firma \"SMA\"                              ###" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "####################################################################################" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
if [ ! -d "$DEST/projects/generic-cmake/build-gcc" ]
   then
     echo "   $DEST/projects/generic-cmake/build-gcc existiert nicht! Verzeichnis wird angelegt..." | tee -a $LOGFILE
     echo "" | tee -a $LOGFILE
     echo "   mkdir $DEST/projects/generic-cmake/build-gcc" | tee -a $LOGFILE
     mkdir $DEST/projects/generic-cmake/build-gcc
     if [ "$?" == 0 ]
        then
          echo "" | tee -a $LOGFILE
          echo "   Verzeichnis wurde angelegt." | tee -a $LOGFILE
          echo "" | tee -a $LOGFILE
          DIRECTORY_FLAG=1
     else
          echo "" | tee -a $LOGFILE
          echo "   Verzeichnis konnte nicht angelegt werden!" | tee -a $LOGFILE
          echo "   Script wird beendet!" | tee -a $LOGFILE
          echo "" | tee -a $LOGFILE
          echo "####################################################################################" | tee -a $LOGFILE
          echo "" | tee -a $LOGFILE
          ERROR_COUNTER="1"
          function_summary
    fi
else
    DIRECTORY_FLAG=1
fi

if [[ $DIRECTORY_FLAG -eq 1 ]]
   then
     echo "   cd $DEST/projects/generic-cmake/build-gcc" | tee -a $LOGFILE
     cd "$DEST/projects/generic-cmake/build-gcc"
     echo "" | tee -a $LOGFILE
     echo "   cmake .." | tee -a $LOGFILE
     echo "" | tee -a $LOGFILE
     cmake .. | tee -a $LOGFILE
     echo "" | tee -a $LOGFILE
     if [ "$?" == "0" ]
        then
          echo "   cmake war erfolgreich..." | tee -a $LOGFILE
          echo "" | tee -a $LOGFILE
          echo "   make" | tee -a $LOGFILE
          echo "" | tee -a $LOGFILE
          make | tee -a $LOGFILE
          echo "" | tee -a $LOGFILE
          if [ "$?" == 0 ]
             then
               echo "   make war erfolgreich..." | tee -a $LOGFILE
               echo "" | tee -a $LOGFILE
               if [ "$SUDO" == 1 ]
                  then
                    echo "   sudo make install" | tee -a $LOGFILE
	                echo "" | tee -a $LOGFILE
                    sudo make install | tee -a $LOGFILE
               else
                    echo "   make install" | tee -a $LOGFILE
	                echo "" | tee -a $LOGFILE
                    make install | tee -a $LOGFILE
               fi
               echo "" | tee -a $LOGFILE
               if [ "$?" == 0 ]
                  then
                    echo "   make install war erfolgreich..." | tee -a $LOGFILE
                    echo "" | tee -a $LOGFILE
	                echo "   ldconfig" | tee -a $LOGFILE
	                echo "" | tee -a $LOGFILE
                    ldconfig | tee -a $LOGFILE
                    YASDI=`ldconfig -p | grep $YASDI_LD | wc -c`
	                if [ "$YASDI" -gt 0 ]
                       then
                         echo "   ldconfig wurde erfolgreich ausgefuehrt..." | tee -a $LOGFILE
	                else
                         echo "   ldconfig schlug fehl!" | tee -a $LOGFILE
                         ERROR_COUNTER="1"
 	                fi
	           else
                    echo "   make install schlug fehl!" | tee -a $LOGFILE
                    echo "   Script wird beendet!" | tee -a $LOGFILE
                    echo "" | tee -a $LOGFILE
                    echo "####################################################################################" | tee -a $LOGFILE
                    echo "" | tee -a $LOGFILE
                    ERROR_COUNTER="1"
                    function_summary
               fi
          else
               echo "   make schlug fehl!" | tee -a $LOGFILE
               echo "   Script wird beendet!" | tee -a $LOGFILE
               echo "" | tee -a $LOGFILE
               echo "####################################################################################" | tee -a $LOGFILE
               echo "" | tee -a $LOGFILE
               ERROR_COUNTER="1"
               function_summary
          fi
     else
          echo "   cmake schlug fehl!" | tee -a $LOGFILE
          echo "   Script wird beendet!" | tee -a $LOGFILE
          echo "" | tee -a $LOGFILE
          echo "####################################################################################" | tee -a $LOGFILE
          echo "" | tee -a $LOGFILE
          ERROR_COUNTER="1"
          function_summary
     fi
else
    echo "   $DEST/projects/generic-cmake/build-gcc existiert nicht!" | tee -a $LOGFILE
    echo "   Script wird beendet!" | tee -a $LOGFILE
fi
echo "" | tee -a $LOGFILE
echo "####################################################################################" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE

###################################################################################################
# Installieren der MySQL-Libary                                                                   #
###################################################################################################
echo "### Installation der MySQL-Bibliothek                                            ###" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "####################################################################################" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
if [ `find / | grep "$MYSQL_LIB" | wc -c` == "0" ]
   then
     echo "   MySQL-Bibliothek ist nicht installiert!" | tee -a $LOGFILE
     INPUT=""
     FLAGINPUT=0
     while [ "$INPUT" != "JA" -a "$INPUT" != "J" -a "$INPUT" != "NEIN" -a "$INPUT" != "N" ]
      do
        if [ "$INPUT" != "JA" -a "$INPUT" != "J" -a "$INPUT" != "NEIN" -a "$INPUT" != "N" -a "$FLAGINPUT" == "1" ]
           then
             echo "   Fehlerhafte Eingabe!"
        fi
        echo -n "   Moechten Sie dennoch mit dem Script fortfahren? [Ja|Nein]: "
        read INPUT
        INPUT=`echo "$INPUT" | tr '[:lower:]' '[:upper:]'`
        FLAGINPUT=1
     done
     if [ "$INPUT" == "JA" -o "$INPUT" == "J" ]
      then
        echo "   Moechten Sie die Bibliothek nun installieren? [Ja|Nein]: $INPUT" >> $LOGFILE
        if [ "$SUDO" == "1" ]
           then
             echo "   sudo apt-get -y install $MYSQL_APT" | tee -a $LOGFILE
             echo "" | tee -a $LOGFILE
             sudo apt-get -y install "$MYSQL_APT" | tee -a $LOGFILE
        else
             echo "   apt-get -y install $MYSQL_APT" | tee -a $LOGFILE
             echo "" | tee -a $LOGFILE
             apt-get -y install "$MYSQL_APT" | tee -a $LOGFILE
        fi
        echo "" | tee -a $LOGFILE
	    if [ "$?" != "0" ]
           then
	         echo "   Bibliothek konnte nicht installiert werden!" | tee -a $LOGFILE
	         echo "   Script wird beendet!" | tee -a $LOGFILE
	         echo "" | tee -a $LOGFILE
	         echo "####################################################################################" | tee -a $LOGFILE
	         echo "" | tee -a $LOGFILE
             ERROR_COUNTER="1"
             function_summary
	    else
             echo "   Bibliothek wurde erfolgreich installiert..." | tee -a $LOGFILE
             echo "" | tee -a $LOGFILE
             echo "   ldconfig" | tee -a $LOGFILE
             echo "" | tee -a $LOGFILE
             ldconfig | tee -a $LOGFILE
             MYSQL=`ldconfig -p | grep $MYSQL_LD | wc -c`
	         if [ "$MYSQL" -gt 0 ]
                then
                  echo "   ldconfig wurde erfolgreich ausgefuehrt..." | tee -a $LOGFILE
	         else
                  echo "   ldconfig schlug fehl!" | tee -a $LOGFILE
                  ERROR_COUNTER="1"
 	         fi
        fi
     else
        echo "   Moechten Sie die Bibliothek nun installieren? [Ja|Nein]: $INPUT" >> $LOGFILE
        if [ "$INPUT" == "" ]
           then
             echo "" | tee -a $LOGFILE
        fi
        echo "" | tee -a $LOGFILE
        echo "####################################################################################" | tee -a $LOGFILE
        echo "" | tee -a $LOGFILE
        ERROR_COUNTER="1"
        function_summary
     fi
else
     echo "   Bibliothek ist bereits installiert..." | tee -a $LOGFILE
fi
echo "" | tee -a $LOGFILE
echo "####################################################################################" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE

###################################################################################################
# Konfigurationsdateien erstellen                                                                 #
###################################################################################################
echo "### Konfigurator fuer den SunnyConnectorPI ausfuehren                            ###" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE
echo "####################################################################################" | tee -a $LOGFILE
cd $SCRIPTPATH
if [ "$?" != "0" ]
   then
   echo "" | tee -a $LOGFILE
   echo "   Konnte nicht ins Verzeichnis \"$SCRIPTPATH\" wechseln!" | tee -a $LOGFILE
   echo "" | tee -a $LOGFILE
   echo "####################################################################################" | tee -a $LOGFILE
   echo "" | tee -a $LOGFILE
   ERROR_COUNTER="1"
   function_summary
else
   echo "" | tee -a $LOGFILE
   echo "   Hierfuer muss die Anlage bzw. alle Wechselrichter ONLINE sein!" | tee -a $LOGFILE
   echo "   Den der Konfigratior muss auf die Wechselrichter zugegriffen." | tee -a $LOGFILE
   echo "   Die Konfiguration kann jedoch auch zu einem spaeteren Zeitpunkt ausgefuehrt werden." | tee -a $LOGFILE
   INPUT=""
   FLAGINPUT=0
   while [ "$INPUT" != "JA" -a "$INPUT" != "J" -a "$INPUT" != "NEIN" -a "$INPUT" != "N" ]
    do
      if [ "$INPUT" != "JA" -a "$INPUT" != "J" -a "$INPUT" != "NEIN" -a "$INPUT" != "N" -a "$FLAGINPUT" == "1" ]
         then
           echo "   Fehlerhafte Eingabe!"
      fi
      echo -n "   SunnyConnectorPI jetzt konfigurieren? [Ja|Nein]: "
      read INPUT
      INPUT=`echo "$INPUT" | tr '[:lower:]' '[:upper:]'`
      FLAGINPUT=1
   done
   if [ "$INPUT" == "JA" -o "$INPUT" == "J" ]
    then
      echo "   SunnyConnectorPI jetzt konfigurieren? [Ja|Nein]: $INPUT" >> $LOGFILE

      ./SunnyConnectorPIConfigurator -l $LOGFILE
      if [ "$?" != "0" ]
         then
           echo "   Konfiguration schlug fehl!" | tee -a $LOGFILE
           echo "" | tee -a $LOGFILE
           echo "####################################################################################" | tee -a $LOGFILE
           echo "" | tee -a $LOGFILE
           ERROR_COUNTER="1"
           function_summary
      else
           echo "" | tee -a $LOGFILE
           echo "   Konfiguration war erfolgreich..." | tee -a $LOGFILE
           echo "" | tee -a $LOGFILE
      fi
  else
      echo "   SunnyConnectorPI jetzt konfigurieren? [Ja|Nein]: $INPUT" >> $LOGFILE
      echo "" | tee -a $LOGFILE
      echo "   Konfigurator wurde nicht ausgefuehrt..." | tee -a $LOGFILE
      echo "" | tee -a $LOGFILE
  fi
fi
echo "####################################################################################" | tee -a $LOGFILE
echo "" | tee -a $LOGFILE

###################################################################################################
# Pruefen ob das Script erfolgreich ausgefuehrt wurde                                               #
###################################################################################################
function_summary
