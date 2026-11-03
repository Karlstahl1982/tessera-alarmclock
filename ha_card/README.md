# Tessera AlarmClock – Home-Assistant-Karte (unverändert seit v0.2.3)

Die Karte zeigt den nächsten Alarm und eine übersichtliche Liste der angelegten Wecker. **Neue Wecker** werden mit dem vierstufigen Wizard angelegt; **vorhandene Wecker** öffnen eine grafische Bearbeitungsseite mit großer Zeitanzeige, Wiederholung, Ton, Lautstärke, Schlummern und Status.

## Installation

1. `tessera-alarmclock-card.js` nach `/homeassistant/www/tessera-alarmclock-card.js` kopieren (bestehende Datei überschreiben).
2. In Home Assistant unter **Einstellungen → Dashboards → Ressourcen** die Ressource `/local/tessera-alarmclock-card.js?v=0.2.3` als **JavaScript-Modul** registrieren beziehungsweise die bestehende URL darauf ändern.
3. Den Browser vollständig neu laden (Strg+F5).
4. Karte auf dem Dashboard: siehe `LOVELACE_KARTE.yaml`. Bei mehreren Geräten optional `device_filter` setzen.

## Wichtiger Unterschied

Der HA-Kartentausch aktualisiert nur Home Assistant. **Für die neue, nicht wegklickbare Alarmanzeige und die grafische Bearbeitung auf dem Guition-Display ist zusätzlich das Tessera-Firmware-Update auf AlarmClock v0.2.3 erforderlich.** Dazu alle Projektdateien ins GitHub-Repository hochladen und das Plugin für „Hilde“ aktualisieren.

## Verlässlichkeit

HA ist für die Konfiguration nicht die führende Instanz. Die ESP32-Firmware verwaltet die fünf Wecker lokal. Funktionstests für das Display sind im Zielgerät noch erforderlich; ein von der Firmware unabhängiger Ersatzwecker ist für wichtige Termine ratsam.


> **Hinweis zum Plugin-Release 0.2.4:** Der Hotfix betrifft nur die Firmware-Komponente (LVGL-Kompilierung). Ein erneutes Kopieren der Home-Assistant-Karte oder Ändern der Ressourcen-URL ist nicht erforderlich.
