# Tessera AlarmClock – Projektzusammenfassung

**Version:** 0.3.0 (Beta)  
**Ziel:** Grafischer, intuitiv bedienbarer eigenständiger Wecker auf Guition ESP32-4848S040, mit fünf lokalen Alarmen und Home-Assistant-Anbindung.

## Arbeitsschritte

- [x] v0.2.4 als funktionierenden Ausgangsstand übernommen.
- [x] Grafische 2-spaltige Weckerliste für bis zu fünf Alarme mit Plus-Kachel umgesetzt.
- [x] Kachel mit Uhrzeit, Name, Rhythmus und Status AN/AUS.
- [x] 2×2-Bearbeitungskacheln für Uhrzeit, Wiederholung, Klang und Lautstärke/Schlummern.
- [x] Bestehende Detailseiten und den Drehrad-Uhrzeitdialog angebunden.
- [x] Blockierenden Alarm und Audioausgabe aus v0.2.4 unverändert gelassen.
- [x] Übersetzungen, Manifest und automatisierte Tests aktualisiert.
- [ ] Echten ESPHome-Build unter Tessera auf dem Zielgerät prüfen.
- [ ] 480×480-Display auf Lesbarkeit, Hitboxen und Speicherauslastung prüfen.
- [ ] Offline-Funktion bei HA-/WLAN-Ausfall erneut prüfen.

## Probleme / Umgehungen

- **LVGL Button-Widget fehlte in 0.2.3:** In v0.2.4 bereits durch Standard-LVGL-Objekt ersetzt; v0.3.0 übernimmt diese Lösung.
- **Kleine Displayfläche:** Texte reduziert, Weckerkacheln und Bearbeiten-Raster eingeführt. Ob Schriftgrößen bei der individuellen Tessera-Skalierung passen, muss im Gerätetest bestätigt werden.
- **Fehlende Font-Glyphen für Emoji/Symbole:** Uhrgrafiken aus LVGL-Formen statt nicht garantierter Symbolfonts.
- **Speicher:** Card-Widgets werden einmalig beim Öffnen erstellt, nicht bei jedem UI-Tick. Das Manifest erhält ein erhöhtes Speicherbudget; tatsächlichen Verbrauch im Build beobachten.
- **Kompatibilität:** C++-Syntaxprüfung mit Mocks ist kein ESPHome-Vollbuild. Bei Kompilierfehlern Buildlog prüfen; Firmware v0.2.4 bleibt bis zum erfolgreichen Update verwendbar.

## Installation

1. `Tessera_AlarmClock_0.3.0.zip` entpacken.
2. Gesamten Paketinhalt ohne äußeren Versionsordner in das bestehende öffentliche GitHub-Repository hochladen (`main`).
3. Tessera → Plugin via Link → Branch testen → `main`.
4. Version **0.3.0** prüfen, Firmware kompilieren; erst nach erfolgreichem Build flashen.
5. Hauptkachel „Nächster Wecker“ öffnen und alle Kacheln testen; insbesondere Status-Toggle und Speichern.
6. Die HA-Karte erfordert kein Update, da an ihr nichts geändert wurde.

**Rückweg:** Bei Problemen im Repository auf v0.2.4 zurückgehen und Tessera erneut bauen.
