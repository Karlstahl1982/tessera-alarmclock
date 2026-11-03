# Tessera AlarmClock 0.3.0 (Beta)

Schlanker Offline-Wecker für das Guition ESP32-4848S040 mit Tessera und Home Assistant.

## Display-Bedienung

- Die Tessera-Kachel „Nächster Wecker“ öffnet die grafische Weckerübersicht.
- Je Wecker: Uhrzeit, Name, wiederkehrend/einmalig und Statusplakette **AN/AUS**.
- Statusplakette oben rechts antippen: direkt ein- oder ausschalten.
- Rest der Weckerkachel antippen: grafische Bearbeiten-Ansicht.
- `+`-Kachel: neuen Wecker über bestehenden Assistenten erstellen (maximal fünf).
- Bearbeiten: Uhrzeit (auch im großen Uhrzeitfeld), Rhythmus, Klang oder Lautstärke/Schlummern als Kacheln öffnen.
- Der Wecker wird erst nach Tippen auf **Speichern** übernommen.
- **Alarm-Vollbild** lässt sich nur über Stoppen oder Schlummern quittieren.

## GitHub / Tessera aktualisieren

1. ZIP entpacken und alle Dateien aus dem Paket direkt in den Stamm des GitHub-Repositories `Karlstahl1982/tessera-alarmclock` hochladen und ersetzen.
2. Commit auf `main` durchführen.
3. Tessera → Plugins → Mit einem Link hinzufügen → **Branch testen** (`main`).
4. Vergewissern, dass Version **0.3.0** und der neue Commit verwendet werden.
5. Firmware zunächst nur kompilieren. Bei Erfolg auf „Hilde“ übertragen.
6. Weckerkachel öffnen und Raster/Bearbeitung prüfen.

**Kompatibilität:** Basiert auf funktionierender v0.2.4; ESPHome 2026.9.0, Tessera screens-v0.4.96. Tests mit C++-Mock-Schnittstellen sind keine Bestätigung eines echten Firmware-Builds.

**Home Assistant:** Die bisherige `ha_card/tessera-alarmclock-card.js` bleibt funktionsgleich. Kein erneutes Installieren erforderlich.

**Hinweis:** Vorherige Weckzeiten bleiben über ESPHome-Entitäten und lokale Präferenzen bestehen. Zur Sicherheit wichtige Weckzeiten vor dem Flashen notieren.
