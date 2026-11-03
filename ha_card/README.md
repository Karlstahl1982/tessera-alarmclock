# Home-Assistant-Karte – AlarmClock 0.2.0

Die Karte ist die **Bedienoberfläche**, nicht der Wecker. Das Guition-Gerät speichert und löst Alarme weiterhin selbstständig aus.

## Installation

1. Aus dem ZIP `ha_card/tessera-alarmclock-card.js` nach `/config/www/tessera-alarmclock-card.js` kopieren (in manchen Dateimanager-Ansichten heißt `/config` auch `/homeassistant`).
2. Home Assistant → Einstellungen → Dashboards → Ressourcen (Menü rechts oben) → Ressource hinzufügen.
3. Adresse `/local/tessera-alarmclock-card.js` und Typ **JavaScript-Modul**.
4. Home Assistant im Browser einmal neu laden (bei altem Cache: Strg+F5).
5. Eine neue **manuelle Karte** im Dashboard erstellen und den Inhalt aus `ha_card/LOVELACE_KARTE.yaml` verwenden.
6. Bei mehreren ESPHome-AlarmClock-Geräten optional `device_filter: andy` (Teil des Entity-IDs) angeben.

Die 40 bisherigen Konfigurations-Entitäten bleiben für ESPHome erhalten, sind aber als `entity_category: config` gruppiert. Auf dem Dashboard genügt die einzelne AlarmClock-Karte. Die Karte benötigt keine Drittanbieter-Frontendbibliotheken.

**Wichtig:** Erst das neue Tessera-Plugin erfolgreich kompilieren und auf das Display installieren, dann die HA-Karte nutzen. Die Karte benötigt die neuen Entitäten `AlarmClock Belegung`, `AlarmClock Verwaltung` und die fünf `AlarmClock N Name`.

Bei unbekannten oder fehlenden Entitäten zeigt die Karte eine Fehlermeldung. Browser/HA müssen die ESPHome-Geräteverbindung erreichen.

## Einschränkung

Die Anzeige des *nächsten* Weckers berechnet lokale Datumswerte im Browser. Für eindeutige Zeitanzeigen sollte der Browser die Zeitzone Europe/Berlin verwenden. Maßgeblich für die Auslösung bleibt die lokale Wecklogik des ESP32.
