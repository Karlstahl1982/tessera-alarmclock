# Testbericht v0.3.4

## Automatisierte Prüfungen

- [x] 24 Python-/C++-/JS-Tests erfolgreich (`python -m unittest discover -s tests -v`).
- [x] C++-Syntaxprüfung mit dem bisherigen Tessera-/ESPHome-Mock.
- [x] Audiotest: Startpegel, 1-Sekunden-Update, gleitende Gain-Anpassung, Zielwert nach 60 s.
- [x] Laufzeit-Regression: erster planmäßiger Alarm, Cold Start, 32-Bit-Millis-Überlauf.
- [x] Laufzeit-Regression: einmaliger Wecker bleibt während der Schlummerphase `next_alarm()`, selbst nach Deaktivierung.
- [x] Globale Unterdrückung beendet aktive und schlummernde Alarme; keine per-Wecker-HA-Entitäten im Paket öffentlich.
- [x] JS-Runtime: Statusanzeige, Unterdrücken/erneut zulassen und Stoppen.
- [x] LVGL-Strukturprüfung: Balkenrechtecke als Geschwister mit expliziter Position, Padding null.

## Noch offen

- [ ] Reale ESPHome-2026.9.0-Kompilierung mit Tessera Screens v0.4.96.
- [ ] OTA-Flash und Prüfung auf Guition ESP32-4848S040.
- [ ] Sichtprüfung des fortlaufenden Balkens und akustische Prüfung der 60-Sekunden-Gain-Rampe.
- [ ] Schlummertest auf dem realen Display und Statusanzeige in der Home-Assistant-Karte.
- [ ] HA-Entitätenregistrierung nach dem Wechsel zu `internal: true` kontrollieren.

## Grenzen

Die C++-Tests verwenden nachgebildete Schnittstellen. Hardwareabhängiges LVGL-Rendering und das tatsächliche Audioverhalten können hier nicht garantiert werden.
