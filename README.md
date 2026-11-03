# Tessera AlarmClock v0.3.7

Lokaler Wecker für Guition ESP32-4848S040 mit Tessera. Bis zu fünf Wecker, lokale Zeitauswertung, Alarmtöne, Schlummern, Touchbedienung und nur zentrale Home-Assistant-Entitäten.

## Neu in v0.3.7

Das bisherige blaue Symbol unten rechts in der großen Tessera-Weckerkachel ist ein stilisierter Stift zum Bearbeiten. Während **Schlummern** wechselt es zu einem **roten X-Button**:

- **Rotes X drücken:** Nur den angezeigten Schlummeralarm inklusive ausstehender Wiederholung abbrechen.
- **Anderswo auf die Kachel tippen:** „Meine Wecker“ öffnen, ohne den Schlummeralarm zu beenden.
- **Ohne Schlummern:** Bearbeiten-Symbol wie bisher.

Das Icon wechselt dynamisch; es sind keine neuen Home-Assistant-Entitäten nötig. Bei mehreren gleichzeitig schlummernden Weckern betrifft das X den auf der Hauptkachel angezeigten, zeitlich nächsten Schlummeralarm; weitere Schlummeralarme werden nicht beendet.

## Installation

1. Eine Sicherung der bisherigen Konfiguration/Weckzeiten vorhalten.
2. Das komplette ZIP entpacken und **alle Dateien aus dem Archiv direkt in das Hauptverzeichnis** des bestehenden Repositories `Karlstahl1982/tessera-alarmclock` hochladen. Vorhandene Dateien ersetzen und auf `main` committen.
3. In Tessera **Plugins → Mit einem Link hinzufügen → Branch testen** `https://github.com/Karlstahl1982/tessera-alarmclock` und Branch `main` auswählen; neue Version **0.3.7** bzw. den neuen Commit kontrollieren.
4. Einen neuen ESPHome-Firmware-Build für **Hilde** starten. Nur nach erfolgreicher Kompilierung auf das Display übertragen.
5. Einen einmaligen Wecker mit kurzer Testzeit anlegen; beim Klingeln **Schlummern** drücken. Auf der Hauptkachel sollte das **rote X** erscheinen. Durch Tippen außerhalb des X muss sich die Übersicht öffnen. Durch Tippen auf das rote X muss das Schlummern abgebrochen werden.
6. Sicherstellen, dass nach Ablauf der ursprünglichen Schlummerzeit **kein Ton** mehr startet. Andere Wecker und ihre Aktivierung sollen unverändert bleiben.

Home Assistant: Die separat installierte Karte und die drei zentralen Entitäten müssen nicht verändert werden.

## Bekannte Grenzen

- Echte ESPHome-Kompilierung und Touchprüfung auf dem Guition können hier nicht durchgeführt werden; die lokalen C++-Mock-Tests ersetzen den Gerätetest nicht.
- Wenn die Kachel sehr klein platziert ist, kann das 40-px-Icon weniger Platz haben. Am großen 2×3-Widget ist der Bereich gut erreichbar.
- Die Akku-/Netzstromversorgung und die laufende Gerätezeit bestimmen weiterhin die Verfügbarkeit des Weckers.
