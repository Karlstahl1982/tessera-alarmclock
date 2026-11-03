# Changelog

## 0.0.4 – 2026-11-03
- Erste lokale Einmal-Weckzeit (Stunde / Minute / aktiv), Anzeige und Touch-Umschaltung.
- Drei native ESPHome-Entitäten und zwei Diagnose-Buttons für Home Assistant.
- Bereits vorhandenes `guition_tone`-RTTTL für die Ausgabe eingebunden, ohne neue Audio-Pins.
- 15-Sekunden-Soundtest, maximal 5 Minuten Alarm, manuelles Stoppen.
- Plugin API 0.16: Gerät beim Alarm aufwecken.
- Plätze 2–5 bewusst weiterhin Platzhalter. Aktuelle Stufe nicht als alleinigen Wecker einsetzen.

## 0.0.3 – 2026-11-03
- Compilerfehler in `AlarmOverview` korrigiert (`const char*` statt `.c_str()`).

## 0.0.2 – 2026-11-03
- GitHub-konformes Root-Layout.

## 0.0.1 – 2026-11-03
- Initiales Installationsgrundgerüst mit fünf Platzhaltern.
