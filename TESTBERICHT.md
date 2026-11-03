# Testbericht – Tessera AlarmClock v0.3.8

## Automatisierte Prüfungen
- **27 Tests erfolgreich** (`python -m unittest discover -s tests -p 'test_*.py' -v`).
- C++-Syntax- und Mock-Laufzeittests einschließlich geplanter Weckzeit, Audiostart, Lautstärkestufen, Schlummermodus und zielgerichtetem Touch-Abbruch erfolgreich.
- Neue Regression prüft: Normales Bearbeiten-Symbol entfernt; sämtliche Einzelkomponenten im Normalzustand versteckt; rotes X wird nur bei Schlummern gezeichnet.
- YAML-/Manifest-, Übersetzungs-, JavaScript- und Layout-Prüfungen erfolgreich.

## Offene Hardwareprüfung
- [ ] Echter ESPHome-Firmware-Build in der Tessera-/Hilde-Umgebung.
- [ ] Sichtkontrolle auf Hilde: **kein** Normalzustand-Symbol, rotes X nur beim Schlummern.
- [ ] Touchkontrolle: normale Kachel öffnet Übersicht, rotes X beendet nur betreffenden Schlummeralarm.

Es gab keine Änderungen an Audio, persistentem Weckerspeicher, Home-Assistant-Anbindung oder Schaltlogik außerhalb der Icon-Sichtbarkeit.
