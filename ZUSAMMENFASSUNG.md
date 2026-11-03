# Projektzusammenfassung – Tessera AlarmClock

**Version:** 0.3.1 (Beta)  
**Ziel:** Eigenständiger Wecker mit bis zu fünf lokalen Alarmen, grafischer intuitiver Bedienung am 4-Zoll-Touchdisplay und Home-Assistant-Verwaltung.

## Arbeitsschritte

- [x] Funktionierenden Ausgangsstand v0.3.0 übernommen.
- [x] Name / Uhrzeit / Wiederholung / Status in den Weckerkacheln räumlich getrennt.
- [x] Grafische Bearbeitungsansicht bereinigt.
- [x] Wiederholung, Wochentage, Tonwahl, Hörprobe und Optionen grafisch umgestaltet.
- [x] Zeit-/Datumswahl und Zusammenfassung aufgeräumt.
- [x] Lokale Wecklogik, gespeicherte Einstellungen und blockierendes Alarm-Vollbild unverändert gelassen.
- [x] Vollständiges Repository-Paket, Changelog und Tests aktualisiert.
- [ ] ESPHome-Firmware-Build mit Ziel-Tessera/ESPHome bestätigen.
- [ ] Alle Touch-Hitboxen und Schriftgrößen auf dem realen 480×480-Display überprüfen.
- [ ] Abschließenden Alarm- und Offline-Test durchführen.

## Probleme / Umgehungen

- **Überlappung:** v0.3.0 setzte Uhrzeit/Name in sich überschneidende Bereiche. v0.3.1 definiert feste getrennte vertikale Layoutzonen und prüft sie rechnerisch.
- **Textlastige Menüs:** Wiederverwendung der vorhandenen LVGL-Karten für Detailseiten, statt zusätzliche komplexe Widgets einzubauen.
- **LVGL-API:** Es werden weiterhin nur bestehende LVGL-Grundobjekte verwendet; der inkompatible `lv_button_create`-Aufruf ist nicht erforderlich.
- **Sonderzeichen:** Icons werden aus LVGL-Formen gezeichnet; keine Abhängigkeit von Emoji-Fonts.
- **Risiko:** Mock-Kompilierung und Geometrietests simulieren keinen echten Build/Display-Render. Erst nach erfolgreichem Build flashen; vor Benutzung als Alltagswecker Alarmfunktion erneut prüfen.

## Installation

1. ZIP entpacken.
2. Gesamten Inhalt ohne zusätzlichen Versionsordner im Stamm des Repositories `https://github.com/Karlstahl1982/tessera-alarmclock` hochladen und committen.
3. In Tessera `Branch testen` (`main`) auswählen und Version 0.3.1 prüfen.
4. Firmware nur kompilieren, Logs prüfen, dann auf „Hilde“ übertragen.
5. Anzeigen/Touchbedienung und Testalarm prüfen. Bei Fehlern vorherigen funktionierenden Stand v0.3.0 wiederherstellen.

## Unverändert / offen

- Fünf lokale Wecker, 5 Alarmmelodien, Home-Assistant-Karte, Stoppen/Schlummern, Weckerpersistenz.
- Kein RTC-Modul; nach Neustart ohne Zeitquelle weiterhin keine garantierte Alarmzeit.
- Alarmhelligkeit und echte Offline-Bedienung bei Tessera-Verbindungsverlust noch nicht vollständig validiert.
