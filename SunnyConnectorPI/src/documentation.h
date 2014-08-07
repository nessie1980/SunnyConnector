/**
* @file            documentation.h
* @brief           Dokumentation für das Projekt.
* @details         Hier wird die Dokumentation für das Programm "SunnyConnectorPI"
* @details         niedergeschrieben.
* @details
*/

#ifndef DOCUMENTATION_H_
#define DOCUMENTATION_H_

/*! @mainpage Dokumentation des SunnyConnectorPI
 *
 * @section introduction Einführung in das Programm SunnyConnectorPI
 *
 * @subsection Kurzbeschreibung
 * SunnyConnectorPI ist ein Programm, welches auf einem 32-bit Linux-OS-System (getestet auf Ubuntu 10.04) läuft.
 * Die Software wird verwendet um Daten aus einem oder mehreren SMA-Wechselrichter(n) einer Solar-Anlage zu lesen.
 * Für die Kommunikation mit dem Wechselrichtern wird die offene Bibliothek <a href="http://www.sma.de/de/produkte/software/yasdi.html">YASDI</a> von der Firma SMA verwendet.
 *
 * @subsection block_diagram Blockschaltbild
 * Das \ref page1 zeigt den eigentlichen Aufbau einer Anlage mit allen nötigen bzw. möglichen Bestandteilen.
 *
 * @section programm_parts Bestandteile des Programms
 * <ul>
 *  <li>
 *  \ref page2 "SunnyConnectorPI" ist ein Linux-Daemon-Programm, das zur Datenerfassung verwendet wird.
 *  </li>
 *  <li>
 *  \ref page3 "SunnyConnectorPId" ist ein Bashscript zum Bedienen des SunnyConnectorPI-Daemons.
 *  </li>
 *  <li>
 *  \ref page4 "CONFIG.INI" ist die Konfigurationsdatei für den SunnyConnectorPI-Daemon.
 *  </li>
 *  <li>
 *  \ref page5 "YASDI.INI" ist die Konfigurationsdatei für den YASDI-Treiber welche vom SunnyConnectorPI-Daemon verwendet wird.
 *  </li>
 * </ul>
 *
 * @section development_environment Einstellungen für die Entwicklungsumgebung (Eclipse)
 * Nötige \ref page6 werden hier aufgezeigt. Ebenfalls wird erläutert welche weitere Software nötig ist.
 *
 * @section mantis Bugtracker "Mantis"
 * Unter folgender URL <a href="http://www.tbarth.eu/mantis/">Mantis-Bugtracker</a> findet man
 * den Bugtracker zum Projekt.
 *
 * @section sources Quellen-Angabe
 * <ul>
 *  <li>
 *  <a href="http://www.linuxcertif.com/man/8/start-stop-daemon/de/">Linux Certif - Man start-stop-daemon(8)</a>
 *  </li>
 *  <li>
 *  <a href="http://peterlombardo.wikidot.com/linux-daemon-in-c">Linux Daemon In C </a>
 *  </li>
 *  <li>
 *  <a href="http://www.lainoox.com/how-to-daemonize-a-process-in-c/">How To: Daemonize a Process in C</a>
 *  </li>
 *  <li>
 *  <a href="http://pronix.linuxdelta.de/C/Linuxprogrammierung/index.shtml">Linux/Unix-Systemprogrammierung</a>
 *  </li>
 *  <li>
 *  <a href="http://content.hccfl.edu/pollock/aunix2/logging.htm">Logging, Log File Rotation, and Syslog Tutorial </a>
 *  </li>
 *  <li>
 *  <a href="http://www.infodrom.org/~joey/Writing/Linux-Magazin/syslog-workshop.html">Linux-Magazin: Syslog</a>
 *  </li>
 * </ul>
 */

/*! @page page1 Blockschaltbild
 *
 * @tableofcontents
 * @section own Beispiel für ein mögliches Blockschaltbild
 * @image html LATEX_Schema_Aufbau.jpg
 * @image latex LATEX_Schema_Aufbau.jpg "Blockdiagramm" width=11cm
 *
 * @section description_block Beschreibung des Blockschaltbildes
 * Auf der rechten Seite befinden sich die Solarpanels mit den Wechselrichtern, in diesem Fall drei Stück. Es kann jedoch auch nur ein Wechselrichter
 * bzw. mehr als drei verbaut sein. Allerdings nicht mehr als 256, denn ein RS485-Bus unterstützt nicht mehr als 256 Geräte.
 * Bewegen wir uns zum nächsten Bauteil der Anlage, so finden wir einen RS485-RS232-Konverter. Dieser ist nötig da die Wechselrichter von SMA mit
 * RS485-Schnittstellen ausgestattet sind bzw. nachgerüstet werden können und ein handelsüblicher Rechner meist
 * nur eine oder zwei RS232-Schnittstellen besitzt. Um nun die beiden unterschiedlichen Schnittstellen miteinander zu verbinden, wird dieser Konverter
 * eingesetzt. Nachdem Bus-Konverter folgt der PC auf dem das Programm SunnyConnectorPI läuft und für das Erfassen der Daten aus den Wechselrichter
 * verantwortlich ist. Da es mit dem Programm möglich ist die Daten auf einen MySQL-Server zu schreiben, kann dieser optional auf der gleichen Hardware
 * installiert werden. Wie man aber dem Schaltbild entnehmen kann, ist es aber auch möglich den MySQL-Server auf einer entfernten Hardware zu
 * installieren bzw. hat man evtl. schon einen laufenden MySQL-Server, dann kann dieser ebenfalls verwendet werden. In den optionalen Bereichen findet
 * man des weiteren noch einen PHP-Server, dieser wird benötigt wenn die zusätzliche Software SunnyConnectorAnaylzer zum Auswerten der Daten verwendet wird.
 */

/*! @page page2 Dokumentation des Programms "SunnyConnectorPI"
 *
 * @tableofcontents
 *  @section description_prog Kurzbeschreibung des Programms
 *  SunnyConnectorPI ist ein Linux-Daemon zur Wechselrichterdatenerfassung und wird mit Hilfe des Daemon-Scripts "sunnyconnetorpid" (\ref page2) bedient.
 *  Das Programm ermöglicht es dem Benutzer Daten aus SMA-Wechselrichtern in einem vom Benutzer vorgegebenen Interval zu lesen und anschliessend in
 *  eine Datei und/oder MySQL-Server zu schreiben. Die Startparameter, welche bestimmen was genau gespeichert werden soll und wohin, befinden sich
 *  in zwei Konfigurationsdateien.<br>
 *  <ul>
 *      <li>
 *          yasdi.ini (\ref page4)
 *      </li>
 *      <li>
 *          config.ini (\ref page5)
 *      </li>
 *  </ul>
 *  Weitere Infos bezüglich der Einstellungsmöglichkeiten in den genannten Dateien findet man in den entsprechenden Dokumentationen der Dateien.
 *
 *  @section function Arbeitsweise des Programms
 *  Die Konfigurationsdateien werden beim Start vom Programm eingelesen und bestimmen somit die Funktionsweise bzw. Arbeitsweise des Programms. Sollte
 *  ein Parameter nicht vorhanden sein, wird ein Defaultparameter verwendet (siehe \ref defines.h). Nachdem der Daemon gestartet wurde, beginnt er mit
 *  Hilfe des <b>YASDI-Treiber</b> auf dem angeschlossenen RS485-Bus nach Wechselrichtern zu suchen. Die Suche wird solange immer wieder ausgeführt bis
 *  alle in der <b>CONFIG.INI</b> definierten Wechselrichter vom Programm gefunden wurden. Sollte ein Wechselrichter nicht erreichbar sein, verweilt das
 *  Programm nicht im Suchprozess bis dieser erreichbar ist, sondern bricht nach einem in der YASDI-Bibliothek vordefinierten Wert (6) die Suche ab.
 *  Nach dem abgeschlossenen Suchvorgang wird sofort versucht eine Verbindung zu den (bereits) gefundenen Wechselrichtern aufzubauen und zu erfragen ob
 *  der Wechselrichter aktiv Strom einspeist. Ist der Wechselrichter im Einspeisezustand so werden weitere von Benutzer in der <b>CONFIG.INI</b>
 *  konfigurierten Daten vom Wechselrichter abgefragt. Diese so ermittelten Werte werden dann je nach Einstellung in eine Datei und/oder in einen
 *  MySQL-Server gespeichert. Sobald der beschriebene Prozess abgeschlossen ist, wird ein Timer gestartet, dessen Zeit in der <b>CONFIG.INI</b> definiert ist.
 *  Wenn nun dieser Timer abgelaufen ist, wird der Prozess von neuem angestoßen.<br><br>
 *  Wenn noch nicht alle Wechselrichter erkannt worden sind, wird ein erneuter Suchprozess initiert. Andernfalls wird sofort mit den Anfragen an die Wechselrichter
 *  fortgefahren.<br><br>
 *
 *  @section failure Fehlersuche
 *  Zur Fehlersuche kann die Datei <b>daemon.log</b> herangezogen werden, denn in diese werden auftretende Fehler per <b>syslog</b> geschrieben.
 *  Die Datei befindet sich im Regelfall unter <b>/var/log/</b>.
 */

/*! @page page3 Dokumentation des Daemon-Scripts "sunnyconnectorpid"
 *
 * @tableofcontents
 * @section daemon Einführung zum Daemon-Script.
 * Das Programm <b>SunnyConnectorPI</b> ist als Daemon geschrieben und kann somit nur auf einem Linux-OS betrieben werden.<br><br>
 * Benutzung: <b>./sunnyconnectorpid {start|stop|restart|status}</b>.<br><br>
 * Für das Starten des Daemon benutzt man folgendes Kommando <b>./SunnyConnectorPI start</b>.
 * Der Daemon ist über das linuxeigene Programm "start-stop-daemon" realisiert und wird von diesem Programm kontrolliert.
 * Das Script hat einige Startparameter, welche für die jeweilige Umgebung modifiziert werden müssen.<br>
 * Für weitere Informationen siehe \ref SunnyConnectorPId_code.<br>
 * Wenn man einen Parameter nicht an das Script übergibt, wird vom SunnyConnectorPI ein Defaultwert gesetzt.
 * Diese Defaultwerte sind in der Headerdatei <b>defines.h</b> deklariert.
  *<br>
 * In der folgenden Code-Sektion findet man die Parameter, welche man an seine Umgebung anpassen sollte bzw. muss.<br>
 * An der letzten Zeile steht der Parameter, welcher die zu übergebenden Parameter an den Daemon <b>SunnyConnectorPI</b> angibt.
 * Wenn man hier eine Option löscht, wird wie oben beschrieben ein Defaultwert vom <b>SunnyConnectorPI</b> aus der Datei <b>defines.h</b> benutzt.
 *
 * @section SunnyConnectorPId_code Veränderbare Variablen das Daemon-Scripts.
 * @code
### Pfadvariable ###
PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin

### Kommandozeilenvariablen für den SunnyConnectorPI ###
# Pfad zur CONFIG.INI
CONFIG="/home/thomas/config.ini"
# Arbeitsverzeichnis für den Daemon
WORKDIR="/srv/SunnyConnectorPI"
# Name, welcher im der Logdatei "daemon.log" angezeigt wird.
NAME="SunnyConnectorPI"
# UMASK für das Anlegen des Arbeitsverzeichnisses
UMASKMKDIR=0770
# Datei für die PID
PIDFILE="/var/run/SunnyConnectorPI.pid"
# UMASK für Dateien des Daemon
UMASK=027
# Pfad zur YASDI.INI
YASDI="/home/thomas/yasdi.ini"

### Variablen des Daemon ###
# Pfad zum SunnyConnectorPI
DAEMON="/home/thomas/$NAME"
# User unter welchem der Daemon laufen soll
USER=root

### Von hieran sollten Sie nichts mehr ändern, ausser Sie wissen was Sie tun ;-) ###
# Optionen für den Daemon
DAEMON_OPTS="-c $CONFIG -d $WORKDIR -n $NAME -m $UMASKMKDIR -p $PIDFILE -u $UMASK -y $YASDI"
 @endcode
*/

/*! @page page4 Dokumentation der Datei "CONFIG.INI"
 *
 * @tableofcontents
 * @section config Parameterbeschreibung
 * Diese Datei ist für den <b>SunnyConnectorPI</b> und beinhaltet die Parameter für das Speichern der gelesenen Daten.<br><br>
 * Diese Zeilen zeigen die Parameter der Datei <b>CONFIG.INI</b>.<br>
 * <ul>
 *  <li>
 *      Das Segment <b>TIMERS</b> beinhaltet einige Timer.<br><br>
 *      <ul>
 *          <li>
 *              // Abstand zwischen den einzelnen Anfragen an den/die Wechselrichter. Das Intervall startet wenn die letzte Anfrage beendet ist.
 *              @code
 * INTERVALDATA=5
 *              @endcode
 *          </li>
 *      </ul>
 *      <br>
 *  </li>
 *  <li>
 *      Das Segment <b>MAIL</b> beinhaltet Variablen zum Verwalten einer Mail-Server-Anbindung.<br><br>
 *      <ul>
 *          <li>
 *              // Wert, der festlegt ob Fehler- und Statusmails versandt werden sollen (0 = nein / 1 = ja).
 *              @code
 * USE=1
 *              @endcode
 *          </li>
 *          <li>
 *              // Name des Mail-Servers.
 *              @code
 * HOST=smtp.example.com
 *              @endcode
 *          </li>
 *          <li>
 *              // Verbindungsport zum Mail-Server.
 *              @code
 * PORT=25
 *              @endcode
 *          </li>
 *          <li>
 *              // SSL-Verschlüsselung (Tls) verwenden.
 *              @code
 * TLS=no
 *              @endcode
 *          </li>
 *          <li>
 *              // Absender-Adresse.
 *              @code
 * FROM=sender@example.com
 *              @endcode
 *          </li>
 *          <li>
 *              // Empfänger-Adresse.
 *              @code
 * TO=recipient
 *              @endcode
 *          </li>
 *          <li>
 *              // Benutzer für die Verbindung zum Mail-Server.
 *              @code
 * USER=root
 *              @endcode
 *          </li>
 *          <li>
 *              // Passwort für die Verbindung zum Mail-Server.
 *              @code
 * PW=password
 *              @endcode
 *          </li>
 *      </ul>
 *      <br>
 *  </li>
 *  <li>
 *      Das Segment <b>FILE</b> beinhaltet Variablen zum Verwalten des Speichers der Werte in einer Datei pro Wechselrichter.<br><br>
 *      <ul>
 *          <li>
 *              // Wert, der festlegt ob die Werte in einer Datei gespeichert werden sollen (0 = nein / 1 = ja).
 *              @code
 * USE=1
 *              @endcode
 *          </li>
 *          <li>
 *              // Trennzeichen, welches zwischen zwei Werte in einer Datei stehen soll.
 *              @code
 * SEPERATOR=;
 *              @endcode
 *          </li>
 *          <li>
 *              // Dateierweiterung der Datei(en).
 *              @code
 * EXT=.csv
 *              @endcode
 *          </li>
 *      </ul>
 *      <br>
 *  </li>
 *  <li>
 *      Das Segment <b>MYSQL</b> beinhaltet Variablen zum Verwalten des Speichers der Werte in einem MySQL-Server.<br><br>
 *      <ul>
 *          <li>
 *              // Wert, der festlegt ob die Werte in einem MySQL-Server gespeichert werden sollen (0 = nein / 1 = ja).
 *              @code
 * USE=1
 *              @endcode
 *          </li>
 *          <li>
 *              // Name des MySQL-Servers.
 *              @code
 * HOST=www.example.com
 *              @endcode
 *          </li>
 *          <li>
 *              // Verbindungsport zum MySQL-Server.
 *              @code
 * PORT=3306
 *              @endcode
 *          </li>
 *          <li>
 *              // Benutzer für die Verbindung zum MySQL-Server.
 *              @code
 * USER=root
 *              @endcode
 *          </li>
 *          <li>
 *              // Passwort für die Verbindung zum MySQL-Server.
 *              @code
 * PW=mysql
 *              @endcode
 *          </li>
 *          <li>
 *              // Datenbank auf dem MySQL-Server.
 *              @code
 * DB=sunnyconnector
 *              @endcode
 *          </li>
 *          <li>
 *              // Tabelle für die Seriennummern auf dem MySQL-Server.
 *              @code
 * SNTAB=seriennummern
 *              @endcode
 *          </li>
 *          <li>
 *              // Tabelle für die Datenpunkte auf dem MySQL-Server.
 *              @code
 * DATATAB=daten
 *              @endcode
 *          </li>
 *      </ul>
 *      <br>
 *  </li>
 *  <li>
 *      Das Segment <b>STATUS</b> beinhaltet einige Variablen zum Verwalten des/der Wechselrichter(s).<br><br>
 *      <ul>
 *          <li>
 *              // Channel, über welchen festgestellt werden kann ob ein Wechselrichter an ist.
 *              @code
 * STATUSCHANNEL=Status
 *              @endcode
 *          </li>
 *          <li>
 *              // Wert, über welchen festgestellt werden kann ob ein Wechselrichter Strom einspeist oder nicht.
 *              @code
 * STATUSVALUE=Mpp
 *              @endcode
 *          </li>
 *      </ul>
 *      <br>
 *  </li>
 *  <li>
 *      Das Segment <b>SERIALNUMBERS</b> beinhaltet den/die Seriennummer(n) des/r Wechselrichter(s) welche gelesen werden soll(en).<br><br>
 *      <ul>
 *          <li>
 *              // Seriennummer eines oder mehrerer Wechselrichter(s), wobei jeder Wechselrichter eine eigene Zeile und <b>DEV</b>-Bezeichnung hat.
 *              @code
 * DEV0=2000631096
 *              @endcode
 *          </li>
 *      </ul>
 *      <br>
 *  </li>
 *  <li>
 *      Das Segment <b>VALUES</b> beinhaltet den/die Wert(e), welche aus den/m Wechselrichter(n) gelesen werden soll(en).<br><br>
 *      <ul>
 *          <li>
 *              // Lesen des Werts welcher den total eingespeisten Strom (KWh) eines Wechselrichters anzeigt.
 *              @code
 * DATA9=E-Total
 *              @endcode
 *          </li>
 *      </ul>
 *      <br>
 *  </li>
 *  </ul>
 *
 * @section example_config Beispiel einer CONFIG.INI
 * @code
 * [TIMERS]
 * INTERVALDATA=5
 *
 * [MAIL]
 * USE=0
 * SERVER=smtp.example.com
 * PORT=25
 * TLS=no
 * FROM=
 * TO=
 * USER=
 * PW=
 *
 * [FILE]
 * USE=1
 * SEPERATOR=;
 * EXT=.csv
 *
 * [MYSQL]
 * USE=0
 * HOST=
 * PORT=
 * USER=
 * PW=
 * DB=
 * SNTAB=
 * DATATAB=

 * [STATUS]
 * STATUSCHANNEL=Status
 * STATUSVALUE=Mpp

 * [SERIALNUMBERS]
 * DEV0=2000631068
 * DEV1=2000631096
 * DEV2=2000499134

 * [VALUES]
 * DATA0=Upv-Ist
 * DATA1=Upv-Soll
 * DATA2=Iac-Ist
 * DATA3=Uac
 * DATA4=Fac
 * DATA5=Pac
 * DATA6=Riso
 * DATA7=Ipv
 * DATA9=E-Total
 * DATA10=h-Total
 * DATA11=h-On
 * DATA12=Netz-Ein
 * DATA14=Status
 * DATA15=Phase
 * DATA16=Balancer
 * DATA17=Fehler
 @endcode
*/

/*! @page page5 Dokumentation der Datei "YASDI.INI"
 *
 * @tableofcontents
 * @section yasdi Parameterbeschreibung
 * Diese Datei ist für den <b>YASDI-Treiber</b> und beinhaltet die Parameter für diesen Treiber.<br><br>
 * Diese Zeilen zeigen die Parameter der Datei <b>YASDI.INI</b>.<br>
 * <ul>
 *  <li>
 *      Das Segment <b>DriverModules</b> beinhaltet den/die Treiber welche benutzt werden sollen.<br><br>
 *      <ul>
 *          <li>
 *              // Diese Zeile bedeutet, dass der serielle Treiber von YASDI benutzt werden soll.
 *              @code
 * Driver0=yasdi_drv_serial
 *              @endcode
 *          </li>
 *      </ul>
 *      <br>
 *  </li>
 *  <li>
 *      Das Segment <b>COM1</b> gibt an, dass für die Kommunikation des YASDI-Treibers der COM-Port 1 verwendet wird. <br><br>
 *      <ul>
 *          <li>
 *              // COM-Port 1 auf dem Linuxsystem <b>/dev/ttyS0</b> (Das Device muss vom Dämonuser auch lesbar sein, Stichwort "Userrecht").
 *              @code
 * Device=/dev/ttyS0
 *              @endcode
 *          </li>
 *      </ul>
 *      <ul>
 *          <li>
 *              // Dies beschreibt, dass der YASDI-Treiber per RS232-Protokoll kommunizieren soll.
 *              @code
 * Media=RS232
 *              @endcode
 *          </li>
 *      </ul>
 *      <ul>
 *          <li>
 *              // Baudrate für Kommunikation (sollte an die eigene Umgebung angepasst werden)
 *              @code
 * Baudrate=1200
 *              @endcode
 *          </li>
 *      </ul>
 *      <ul>
 *          <li>
 *              // Protokoll für Kommunikation (sollte an die eigene Umgebung angepasst werden)
 *              @code
 * Protocol=SMANet
 *              @endcode
 *          </li>
 *      </ul>
 *      <br>
 *      Das Segment <b>MISC</b> gibt an wohin der YASDI-Treiber seine Debug-Ausgaben hinschreiben soll.<br><br>
 *      <ul>
 *          <li>
 *              // Definiert das Linux-Device auf das die Debug-Ausgaben geschrieben werden sollen
 *              @code
 * DebugOutput=/dev/stderr
 *              @endcode
 *          </li>
 *      </ul>
 *      <br>
 *  </li>
 *  </ul>
 *
 * @section example_yasdi Beispiel einer YASDI.INI
 * @code
 * [DriverModules]
 * Driver0=yasdi_drv_serial
 *
 * [COM1]
 * Device=/dev/ttyS0
 * Media=RS232
 * Baudrate=1200
 * Protocol=SMANet
 *
 * [Misc]
 * //DebugOutput=/dev/stderr
 * @endcode
*/

/*! @page page6 Einstellungen bzw. Vorbereitungen für die Entwicklungsumgebung
 *
 * @tableofcontents
 * @section programms Nötige Programme für die YASDI-Treiber-Installation
 * Der YASDI-Treiber wird später für die Compilierung des <b>SunnyConnectorPI</b> benötigt.
 * Bevor man mit dem Compilieren des YASDI-Treibers beginnen kann muss man sicherstellen ob alle dafür nötigen Programme installiert sind.<br>
 * Benötigte Programme:
 * <ul>
 *      <li>
 *          unzip (zum Entpacken des YASDI-Sourcen-Packets)
 *          @code
 * apt-get install unzip
 *          @endcode
 *      </li>
 *      <li>
 *          cmake (zum Compilieren der YASDI-Sourcen)
 *          @code
 * apt-get install cmake
 *          @endcode
 *      </li>
 * </ul>
 * <b>HINWEIS</b>: Des weiteren wird davon ausgegangen das bereits ein funktionsfähiges Eclipse vorhanden ist!
 *
 * @section yasdi_driver YASDI-Treiber compilieren und installieren
 * Auf der Seite von SMA (<a href="http://www.sma.de/de/produkte/software/yasdi.html">YASDI</a>) kann man die Sourcen für den YASDI-Treiber herunterladen.
 * Dieses ZIP-File muss man anschliessend entpacken und die darin befindlichen Sourcen compilieren.<br>
 * Hier für sind folgende Schritte durchzuführen:
 * <ul>
 *      <li>
 *          Entpacken des ZIP-Files (z.B. nach /home/thomas/yasdi)
 *          @code
 * unzip yasdi-1.8.1build9-src.zip -d /home/thomas/yasdi
 *          @endcode
 *      </li>
 *      <li>
 *          YASDI-Treiber compilieren und auf dem System installieren
 *          @code
 * cd <to the YASDI project path>
 * cd projects/generic-cmake
 * mkdir build-gcc
 * cd build-gcc
 * cmake ..
 * make
 * sudo make install
 *          @endcode
 *      </li>
 * </ul>
 *
 * @section lib_mysqlclient libmysqlclient-dev installieren
 * Da die Software <b>SunnyConnectorPI</b> mit einem MySQL-Server kommunizieren kann, wird für die Compilierung ebenfalls die Entwicklerbibliothek des
 * mysqlclients benötigt.
 *
 * <ul>
 *      <li>
 *          libmysqlclient-dev (zur Kommunikation mit einem MySQL-Server)
 *          @code
 * apt-get install libmysqlclient-dev
 *          @endcode
 *      </li>
 * </ul>
 *
 * @section new_project Neues C++-Projekt erstellen
 * Nachdem man die oben beschriebenen Vorarbeiten abgeschlossen hat, muss man ein neues C++-Projekt über die Reiter "Datei->Neu->C++-Projekt" anlegen.
 * Im angezeigten Dialog sollte man folgende Angaben, wie im Bild dargestellt, vornehmen und die Eingabe durch klicken des Button <b>Weiter</b>"
 * bestätigen. Darauf erscheint wieder ein Dialog bei dem man keine Änderungen machen muss, sondern man klickt einfach auf <b>Fertigstellen</b>.
 *
 * <b>HINWEIS</b>: Der Name des Projekts sollte jedoch <b>SunnyConnectorPI</b> heißen. Hier wurde nur <b>-Test</b> angefügt, da schon ein Projekt
 * mit diesem Name vorhanden war. Ganz wichtig ist auch, dass beim <b>Toolchains</b> der Punkt <b>Linux GCC</b> angewählt ist, da sonst kein
 * Compilieren des Projekts möglich ist.
 * @image html new_project.jpg "Neues C++-Projekt erstellen"
 * @image latex new_project.jpg "Neues C++-Projekt erstellen" width=17cm
 *
 * @section compiler C/C++-Compiler-Einstellungen für das neue Projekt
 * Nachdem man ein C++-Projekt angelegt hat, muss man noch die C++-Compiler-Einstellungen anpassen.
 * Zu den Projekteigenschaften gelangt man über die Reiter "Projekt->Eigenschaften->C/C++ Build->Einstellungen", wo man nun die Einstellungen gemäß
 * Abbildung konfiguriert.
 * @image html compiler_includes.jpg "C/C++-Compiler-Einstellungen"
 * @image latex compiler_includes.jpg "C/C++-Compiler-Einstellungen" width=17cm
 * Die Pfade sind an die jeweilige Entwicklungsumgebung anzupassen, wobei man über das grüne Pluszeichen auch zu der entsprechenden Datei browsen kann.
 *
 * @section linker C/C++-Linker-Einstellungen für das neue Projekt
 * Abschliessend müssen noch die C++-Linker-Einstellungen konfiguriert werden.
 * Diese findet man etwas weiter unten im selben Einstellungsfenster.
 * @image html linker_includes.jpg "C/C++-Linker-Einstellungen"
 * @image latex linker_includes.jpg "C/C++-Linker-Einstellungen" width=17cm
 * Auch hier sind die Pfade an Ihre Entwicklungsumgebung anzupassen.
 *
 * @section import Projektdaten-Import für das neue Projekt
 * Nun fügt man die Dateien von der Entwickler-CD hinzu, in dem man folgende Reiter anklickt "Datei->Importieren" um die Projektdaten von einem
 * Dateisystem (Entwickler-CD) importieren. Im Importdialog wählt man die Punkte wie im Bild gezeigt an und bestätigt mit <b>Weiter</b>.
 * @image html import_general.jpg "Projektdaten-Import"
 * @image latex import_general.jpg "Projektdaten-Import" width=17cm
 *
 * Anschliessend öffnet sich ein neuer Dialog, in dem man wählen kann von welcher Quelle man importieren möchte. Mit <b>Durchsuchen</b> kann man per
 * Dialog zur Entwickler-CD klicken (z.B. /media/Entwickler_CD/SunnyConnectorPI/). In das Eingabefeld <b>In Ordner</b> trägt man den Namen des Projekts
 * ein, welcher <b>SunnyConnectorPI</b> lautet. Jetzt muss noch im linken Textfeld der Haken gesetzt werden und man kann die Eingabe mit <b>Fertigstellen</b>
 * beenden. Hier bekommt man noch die Meldung angezeigt ob man die ".project"-Datei überschreiben möchte, dort klickt man auf <b>Nein für alle</b>!
 * @image html import_src_dest.jpg "Quelle / Ziel konfigurieren"
 * @image latex import_src_dest.jpg "Quelle / Ziel konfigurieren" width=17cm
 *
 * @section svn Projekt aus dem SVN-Repository auschecken
 * Hier ist noch beschrieben wie man per Subversive (Eclipse-Plugin) die Quellen aus unserem SVN-Server auschecken kann.<br>
 * Es müssen folgende Reiter durchlaufen werden "Datei->Neu->Andere..." um diesen Dialog zu erhalten.
 * @image html other_svn.jpg "Projekt aus SVN erstellen"
 * @image latex other_svn.jpg "Projekt aus SVN erstellen" width=17cm
 *
 * Dort wählt man wie im Bild dargestellt <b>Project from SVN</b> und klickt dann auf <b>Weiter</b>.
 * Der nächste Dialog dient zur Auswahl, ob eine neue SVN-Verbindung erstellt oder eine bereits vorhandene Verbindung verwendet werden
 * soll. Dort sollte man <b>Create a new repository location</b> anwählen und die Eingabe mit <b>Weiter</b> bestätigen.
 * @image html create_new_svn.jpg "SVN-Verbindung wählen bzw. erstellen"
 * @image latex create_new_svn.jpg "SVN-Verbindung wählen bzw. erstellen" width=17cm
 *
 * Anschliessend wird man aufgefordert Angaben über den Zugang zum SVN-Server zu machen. Unter <b>URL</b> gibt man die URL
 * http://www.tbarth.eu/svn/SunnyConnectorPI zu unserem SVN-Server ein. Die Angaben für den Benutzer können leer bleiben,
 * da unser SVN-Server so konfiguriert ist, dass anonyme Benutzer lesend auf das Repository zugreifen können.
 * @image html checkout_daten.jpg "URL- und Benutzerangaben zur SVN-Verbindung"
 * @image latex checkout_daten.jpg "URL- und Benutzerangaben zur SVN-Verbindung" width=17cm
 * Mit Hilfe des <b>Browse</b>-Button wählt man nun noch aus, welchen Zweig (Tree) man auschecken möchte. <b>Trunk</b> ist hier die korrekte Wahl,
 * wenn man den aktuellen Entwicklungsstand haben möchte.
 * @image html checkout_trunk.jpg "Wahl des Trees den man auschecken möchte"
 * @image latex checkout_trunk.jpg "Wahl des Trees den man auschecken möchte" width=17cm
 * Im Reiter <b>Advanced</b> ist noch der Punkt <b>Enable Structure Detection</b> abzuwählen, da man den <b>trunk</b> auscheckt.
 * @image html svn_advanced.jpg "Enable Structure Dectection"
 * @image latex svn_advanced.jpg "Enable Structure Dectection" width=17cm
 * Abgeschlossen wird das Fenster durch das Klicken auf <b>Weiter</b> und man gelangt so zum nächsten Dialog, den man einfach mit <b>Fertigstellen</b>
 * abschliesst.
 * @image html checkout_head.jpg "HEAD (akutellste Version) auschecken"
 * @image latex checkout_head.jpg "HEAD (aktuellste Version) auschecken" width=17cm
 * Zum Schluss wird noch nach dem Namen für das Projekt gefragt, wobei man hier <b>SunnyConnectorPI</b> einträgt. Nun einfach mit <b>Fertigstellen</b>
 * den Checkout anstossen.
 * @image html checkout_as.jpg "Projekt-Name in der IDE"
 * @image latex checkout_as.jpg "Projekt-Name in der IDE" width=17cm
 *
 * <b>HINWEIS</b>: Die Compiler- bzw. Linker-Einstellung (\ref compiler / \ref linker) werden zwar aus dem SVN-Repositry ausgelesen, müssen aber noch an die eigene Umgebung angepasst
 * werden.
 *
*/

/*! @page page7 Änderungen für die ARM-Entwicklungsumgebung
 *
 *@tableofcontents
 * @section arm_toolchain Installation der Toolchain für ARM-Prozessoren (Raspberry Pi)
 * Die Toolchain wird mittels <b>GIT</b> auf den Entwicklungsrechner geladen.
 * Hierfür wird <b>GIT</b> installiert.
 * @code
 * apt-get install build-essential git
 * @endcode
 * Anschließend erstellt man ein Verzeichnis, in das man die Toolchain lädt und holt das Toolchain.
 * @code
 * mkdir rpi-cross
 * cd rpi-cross
 * git clone git://github.com/raspberrypi/tools.git
 * @endcode
 * Nur wird noch getestet ob die Toolchain sauber geladen wurde.
 * @code
 * cd tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin
 * arm-linux-gnueabihf-gcc -v
 * @endcode
 * Die Ausgabe sollte dann ähnlicher der aufgeführten sein
 * @code
 * Es werden eingebaute Spezifikationen verwendet.
 * COLLECT_GCC=arm-linux-gnueabihf-gcc
 * COLLECT_LTO_WRAPPER=/home/thomas/rpi-cross/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin/../libexec/gcc/arm-linux-gnueabihf/4.7.2/lto-wrapper<br>
 * Ziel: arm-linux-gnueabihf
 * Konfiguriert mit: /cbuild/slaves/oort61/crosstool-ng/builds/arm-linux-gnueabihf-raspbian-linux/.build/src/gcc-linaro-4.7-2012.08/configure --build=i686-build_pc-linux-gnu<br>
 * --host=i686-build_pc-linux-gnu --target=arm-linux-gnueabihf --prefix=/cbuild/slaves/oort61/crosstool-ng/builds/arm-linux-gnueabihf-raspbian-linux/install<br>
 * --with-sysroot=/cbuild/slaves/oort61/crosstool-ng/builds/arm-linux-gnueabihf-raspbian-linux/install/arm-linux-gnueabihf/libc --enable-languages=c,c++,fortran<br>
 * --disable-multilib --with-arch=armv6 --with-tune=arm1176jz-s --with-fpu=vfp --with-float=hard --with-pkgversion='crosstool-NG linaro-1.13.1+bzr2458 - Linaro GCC 2012.08'<br>
 * --with-bugurl=https://bugs.launchpad.net/gcc-linaro --enable-__cxa_atexit --enable-libmudflap --enable-libgomp --enable-libssp<br>
 * --with-gmp=/cbuild/slaves/oort61/crosstool-ng/builds/arm-linux-gnueabihf-raspbian-linux/.build/arm-linux-gnueabihf/build/static<br>
 * --with-mpfr=/cbuild/slaves/oort61/crosstool-ng/builds/arm-linux-gnueabihf-raspbian-linux/.build/arm-linux-gnueabihf/build/static<br>
 * --with-mpc=/cbuild/slaves/oort61/crosstool-ng/builds/arm-linux-gnueabihf-raspbian-linux/.build/arm-linux-gnueabihf/build/static<br>
 * --with-ppl=/cbuild/slaves/oort61/crosstool-ng/builds/arm-linux-gnueabihf-raspbian-linux/.build/arm-linux-gnueabihf/build/static<br>
 * --with-cloog=/cbuild/slaves/oort61/crosstool-ng/builds/arm-linux-gnueabihf-raspbian-linux/.build/arm-linux-gnueabihf/build/static<br>
 * --with-libelf=/cbuild/slaves/oort61/crosstool-ng/builds/arm-linux-gnueabihf-raspbian-linux/.build/arm-linux-gnueabihf/build/static<br>
 * --with-host-libstdcxx='-L/cbuild/slaves/oort61/crosstool-ng/builds/arm-linux-gnueabihf-raspbian-linux/.build/arm-linux-gnueabihf/build/static/lib -lpwl'<br>
 * --enable-threads=posix --disable-libstdcxx-pch --enable-linker-build-id --enable-plugin --enable-gold<br>
 * --with-local-prefix=/cbuild/slaves/oort61/crosstool-ng/builds/arm-linux-gnueabihf-raspbian-linux/install/arm-linux-gnueabihf/libc --enable-c99 --enable-long-long
 * Thread-Modell: posix
 * gcc-Version 4.7.2 20120731 (prerelease) (crosstool-NG linaro-1.13.1+bzr2458 - Linaro GCC 2012.0
 * @endcode
 *
 * @section yasdi_mysql_pi YASDI-Treiber- und MySQL-Client-Installation
 * Der YASDI-Treiber und der MySQL-Client müssen auf dem Raspberry Pi installiert werden und später auf den eigentlichen
 * Entwicklungsrechner kopiert werden. Denn für die Erstellung des <b>SunnyConnectorPI</b> werden die Libaries der beiden
 * Programme als ARM-Compilat benötigt.<br>
 * Hierfür gehen Sie wie bei der Installation auf dem Entwicklungsrechner (\ref page6) vor.
 *
 * @section remote_connection Erstellen einer Remote-Connection zum Raspberry Pi
 * Mit Hilfe dieser Remote-Connection können Sie später direkt aus Eclipse heraus auf den Raspberry Pi zu greifen und Änderungen darauf vornehmen.
 * Zur Ansicht gelangen Sie über die Reiter "Fenster -> Sicht anzeigen -> Andere".
 * Dort wählen Sie unter "Remote Systems" den Punkt "Remote System Details" und klicken abschliessend "OK".
 * @image html remote_view.jpg "Ansicht wählen"
 * @image latex remote_view.jpg "Ansicht wählen" width=17cm
 *
 * Es öffnet sich nun das Fenster mit den vorhanden Remote-Verbindungen.<br>
 * Hier sehen Sie nur eine Verbindung für die lokale Entwicklung.
 * @image html remote_window.jpg "Remote-System-Fenster"
 * @image latex remote_window.jpg "Remote-System-Fenster" width=17cm
 *
 * In dem neuen Fenster führen Sie einen Rechts-Klick aus und wählen im Kontextmenu "Neue Verbindung" aus und es
 * öffent sich ein Fenster zur Auswahl des Types der neuen Verbindung.<br>
 * Hier wählen Sie "SSH only" aus und klicken auf "Weiter".
 * @image html remote_type.jpg "Type der Remote-Verbindung wählen"
 * @image latex remote_type.jpg "Type der Remote-Verbindung wählen" width=17cm
 *
 * Anschließend gelangt man zum Fenster, in dem Sie die Daten des Raspberry Pi eingeben müssen.
 * Der Dialog wird mit "Fertigstellen" abgeschlossen.
 * @image html remote_new.jpg "Verbindungsdaten des Raspberry Pi"
 * @image latex remote_new.jpg "Verbindungsdaten des Raspberry Pi" width=17cm
 *
 * Im System-Detail-Fenster können Sie nun die neu angelegte Verbindung sehen.
 * @image html remote_window_2.jpg "Neue Verbindung angelegt."
 * @image latex remote_window_2.jpg "Neue Verbindung anglegt." width=17cm
 *
 * Es kann nun eine Verbindung mit dem Raspberry Pi hergestellt werden. Hierzu führen Sie einen Rechts-Klick auf die
 * neu angelegt Verbindung aus. Dort wählen Sie nun den Kontext "Verbinden". Beim ersten Verbinden erhalten Sie einen
 * Dialog in dem Sie die User-Daten des Raspberry Pi`s eingeben müssen.
 * @image html remote_user.jpg "Raspberry Pi-Verbindungsdaten"
 * @image latex remote_user.jpg "Raspberry Pi-Verbindungsdaten" width=17cm
 *
 * Nun können Sie wieder über einen Rechts-Klick und dem Punkt "Show in Remote System View" ein neues Fenster öffnen
 * in dem Sie einmal das lokale Dateisystem und das Raspberry Pi-Dateisystem erhalten.
 * Mit Hilfe diesem Fenster können Sie Dateien zwischen den Rechnern hin- und herkopieren.
 * @image html remote_system_view.jpg "System-View-Fenster"
 * @image latex remote_system_view.jpg "System-View-Fenster" width=17cm
 *
 * Über den Menü-Punkt "Ssh Terminals" können Sie ein SSH-Terminal zum Raspberry Pi öffnen, in dem Sie wiederrum einen Rechts-Klick auf den
 * Menü-Punkt ausführen und anschliessend den Punkt "Launch Terminal" anwählen.
 * Dadurch öffnet sich ein neues Fenster mit dem Titel "Terminals", über das Sie nun direkt mit dem Raspberry Pi verbunden sind und
 * direkt Befehle auf dem Pi ausführen können.
 * @image html remote_terminal.jpg "Terminal-Fenster"
 * @image latex remote_terminal.jpg "Terminal-Fenster" width=17cm
 *
 * @section libaries_copy YASDI- und MySQL-Client-Bibliothek auf den Entwicklungsrechner kopieren
 * Zum Schluss müssen Sie noch die Bibliotheken vom Raspberry Pi auf den eigentlichen Entwicklungsrechner kopieren.
 * Hierfür verwenden Sie die bereits erstellte Remote-Verbindung.<br>
 * Zu Beginn wählen Sie das "Remote Systems"-Fenster aus und navigieren dann unter "Local" im Ordner "MyFiles" zu
 * folgendem Unterordner
 * @code
 * rpi-cross/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/lib
 * @endcode
 *
 * @subsection yasdi_copy Kopieren der YASDI-Bibliotheken
 * Nun scrollen Sie nach unten zum Raspberry Pi und navigieren hier im Ordner "Stammverzeichnis" zum Unterordner
 * @code
 * /usr/local/lib
 * @endcode
 * Hier finden Sie weiter unten Dateien mit nach dem Muster "libyasdi*" im Dateinamen. Diese Dateien markieren Sie alle und kopieren
 * diese auf den oben geöffneten lokalen Ordner.<br>
 *
 * @subsection mysql_copy Kopieren der MySQL-Client-Bibliotheken
 * Nun scrollen Sie wieder nach unten zum Raspberry Pi und navigieren hier im Ornder "Stammverzeichnis" zum Unterordner
 * @code
 * /usr/lib/arm-linux-gnueabihf
 * @endcode
 * Hier finden Sie weiter unten Dateien mit dem Muster "libmysql*" im Dateinamen. Diese Dateien markieren Sie alle und kopieren
 * diese auf den oben geöffneten lokalen Ordner.<br>
 * Aus dem Ordner
 * @code
 * /lib/arm-linux-gnueabihf
 * @endcode
 * Aus diesem Ordner benötigen Sie ebenfalls die Datei "libz.so.1" auf den Entwicklungsrechner.
 *
 * @section toolchain_arm ToolChain-Einstellungen für die ARM-Umgebung
 * Für die ARM-Umgebung muss der Compiler von C++ auf den Cross-Compiler für den ARM-Prozessor geändert werden.
 * Hierfür öffnen Sie die Projekteigenschaften und wählen dann rechts unter dem Punkt "C++-Build" den Kontextpunkt "Tool Chain Editor" aus.
 * Dort müssen Sie dann die Auswahl unter dem Punkt "Current toolchain" auf "Cross GCC" ändern.
 * Diese Vorgang müssen Sie für alle Konfigurationen "Debug", "Release" usw. vornehmen.
 * @image html arm_toolchain.jpg "Cross-GCC-Compiler-Einstellungen"
 * @image latex arm_toolchain.jpg "Cross-GCC-Compiler-Einstellungen" width=17cm
 *
 * Als nächstes ändern Sie im Fenster des Menupunkts "Einstellungen" die CrossCompiler-Einstellugen wie im Bild angezeigt.
 * Die Eingabe im Feld "Pfad" müssen Sie an Ihr System anpassen.
 * @image html arm_cross_settings.jpg "Weitere Cross-GCC-Compiler-Einstellungen"
 * @image latex arm_cross_settings.jpg "Weitere Cross-GCC-Compiler-Einstellungen" width=17cm
 *
 * @section compiler_arm Cross-Compiler-Einstellungen für die ARM-Umgebung
 * Nachdem Sie das ToolChain eingereichtet haben, müssen Sie noch die Cross-Compiler-Einstellungen anpassen.
 * Zu den Projekteigenschaften gelangt man über die Reiter "Projekt->Eigenschaften->C/C++ Build->Einstellungen", wo man nun die Einstellungen gemäß
 * Abbildung konfiguriert.
 * @image html arm_compiler_includes.jpg "Cross-Compiler-Einstellungen"
 * @image latex arm_compiler_includes.jpg "Cross-Compiler-Einstellungen" width=17cm
 * Die Pfade sind an die jeweilige Entwicklungsumgebung anzupassen, wobei man über das grüne Pluszeichen auch zu dem entsprechenden Verzeichnis browsen kann.
 *
 * @section linker_arm Cross-Linker-Einstellungen für die ARM-Umgebung
 * Abschliessend müssen noch die Cross-Linker-Einstellungen konfiguriert werden.
 * Diese findet man etwas weiter unten im selben Einstellungsfenster.
 * @image html arm_linker_includes.jpg "Cross-Linker-Einstellungen"
 * @image latex arm_linker_includes.jpg "Cross-Linker-Einstellungen" width=17cm
 * Auch hier sind die Pfade bzw. Dateien an Ihre Entwicklungsumgebung anzupassen.
 *
 * @section additional_linker_arm Weitere Cross-Linker Einstellungen für die ARM-Umgebung
 * Abschliessend müssen Sie noch zwei Bibliotheken per Hand einbinden.
 * Diese müssen Sie wie in der Abbildung einfügen.
 * @image html arm_other_objects.jpg "Cross-Linker-Einstellungen"
 * @image latex arm_other_objects.jpg "Cross-Linker-Einstellungen" width=17cm
 * Wie bereits oben mehrfach erwähnt müssen hier die Pfade ebenfalls wieder an Ihr System angepasst werden.
 *
*/

#endif /* DOCUMENTATION_H_ */
