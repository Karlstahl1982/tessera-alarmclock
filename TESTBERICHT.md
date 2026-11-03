# Testbericht – v0.3.2

**Automatische Tests:** 21 bestanden (Unittest-Discovery), darunter C++-Syntaxprüfung mit Mock-ESPHome/Tessera, C++-Laufzeittests für Audioerstauslösung, I²S-Stufenregelung und Kaltstart-Retries sowie JavaScript-Regressionstests der Home-Assistant-Karte.

**Lautstärke-Funktionslogik (60 % Zielwert):**

- 0 s → 30 %, nächste Stufe in 6 s
- 6 s → 33 %, nächste Stufe in 6 s
- 30 s → 45 %, nächste Stufe in 6 s
- 54 s → 57 %, nächste Stufe in 6 s
- 60 s → 60 %, Ziel erreicht

**Nicht durchgeführt:** ESPHome-2026.9.0-/ESP-IDF-Kompilierung mit originalem Tessera-Framework, OTA-Flash auf dem Guition, akustische Pegelmessung sowie Test der vollständigen Hardware-Wiedergabe. Der C++-Mocktest ist kein Ersatz dafür.

**Prüfschritte auf dem Gerät:** Erststart-Ton, Countdown 6 → 1, hörbarer Stufenanstieg alle 6 s, Ziel nach 60 s, Testton-Ende nach 75 s, Anzeige bleibt bis manuellem Stoppen sichtbar.
