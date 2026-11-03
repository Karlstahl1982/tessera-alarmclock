# Tessera AlarmClock v0.3.6

Lokaler Wecker für Guition ESP32-4848S040 mit Tessera. Vollständiges Plugin mit fünf Weckern, Alarm-Vollbild, Schlummerfunktion und zentralen Home-Assistant-Entitäten.

## Lautstärkekurve (neu)

- **0 Sekunden:** 5 %
- **15 Sekunden:** 10 %
- **30 Sekunden:** 15 %
- **60 Sekunden:** 25 %
- **165 Sekunden:** 60 % (falls 60 % als Maximallautstärke eingestellt)
- **285 Sekunden / 4:45 Minuten:** 100 % (falls 100 % als Maximum eingestellt)
- **300 Sekunden / 5:00 Minuten:** automatisches Ende des Alarmtons (Alarmanzeige bleibt bis zur Quittierung sichtbar)

Die Kurve endet bei der **pro Wecker eingestellten Maximallautstärke**. Die Prozentanzeige ist eine Software-Skalierung, keine Messung des Schallpegels. Ein Start bei 5 % kann bei kleinen Lautsprechern praktisch unhörbar sein.

Der Fortschrittsbalken wächst mit jeder 15-Sekunden-Stufe. Der Countdown zeigt die Sekunden bis zur **nächsten Erhöhung**, nicht bis zum endgültigen Zielwert. Der 75-Sekunden-Testalarm erlaubt die Prüfung der ersten vier Erhöhungen (bei 15, 30, 45 und 60 Sekunden).

## Installation / Update von v0.3.5

1. Vorhandene Weckzeiten vorsichtshalber notieren.
2. ZIP entpacken und **alle Dateien** direkt in das Hauptverzeichnis des bestehenden GitHub-Repositories `Karlstahl1982/tessera-alarmclock` laden, ältere Dateien ersetzen, `main` committen.
3. In Tessera unter **Mit Link hinzufügen → Branch testen** den neuen Commit von `main` einlesen. Manifestversion **0.3.6** kontrollieren.
4. Auf Hilde zunächst einen **ESPHome-Firmware-Build** ausführen. Nur bei erfolgreichem Build installieren.
5. Den Testalarm starten und 0 s / 15 s / 30 s / 45 s / 60 s prüfen. Es sollten 5 % / 10 % / 15 % / 20 % / 25 % angezeigt werden; jeweils 15 Sekunden bis zum nächsten Schritt.
6. Den echten Weckalarm mit einer kurzen Testzeit prüfen; bis zur Bestätigung einen zweiten unabhängigen Wecker nutzen.

**Home Assistant:** Es bleibt bei drei zentralen Entitäten. Die separat installierte Dashboard-Karte benötigt für dieses Update **keine** Anpassung.

## Technische Hinweise

- Die Lautstärke steigt digital über `rtttl::Rtttl::set_gain()` und `speaker::Speaker::set_volume()`; nur bei Wechsel der Stufe wird der laufende I²S-Ausgang neu gesetzt.
- Nach einem Geräte-Neustart ohne Zeitquelle ist ohne RTC kein verlässlicher Weckalarm garantiert.
- Der native ESPHome-Firmware-Build mit echter Hardware und akustischer Test muss beim Anwender erfolgen.
- Die Wecker-Einstellungen bleiben durch die Änderung von v0.3.5 erhalten.
