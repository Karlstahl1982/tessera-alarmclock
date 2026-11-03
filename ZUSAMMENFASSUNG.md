# Projektzusammenfassung – Tessera AlarmClock v0.3.8

## Ziel
Das unerwünschte blaue Bearbeiten-Symbol von der Tessera-Weckerhauptkachel entfernen. Das rote X zum gezielten Abbrechen eines Schlummeralarms muss erhalten bleiben. Ansonsten v0.3.7 unverändert übernehmen.

## Arbeitsschritte
- [x] Vollständige v0.3.7 als Basis übernommen (Display, fünf Wecker, Audio, Home Assistant, Tests).
- [x] Normalzustand: Symbolhintergrund **und alle neun getrennten Symbolteile** werden ausgeblendet.
- [x] Schlummerzustand: rotes X bleibt sichtbar; Antippen bricht nur den gewählten Schlummeralarm ab.
- [x] Restliche Kachel weiterhin zur Weckerübersicht navigierbar.
- [x] Version, Dokumentation, Regressionstest aktualisiert.
- [x] Lokale automatisierte Tests ausgeführt (siehe TESTBERICHT.md).
- [ ] ESPHome-Firmware auf Hilde kompilieren und übertragen.
- [ ] Physischer Displaytest: Normalzustand ohne Symbol; Schlummermodus mit rotem X.

## Probleme und umgangene Fehler
- **LVGL-Elemente unabhängig:** Das Symbol besteht aus einem Hintergrundobjekt und neun separaten Rechtecken. Nur den Hintergrund auszublenden hätte sichtbare Reste hinterlassen. Deshalb werden alle Komponenten gemeinsam versteckt.
- **Unbeabsichtigtes Abbrechen:** Der bisherige touchbasierte Abbruch bleibt auf die rechte untere Ecke begrenzt und wird ausschließlich im Schlummermodus ausgelöst.
- **Kompatibilität:** Keine neuen LVGL-Komponenten oder externen Abhängigkeiten; bisherige Audiologik und persistente Wecker bleiben erhalten.
- **Verifikation:** Lokale Mock-Tests sind kein Ersatz für den echten ESPHome-Build und das reale Touchdisplay.

## Installation
1. Bisherige funktionierende Version bei Bedarf sichern; vorhandene Wecker nicht löschen.
2. ZIP entpacken und **alle Dateien und Verzeichnisse** in das Stammverzeichnis von `Karlstahl1982/tessera-alarmclock` übertragen, vorhandene Dateien ersetzen, auf Branch `main` committen.
3. Tessera: **Plugins → Mit einem Link hinzufügen → Branch testen**, Repository `https://github.com/Karlstahl1982/tessera-alarmclock`, Branch `main`; **0.3.8** prüfen.
4. Firmware für Hilde kompilieren, nach erfolgreichem Build flashen.
5. Hauptkachel im Normalbetrieb prüfen: **keinerlei Symbol** unten rechts, Kachel öffnet bei Berührung die Übersicht.
6. Einen Testwecker schlummern lassen: **rotes X** muss erscheinen; nur ein Tipp auf das X soll den Schlummeralarm beenden. Andere Wecker unverändert.
7. Home-Assistant-Karte braucht **kein** Update.
