# Changelog

## v0.3.4 – 03.11.2026

### Behoben
- Fortschrittsbalken der Lautstärke als unabhängig positionierte LVGL-Rechtecke mit definiertem Innenabstand.
- Schlummerzustand eines einmaligen Weckers bleibt im Raster und in der Hauptkachel sichtbar, auch wenn er für weitere Termine deaktiviert ist.

### Verbessert
- Lautstärkeerhöhung jetzt alle 1 s mit stetig interpoliertem digitalen Gain statt Sprüngen alle 6 s; Prozentanzeige und Countdown bis zum Zielpegel angepasst.
- Statusmeldungen für nächsten Wecker, klingelnde und schlummernde Alarme.

### Geändert (Home Assistant)
- Einzel-Entitäten der fünf Wecker auf `internal: true` gesetzt, ohne deren IDs oder lokale Speicherung zu ändern.
- Nur drei zentrale HA-Entitäten werden veröffentlicht: nächster Wecker, globale Unterdrückung, Alarm stoppen.
- Neue reduzierte Lovelace-Karte, kein Bearbeiten einzelner Wecker mehr in HA.
- Globale Unterdrückung überspringt neue Alarme und quittiert laufende / schlummernde Alarme; Boot-Standard AUS.

### Prüfstatus
- 24 automatisierte Tests, C++-Mock-Build, JS-Laufzeittest und Paketprüfung bestanden.
- Echter ESPHome-Build, OTA-Flash und Hardwaretests offen.
