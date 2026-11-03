# Tessera AlarmClock 0.3.1 (Beta)

Eigenständiger Wecker für Guition ESP32-4848S040 mit Tessera / ESPHome und Home Assistant. Dieses Release verbessert **nur die Display-Bedienung**.

## Änderungen am Display

- Weckername steht **oben**; Uhrzeit **mittig**, Wiederholung **unten**, Aktivierungsstatus **rechts oben**.
- Auf die AN/AUS-Plakette tippen: schalten; auf die übrige Kachel tippen: bearbeiten.
- Bearbeitungsansicht mit vier Icon-Kacheln: Zeit, Rhythmus, Ton, Lautstärke/Schlummern.
- Wiederholung über drei Auswahlkarten, Wochentage über Chips, Töne über fünf Auswahlkarten plus Hörprobe.
- Optionen als vier Auswahlkarten. Bei Lautstärke links auf **−** und rechts auf **+** tippen.
- Uhrzeit und Datum weiterhin über vertikale Drehräder einstellbar.
- Die blockierende Alarmansicht bleibt unverändert.

## Installation über GitHub

1. Das ZIP entpacken.
2. **Alle enthaltenen Dateien und Unterordner** direkt im Stamm von `Karlstahl1982/tessera-alarmclock` ersetzen.
3. Auf Branch `main` committen.
4. Tessera → Plugins → Branch testen → `https://github.com/Karlstahl1982/tessera-alarmclock`, `main`.
5. Version **0.3.1** und den neuen GitHub-Commit prüfen.
6. Firmware **zuerst nur kompilieren**, anschließend bei Erfolg auf „Hilde“ übertragen.
7. Weckerraster, Bearbeiten, Wiederholung, Wochentage, Klangwahl, Optionen und Alarm-Vollbild prüfen.

## Home Assistant

`ha_card/tessera-alarmclock-card.js` ist enthalten und bleibt gegenüber v0.3.0 unverändert. **Kein erneutes Installieren erforderlich.** Die vorhandene Lautsprecher-YAML bleibt ebenfalls unverändert.

## Teststatus

Automatisierte Paket-, JavaScript-, C++-Syntax- und Layouttests. Dies ist **kein Ersatz** für den realen ESPHome-Build oder einen Test der Fontgrößen auf dem Touchdisplay.
