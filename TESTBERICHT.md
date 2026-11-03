# Testbericht – v0.3.3

**Ursache:** Der erste alarmierende `on_interval`-Aufruf verwendete zwei unterschiedlich alte Millisekundenwerte. Ein um 1 ms späteres `millis()` wurde als Alarmbeginn gespeichert, doch die Prüfung im gleichen Tick benutzte einen zuvor erfassten Zeitpunkt. Das führte bei unsignierter Subtraktion sofort zum 5-Minuten-Audio-Timeout.

## Reproduzierbarkeit

- [x] `test_first_scheduled_alarm.cpp` auf der ungepatchten Logik von v0.3.2 ausgeführt: `player.playing` ist nach dem ersten Tick **false** (Assertion fehlgeschlagen, Exitcode 134).
- [x] Gleicher Test mit v0.3.3: erster planmäßiger Alarm bleibt aktiv und Audio-Player läuft.
- [x] Derselbe Test prüft das Schlummern und die erneute Auslösung.
- [x] `test_millis_rollover.cpp` simuliert den Zählerüberlauf nach rund 49,7 Tagen, ohne Sofortablauf; die Quittierung nach Ablauf der Audiozeit bleibt manuell.

## Unverändertes Audioverhalten

- RTTTL-/I²S-Audiostart einschließlich maximal zweier Retries
- Lautstärke bei 60 % Zielwert: 0 s = 30 %, 6 s = 33 %, 30 s = 45 %, 60 s = 60 %
- Anzeige der nächsten Erhöhung alle sechs Sekunden
- Testalarm: 75 s Audio, danach blockierende Anzeige bis zum manuellen Stoppen

## Grenzen

C++-Komponententests laufen mit nachgebildeten Tessera-/ESPHome-Schnittstellen. Ein echter ESP-IDF-Firmware-Build, OTA-Flash und akustische Prüfung am Guition waren hier nicht möglich. Die Zahl bestandener Unit-Tests ist dem aktuellen Testergebnis zu entnehmen.
