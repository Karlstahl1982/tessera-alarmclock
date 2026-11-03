# Tessera AlarmClock v0.0.5

Schlanker Test-Wecker für **Hilde / Guition ESP32-4848S040**. **Noch kein verlässlicher Alltagswecker.**

## Was neu ist
- **Home Assistant:** Statt zwei Zahlen-Schiebereglern ein natives `time`-Feld (HH:MM) namens `AlarmClock Wecker 1 Uhrzeit`.
- **Touchdisplay:** AlarmClock-Kachel → erste Zeile (Wecker 1) → links auf `-` oder rechts auf `+` in den Zeilen Stunde und Minute tippen. Die Einstellung ist sofort lokal gespeichert.
- Aktivierung in derselben Ansicht und Home Assistant weiterhin möglich.
- Testton, Stoppen und 5-Minuten-Grenze unverändert.
- Vier zusätzliche Plätze bleiben Platzhalter.

## Achtung – Migration
Die alten `number`-Entitäten für Stunde/Minute werden durch eine neue `datetime`-Entität ersetzt. **Die bisher eingestellte Uhrzeit wird nicht übernommen.** Nach der Aktualisierung neu einstellen; Standard ist **07:00**. In HA eventuell alte, nicht mehr verfügbare Zahlen-Entitäten entfernen, wenn sie noch angezeigt werden. Der Aktivierungsschalter bleibt erhalten; vor dem Update **deaktivieren**, damit die neue Standard-Uhrzeit nicht unerwartet auslöst.

## Voraussetzungen
- Tessera mit Plugin-API mindestens 0.16, ESPHome 2026.9.0.
- Funktionsfähiger Audio-Override mit `rtttl: id: guition_tone`.
- Vorhandenes Plugin v0.0.4 auf Hilde oder eine kompatible Erstinstallation.

## Installation / Test
1. **Wecker 1 deaktivieren** und bestehende Override-YAML sichern.
2. ZIP entpacken und den gesamten Inhalt direkt in die Wurzel von https://github.com/Karlstahl1982/tessera-alarmclock auf `main` hochladen; vorhandene Dateien ersetzen und Commit erstellen.
3. Tessera → Plugins → *Mit einem Link hinzufügen* → *Branch testen* → Repository und `main` → *Plugin lesen*; das Plugin auf Hilde aktualisieren.
4. ESPHome-Kompilierung starten. Bei Compilerfehler: **nicht flashen**, Log prüfen, v0.0.4 weiter nutzen.
5. Nach erfolgreicher Kompilierung Firmware auf Hilde aktualisieren.
6. In Home Assistant unter dem ESPHome-Gerät Hilde die neue Uhrzeit-Entität `AlarmClock Wecker 1 Uhrzeit` auf z. B. 07:15 setzen; auf dem Touchdisplay die Kachel öffnen und kontrollieren, ob 07:15 angezeigt wird.
7. Testweise auf dem Touchdisplay Stunde oder Minute um 1 ändern; prüfen, ob Home Assistant die Uhrzeit übernimmt.
8. Erst danach den Wecker für einen ungefährlichen Funktionstest aktivieren. **Nicht als einzigen Wecker verwenden.**

## Grenzen
- Nur ein Einmalwecker aktiv; keine Wochentage, kein Snooze, kein Vollbild-Alarm, keine 5-Minuten-Nachholung.
- Ohne gültige Systemzeit keine Alarmauslösung; bei WLAN-Verlust im laufenden Betrieb hängt Offline-Verhalten von Tessera-Uhr ab.
- Compiler und Hardware wurden hier nicht ausgeführt; YAML/JSON/Python und Paketstruktur wurden geprüft.

## Rückbau
Die GitHub-Dateien aus v0.0.4 zurückspielen, in Tessera erneut importieren und die vorherige Firmware installieren. Die Audio-Override-YAML unverändert lassen.

## Quellen
- https://esphome.io/components/datetime/template/
- https://github.com/MaxGramser/tessera-plugins/blob/main/docs/FIRMWARE_API.md
