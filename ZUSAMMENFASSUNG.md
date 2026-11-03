# Tessera AlarmClock – Projektzusammenfassung v0.0.5

## Ziel
Kompakter, lokaler Wecker auf Guition ESP32-4848S040 mit Tessera-Touchoberfläche, fünf Weckerplätzen, Alarmton und Home-Assistant-Verwaltung. Der ESP32 soll später die führende Quelle für Weckzeiten sein. Ein zuverlässiger Alarm ist derzeit **noch nicht gewährleistet**.

## Arbeitsschritte / Status
- [x] Lautsprecher auf Hilde funktionsfähig, Testton kommt.
- [x] Plugin-Skelett und fünf Weckerplätze (v0.0.3) werden angezeigt.
- [x] Erster Einmalwecker mit lokalem Auslösen und Testton (v0.0.4) erstellt; User bestätigt Ton und Stoppen.
- [x] Problem erkannt: v0.0.4 nutzt Zahlen-Schieberegler 0–59 statt echte Uhrzeiteingabe.
- [x] v0.0.5: native ESPHome-Zeit-Entität für HH:MM und separate -/+ Touchbedienung implementiert; Syntax-/Paketchecks ausgeführt.
- [ ] v0.0.5 in Tessera kompilieren und auf Hilde testen (noch nicht geprüft).
- [ ] Lokale Alarmzustände/Flash-Persistenz und Offline-Verhalten testen.
- [ ] Vier weitere Wecker implementieren.
- [ ] Später: Wochentage, Schlummern 5/10 Minuten max. 3-mal, 5-Minuten-Nachholen, Vollbild-Alarm, sanfter Lautstärkeanstieg 60 Sekunden, Alarmhelligkeit.
- [ ] Später: eigene Home-Assistant-Dashboard-Karte.

## Probleme / Umgehungen
- **Schieberegler**: Umgestellt von zwei `number`-Entitäten auf `datetime: type: time`. Damit wird Home Assistant eine Uhrzeitauswahl anbieten; die Touch-Karte ändert Stunden/Minuten getrennt.
- **Migration:** Alte Stunde/Minute-Werte werden nicht übernommen; die neue Uhrzeit startet bei 07:00. Aktivierung vor dem Update ausschalten und Uhrzeit danach kontrollieren.
- **Lautsprecher:** Guition-Board wurde hardwareseitig für Audio umgerüstet. Das funktionierende Tessera-Override mit `guition_tone` bleibt Voraussetzung.
- **Offline-Zeit:** Ohne gültige Uhrzeit nach Neustart kein verlässlicher Wecker; keine RTC-Hardware vorgesehen.
- **Teststand:** Kein echter Firmware-Compiler/Hardware-Zugriff in dieser Umgebung; C++-Build auf Hilde über Tessera erforderlich.

## Installation
1. **Wecker 1 deaktivieren**; Override-YAML und letzte funktionierende Firmware sichern.
2. ZIP entpacken; **alle enthaltenen Dateien und Ordner direkt im GitHub-Repository-Stammverzeichnis** `Karlstahl1982/tessera-alarmclock` auf Branch `main` hochladen und ersetzen.
3. Tessera → Plugins → *Mit einem Link hinzufügen* → *Branch testen* (`main`) → Plugin lesen, v0.0.5 für Hilde übernehmen.
4. Firmware kompilieren; bei Fehler keine Installation, sondern Buildlog prüfen.
5. Bei Erfolg installieren, `AlarmClock Wecker 1 Uhrzeit` in Home Assistant einstellen und im Touchdisplay testen.
6. Erst danach den Wecker zu einer Testzeit aktivieren. Nicht als einzigen Wecker verwenden.

## Rollback
Dateien von v0.0.4 in GitHub wiederherstellen, Plugin erneut einlesen und Firmware neu erstellen. Audio-Override unverändert.
