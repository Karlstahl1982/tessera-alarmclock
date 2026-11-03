# Tessera AlarmClock 0.1.0

Vollständiger **Release Candidate** für **Guition ESP32-S3-4848S040** / Tessera Screen Manager (ESPHome 2026.9.0). Kompatibel zum bereits funktionsfähigen Audio-Override mit `rtttl: id: guition_tone`.

## Funktionsumfang
- Fünf unabhängig gespeicherte Wecker mit Uhrzeit und Aktivierung.
- **iOS-inspirierte Uhrzeiteingabe:** zwei vertikal drehbare LVGL-Roller für Stunden (00–23) und Minuten (00–59), 3 sichtbare Zeilen, mittige Auswahl.
- Modi: nächstes Auftreten einer Uhrzeit, einmalig mit Datum, wöchentlich mit frei wählbaren Wochentagen.
- Fünf eigene RTTTL-Tonfolgen, je Wecker konfigurierbar.
- Lautstärke je Wecker 10–100 % des intern sicher begrenzten Ausgangspegels. Anstieg 10→100 % des gewählten Maximalpegels innerhalb von 60 Sekunden.
- Wecker klingelt maximal fünf Minuten je Alarmphase. Schlummern 5 oder 10 Minuten und höchstens 3 Wiederholungen je Wecker.
- Gleichzeitige Alarme getrennt verwaltet; Audioausgabe jeweils für den zuerst laufenden Alarm, die übrigen warten nicht auf ihren Startzeitpunkt.
- Wecker-Alarm als breites Pop-up mit **Stoppen** und **Schlummern**, aktiviert den Bildschirm.
- Bei gültiger, bereits synchronisierter Systemzeit lokale Alarmprüfung auch ohne aktuelle HA-Steuerbefehle.
- Verpasste Termine werden bis zu 5 Minuten nachgeholt; bereits ausgelöste Termine werden per Flash-Schlüssel pro Kalendertag vor Wiederholung nach Neustart geschützt.
- 40 native ESPHome-Entitäten für HA: pro Wecker Uhrzeit, Datum, Schalter, Modus, Ton, Snoozedauer, Lautstärke, Wochentagtext. Zwei HA-Test-/Stopptaster.

## Touchbedienung
1. AlarmClock-Kachel öffnen. Fünf Wecker werden angezeigt.
2. Weckerzeile antippen → Stunden-/Minuten-Drehrad. Die rechte Seite einer Weckerzeile schaltet nur an/aus.
3. **Speichern** übernimmt die Uhrzeit. **Weitere Einstellungen** übernimmt die Uhrzeit und öffnet Modus, Ton, Wochentage, Lautstärke, Snooze und An/Aus.
4. In **Weitere Einstellungen** die Zeile antippen, um Werte umzuschalten. Wöchentliche Alarme: die sieben Wochentage einzeln antippen. Modus `Einmalig am Datum`: die Datumszeile öffnet drei Drehräder (Tag/Monat/Jahr).
5. Während eines Alarms rechts **Stoppen**; links **Schlummern** (maximal 3-mal). In HA gibt es zusätzlich **AlarmClock Alarm stoppen**.

## Home Assistant
- Die 5 × 8 Konfigurationsentitäten werden automatisch vom ESPHome-Gerät angelegt.
- Mitgelieferte `LOVELACE_BEISPIEL.yaml` als Vorlage für eine manuelle Dashboard-Karte verwenden (Entity-IDs nach der Installation in HA prüfen und anpassen).
- Für den Wochentags-Text wird eine durch Kommas getrennte Liste verwendet: `Mo,Di,Mi,Do,Fr,Sa,So`; z. B. `Mo,Mi,Fr`. Leerer Text bedeutet keine aktiven Tage.
- Modus `Next` = nächstes Auftreten der gespeicherten Uhrzeit. `Date` = einmalig am Datum. `Weekly` = wöchentlich an ausgewählten Tagen.
- ESP32 ist führend; HA schreibt über die nativen ESPHome-Entitäten. Nach Wiederverbindung meldet ESPHome deren lokalen Zustand.

## Voraussetzungen und Installation
1. **Vor einem Update alle aktiven Wecker deaktivieren und die alten Zeiten notieren.** Beim Umstieg aus v0.0.5 ändern sich interne Entitäts-IDs; automatische Datenmigration ist nicht enthalten.
2. In `Hilde` muss bereits die funktionierende `guition_tone`-RTTTL-Audio-Konfiguration bestehen. Kein zweites `i2s_audio`/`speaker`/`rtttl` im Plugin anlegen.
3. Sämtliche Dateien aus dem ZIP **direkt** in das Root des GitHub-Repositories `https://github.com/Karlstahl1982/tessera-alarmclock` hochladen, vorhandene Dateien ersetzen und Commit auf `main`.
4. Tessera → Plugins → Mit Link hinzufügen → Branch testen → Repository / `main` → Plugin lesen; dann für Hilde aktualisieren.
5. Firmware **zuerst kompilieren**; bei Fehler nicht flashen, sondern den vollständigen Buildlog sichern. Erst nach einem sauberen Build auf das Display übertragen.
6. In HA fünf Uhrzeiten prüfen und zunächst einen Testalarm auslösen. Danach mit einem einmaligen Alarm ein paar Minuten in der Zukunft testen.
7. Nur nach erfolgreichem Praxis-Test als regulären Wecker verwenden; für kritische Termine einen zweiten unabhängigen Wecker nutzen.

## Technische Grenzen / offene Prüfungen
- **Nicht auf der realen Tessera-Firmware kompiliert oder auf Hilde geflasht.** C++ wurde lediglich mit einem Mock-API-Header auf Syntax geprüft, JSON/YAML/Python wurden syntaktisch validiert. Die echte LVGL-/ESPHome-Kompilierung und die Touchreaktion sind noch zu prüfen.
- Tessera setzt die Uhrzeit ursprünglich über HA. Ohne gültige Uhrzeit nach Neustart kann das Gerät ohne RTC nicht verlässlich wecken; bei laufender Uhr nach einer WLAN-Unterbrechung wird lokal weitergeprüft.
- Tessera kann die normale Bedienung bei langem Verbindungsverlust sperren. Alarm-Pop-up und Offline-Touchsteuerung sind deshalb **noch nicht als ausfallsicher bestätigt**.
- Das Pop-up ist **breit**, kein vollständiges randloses Fullscreen-Overlay. Das entspricht der Plugin-API.
- Die gewünschte per-Alarm-Helligkeit 10–100 % ist derzeit **nicht implementiert**, da die Plugin-API keine sichere per-Plugin-Rückstellung der Panelhelligkeit dokumentiert. Das Gerät wird geweckt, vorhandene Helligkeit beibehalten.
- Audio-Gain ist sicherheitshalber auf 15 % des RTTTL-Gains begrenzt (100 % AlarmClock = 15 % Digitalgain). Bei Knarzen in HA reduzieren.
- Eine Flash-Speicherung von Änderungen an ESPHome-Entitäten ist vorhanden, aber häufiges Umstellen kann Flash verschleißen; keine Sekundentakt-Aktualisierung.
- Sommerzeitumstellungen: interne nächste-Alarm-Prognose nutzt vereinfachte 24-Stunden-Tag-Abstände; die tatsächliche Auslösung prüft lokale Zeit/Datum und ist die maßgebliche Regel.

## Rückbau
Alte Version 0.0.5 über GitHub-Commit oder ZIP zurückspielen, in Tessera erneut Branch laden und Firmware neu erstellen. Vorher alle Wecker deaktivieren. Der Lautsprecher-Override bleibt unverändert.
