# Projektzusammenfassung – Tessera AlarmClock

**Version:** 0.3.2 (Beta)  
**Ziel:** Zuverlässig hörbarer Alarm bereits beim ersten Auslösen; nachvollziehbarer, über 60 Sekunden steigender Pegel mit aktueller %-Anzeige und Countdown bis zur nächsten Stufe. Bestehende Bedienung und Funktionalität erhalten.

## Arbeitsschritte / Status

- [x] Ausgangsbasis v0.3.1 vollständig übernommen.
- [x] Audio-Code überprüft: anfänglicher Gain und I²S-Verhalten untersucht.
- [x] Alarmstart vor LVGL-Vollbildaufbau; separate Initialisierung des Audioplayers.
- [x] Startüberwachung und maximal zwei begrenzte Retries hinzugefügt.
- [x] Lautstärke in zehn Stufen à sechs Sekunden mit Live-Aktualisierung des I²S-Ausgangs.
- [x] Alarmanzeige erweitert: aktueller %-Wert, Zielwert, Balken, Countdown, Audio-Startstatus.
- [x] Testalarm auf 75 Sekunden verlängert, damit der gesamte Rampenverlauf überprüfbar ist.
- [x] YAML/Manifest/Übersetzungen und Installationsdokumentation aktualisiert.
- [x] Tests: 21 Python-Unittests mit C++-Mockkompilierung, auszuführenden C++-Audiotests und JS-Regression bestanden.
- [ ] ESPHome-2026.9.0-Firmware **real** kompilieren.
- [ ] 4-Zoll-Display: optische Darstellung der Countdown-Labels prüfen.
- [ ] Akustischer Test: erster Ton unmittelbar beim ersten Start, Stufen nach 6/12/…/60 s tatsächlich hörbar.
- [ ] Reale Wecker-Auslösung mit Snooze, Auto-Ende und WLAN-/HA-Ausfall erneut prüfen.

## Probleme / Workarounds

1. **Erster Alarm sehr leise oder stumm:** Der frühere digitale Ausgangspegel bei 60 % Zielwert lag anfangs bei nur 0,9 %. Jetzt höherer Startpegel und begrenzte Kaltstartwiederholung; keine definitive Hardware-Diagnose ohne Live-Test.
2. **Veränderter RTTTL-Gain während Playback:** Beim I²S-Pfad übernimmt RTTTL den Gain beim INIT. Eine alleinige `set_gain()`-Aktualisierung bewirkt keine zuverlässige Änderung während einer Melodie. Umgangen durch die zusätzliche, gezielte `speaker_->set_volume(...)`-Nachregelung beim Stufenwechsel.
3. **Testdauer bisher 15 Sekunden:** Zu kurz für 60-sekündigen Verlauf. Test auf 75 Sekunden erweitert; manueller Stopp jederzeit möglich.
4. **Schallpegel nicht messbar:** Prozentangaben sind relative Software-Sollwerte, kein gemessener Schalldruck. Software-Gain auf 0,35 begrenzt, keine automatische Kalibrierung.
5. **Kompilierbarkeit:** C++-Syntax mit Mock-Schnittstellen und Tests bestanden, echter ESPHome-/ESP-IDF-Build lokal nicht möglich. Vor OTA auf Zielsystem kompilieren.

## Installationsanleitung

1. ZIP entpacken; Gesamtinhalt direkt in `https://github.com/Karlstahl1982/tessera-alarmclock` (`main`) hochladen/ersetzen.
2. Tessera → Plugins → Branch testen → `main`; Version **0.3.2** und GitHub-Commit prüfen.
3. Bestehende lokale Audio-YAML (`guition_tone`, `guition_speaker`) beibehalten.
4. ESPHome-Firmware **nur kompilieren**; erst bei Erfolg OTA übertragen.
5. Testalarm mit 40–60 % Sollpegel starten; Anzeige und Hörbarkeit ab Sekunde 0 und Stufen bis 60 s kontrollieren.
6. Test über „Stoppen“ beenden; anschließend reale Weckzeit überprüfen. Bis dahin anderen Wecker bereithalten.

**Rollback:** Vorheriger bewährter Stand v0.3.1 aus Version/Commit wiederherstellen, sollte der Build oder Ton fehlschlagen.
