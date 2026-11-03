# Tessera AlarmClock v0.3.8

Lokaler Wecker für Guition ESP32-4848S040 mit Tessera. Bis zu fünf Wecker, lokale Zeitauswertung, Alarmtöne, Schlummern, Touchbedienung und nur zentrale Home-Assistant-Entitäten.

## Änderung v0.3.8

- **Normalbetrieb:** Kein Bearbeiten-Symbol auf der großen Tessera-Kachel. Die gesamte Kachel bleibt antippbar und öffnet „Meine Wecker“.
- **Schlummermodus:** Ausschließlich dann erscheint unten rechts das **rote X**. Ein Tipp darauf beendet den angezeigten Schlummeralarm samt geplanter Wiederholung.
- **Andere Wecker:** Werden nicht verändert. Die Audio- und Home-Assistant-Funktionen bleiben identisch zu v0.3.7.


## Installation

1. Eine Sicherung der bisherigen Konfiguration/Weckzeiten vorhalten.
2. Das komplette ZIP entpacken und **alle Dateien aus dem Archiv direkt in das Hauptverzeichnis** des bestehenden Repositories `Karlstahl1982/tessera-alarmclock` hochladen. Vorhandene Dateien ersetzen und auf `main` committen.
3. In Tessera **Plugins → Mit einem Link hinzufügen → Branch testen** `https://github.com/Karlstahl1982/tessera-alarmclock` und Branch `main` auswählen; neue Version **0.3.8** bzw. den neuen Commit kontrollieren.
4. Einen neuen ESPHome-Firmware-Build für **Hilde** starten. Nur nach erfolgreicher Kompilierung auf das Display übertragen.
5. Einen einmaligen Wecker mit kurzer Testzeit anlegen; beim Klingeln **Schlummern** drücken. Im Normalbetrieb darf **kein Bearbeiten-Symbol** sichtbar sein. Beim Schlummern sollte das **rote X** erscheinen. Durch Tippen außerhalb des X muss sich die Übersicht öffnen. Durch Tippen auf das rote X muss das Schlummern abgebrochen werden.
6. Sicherstellen, dass nach Ablauf der ursprünglichen Schlummerzeit **kein Ton** mehr startet. Andere Wecker und ihre Aktivierung sollen unverändert bleiben.

Home Assistant: Die separat installierte Karte und die drei zentralen Entitäten müssen nicht verändert werden.

## Bekannte Grenzen

- Echte ESPHome-Kompilierung und Touchprüfung auf dem Guition können hier nicht durchgeführt werden; die lokalen C++-Mock-Tests ersetzen den Gerätetest nicht.
- Beim aktiven Schlummern benötigt das 40-px-Abbrechen-Symbol ausreichend Platz. Am großen 2×3-Widget ist der Bereich gut erreichbar.
- Die Akku-/Netzstromversorgung und die laufende Gerätezeit bestimmen weiterhin die Verfügbarkeit des Weckers.
