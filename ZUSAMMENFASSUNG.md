# PROJEKTZUSAMMENFASSUNG – Tessera AlarmClock 0.1.0

**Ziel:** Ein kompakter, unabhängiger Wecker mit fünf Plätzen auf Guition ESP32-4848S040 / Tessera, per Touch und Home Assistant bedienbar.

## Arbeitsschritte / Status
- [x] Plugin-Integration mit Tessera und Audio-Testton in v0.0.5 auf Hilde bestätigt.
- [x] 5-Wecker-Kern, native Zeit- und Datumseingabe, Wiederholung/Terminlogik erstellt.
- [x] Apple-inspirierte Stunden-/Minuten-Drehräder implementiert (LVGL Roller, Wischgesten).
- [x] Alarme, fünf Tonfolgen, 60-s-Fade-in, fünfminütiges Abspielen, Stoppen und Schlummern (max. 3) implementiert.
- [x] 5 Minuten Nachholen und 1 Auslösung pro Datum mit Flash-Status implementiert.
- [x] Home-Assistant-Entitäten und Dashboard-Vorlage erzeugt.
- [x] YAML/JSON/Python-Validierung, C++-Mock-Syntaxprüfung durchgeführt.
- [ ] Vollständiger ESPHome-Realbuild gegen Tessera screens-v0.4.96 (nur beim Nutzer verfügbar).
- [ ] Touch-Gesten, Popup, lokales Alarmverhalten, HA-Synchronisierung in Hardware testen.
- [ ] Offline-Bedienung bei Tessera-Verbindungsverlust über 5 Minuten verifizieren bzw. Firmware-Patch entwerfen.
- [ ] Alarmhelligkeit inkl. Wiederherstellung des vorherigen Werts ergänzen (Tessera-API nötig).

## Probleme / Workarounds
- Fehler der früheren v0.0.2 (`const char*` plus `.c_str()`) ist in v0.0.3 behoben und in dieser Version vermieden.
- Falsche HOME-ASSISTANT-Plugin-Installationsroute der v0.0.1 korrigiert: Installation ausschließlich über GitHub / Tessera-Branch.
- Slider-Steuerung der v0.0.4 durch native Zeit- und Datums-Drehräder ersetzt.
- Lautsprecher-Hardware in Hilde wurde durch Umschaltung R25–R27 → R21–R23 aktiviert; Audio-Override (`guition_tone`) bleibt nötig.
- Konfigurationsmigration v0.0.5 → v0.1.0 nicht automatisch: Wecker 1 neu eingeben, HA-Entitäts-IDs können wechseln.
- Hardwarezugriff in dieser Laufzeit nicht verfügbar: kein unbestätigtes Erfolgsversprechen.

## Installation
1. Alle Wecker deaktivieren und Zeiten notieren.
2. ZIP entpacken, gesamten Inhalt im Root des GitHub-Repository `Karlstahl1982/tessera-alarmclock` auf `main` ersetzen.
3. Tessera → Plugins → Mit Link hinzufügen → Branch testen → main → Plugin lesen.
4. Aktualisierung für Hilde starten, zuerst kompilieren; erst nach erfolgreichem Build OTA installieren.
5. HA: acht Entitäten pro Wecker zuordnen und `LOVELACE_BEISPIEL.yaml` bei Bedarf anpassen.
6. Testton starten; einmaligen Alarm in wenigen Minuten und anschließend wöchentlichen Alarm prüfen.
7. Bei Fehlern v0.0.5 zurückspielen, Audio-Override nicht verändern.

**Betriebshinweis:** Release Candidate, nicht als einzigen kritischen Wecker verwenden, bis Realtest, Offline- und Langzeitverhalten bestätigt sind.
