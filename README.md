# Tessera AlarmClock v0.0.4

**Erste funktionsfähige Teststufe – noch kein verlässlicher Alltagswecker.**

## Funktionsumfang
- Ein **einmaliger Alarm beim nächsten Auftreten der gewählten Uhrzeit** mit ESP32-interner Überwachung.
- Stunde, Minute und Aktivierung als ESPHome-Entitäten in Home Assistant und in **Tessera auf dem Display → Einstellungen → Plugins → AlarmClock**.
- Uhrzeit sowie Aktivierung werden auf dem ESP32 dauerhaft gespeichert; der Alarm ist zunächst deaktiviert.
- Bei Auslösung deaktiviert sich der Einmalwecker; der RTTTL-Ton wird maximal 5 Minuten wiederholt. 
- Ein eigener **15-Sekunden-Testalarm** und ein **Alarm stoppen**-Button in Home Assistant.
- Auf dem Display zeigt die Kachel den aktiven Wecker. In der Detailansicht aktiviert/deaktiviert ein Tipp auf die erste Zeile den Wecker; während eines aktiven Alarms stoppt ein Tipp den Alarm.
- Die Plätze 2 bis 5 bleiben Platzhalter, sodass das Plugin klein bleibt.

## Technische Voraussetzungen

- Hilde (Guition ESP32-4848S040), bereits funktionierende Tessera-Installation; Plugin API mindestens 0.16.
- Audio-Override-YAML enthält **`rtttl: id: guition_tone`** und die vorhandene I²S-Ausgabe. Diese Datei **nicht entfernen**. Der Stand 0.0.1 oder 0.0.2 des Lautsprechertests bietet diesen ID.
- Gültige Uhrzeit: Tessera stellt `epoch()` bereit. Ohne anfänglich übernommene Uhrzeit wird nicht geweckt. Nach HA-/WLAN-Ausfall bei weiterlaufendem Board funktioniert die zuvor gestellte Uhr weiter, muss aber noch praktisch getestet werden.
- Die Alarmauslösung ist derzeit auf den exakten Minutenwechsel ausgelegt: **keine 5-Minuten-Nachholung**, keine Wochentage, kein Snooze, kein Vollbild-Popup und kein Alarm bei fehlender Gerätezeit. Noch nicht als alleinigen Wecker verwenden!

## Update / Installation

1. Die letzte funktionierende Firmware sowie `hilde.local.yaml` sichern.
2. Das ZIP entpacken; **alle Dateien und Ordner unmittelbar in die Wurzel** von `https://github.com/Karlstahl1982/tessera-alarmclock` auf `main` hochladen; vorhandene Dateien ersetzen; Commit.
3. Tessera → Plugins → Mit einem Link hinzufügen → Branch testen → Repository `https://github.com/Karlstahl1982/tessera-alarmclock`, Branch `main` → Plugin lesen.
4. Plugin auf **Hilde** aktualisieren; prüfen, dass **0.0.4** und ein neuer GitHub-Commit verwendet werden.
5. Firmware erstellen und installieren. Wenn der Build scheitert, die letzte Firmware unverändert lassen und das Buildlog weitergeben.
6. Home Assistant → Einstellungen → Geräte & Dienste → ESPHome → Hilde: `AlarmClock Testalarm` drücken. Er soll rund 15 Sekunden klingen oder früher mit `AlarmClock Alarm stoppen` enden.
7. Danach `AlarmClock Wecker 1 Stunde` / `Minute` auf eine kommende Uhrzeit stellen und `AlarmClock Wecker 1 aktiv` einschalten. Warten, bis die Uhrzeit erreicht ist; der Einmalwecker wird danach automatisch deaktiviert.

**Bei der Installation nur Schritt 1–4 durchführen, danach die Firmware bauen und das Ergebnis melden.**

## Rückbau

AlarmClock 0.0.3 im Repository wiederherstellen, in Tessera Branch neu laden und Firmware zurückbauen. Die Audio-Override-YAML nicht verändern.

## Quellen
- Tessera Plugin API: https://github.com/MaxGramser/tessera-plugins/blob/main/docs/FIRMWARE_API.md
- ESPHome RTTTL: https://esphome.io/components/rtttl/
