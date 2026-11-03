# Testbericht – Tessera AlarmClock v0.3.6

## Prüfumfang
- Einheitstests der neuen 5-%-/15-Sekunden-Lautstärkekurve (Stufengrenzen, maximale Lautstärke, Countdown, Fortschritt und digitales Gain).
- Laufzeittest des I²S-Lautstärkeaufrufs (bei 0 s initial und bei 15 s / 30 s / 60 s jeweils genau ein Update).
- Bestehende Tests zu Cold-Start, 32-Bit-millis-Überlauf, erstem Alarm, Schlummern und Bedienoberfläche.
- C++-Syntaxprüfung mit ESPHome/Tessera-Mocks, JSON-/YAML-Validierung und Home-Assistant-Karten-Smoke-Test.

## Ergebnis
- **25 automatisierte Testfälle erfolgreich** (`python -m unittest discover -s tests -v`).
- C++-Schnittstellen-Mock-Kompilierung mit `-Wall -Wextra -Werror` erfolgreich.
- Native Audio-Tests erfolgreich (0/15/30/60 Sekunden, Zielwertbegrenzung, erster Weckalarm, Audio-Startwiederholung und Schlummern).
- JSON-/YAML-/JS-Konsistenz erfolgreich.
- **Offen:** realer ESPHome-Firmware-Build und Hörtest auf dem Guition-Display.

## Grenzen
- Der Mock testet nicht die konkrete NS4168-/I²S-Lautsprecherhardware.
- Prozentwerte sind Software-Sollwerte; hörbare Lautstärke hängt vom Lautsprecher/Verstärker ab.
- Ein echter Firmware-Build und akustische Prüfung sind erforderlich.
