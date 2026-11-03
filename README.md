# Tessera AlarmClock 0.2.0

Neugestalteter lokaler Wecker für **Guition ESP32-S3-4848S040** und Tessera Screen Manager. Ausgangspunkt ist die auf dem Gerät bereits getestete Plugin-Version 0.1.0 mit funktionierendem Lautsprecher.

## Was ist neu?

- **Hauptkachel bis 2 × 3**: kann auf dem 4-Zoll-Guition die komplette Seite einnehmen (Größe wird im Tessera-Layouteditor eingestellt).
- **Weckerliste nur mit belegten Plätzen** und großer Schaltfläche „+ Wecker hinzufügen“; max. fünf Plätze.
- **Einrichtungsassistent am Touchdisplay:** Typ → Uhrzeit per zwei LVGL-Drehrädern → gegebenenfalls Datum/Wochentage → Alarmton/Hörprobe → Name, Lautstärke und Schlummern → Prüfen und Speichern.
- **Wecker bearbeiten/löschen**, Ein/Aus direkt in der Liste.
- **Eigene Home-Assistant-Karte** (`ha_card/`) mit Namen, Uhrzeitauswahl, Erstellen, Bearbeiten, Aktivieren und Löschen. Ein großer „Nächster Wecker“-Bereich; kein separater Rohentitäten-Block mehr auf dem Dashboard.
- **Namen pro Wecker**, auf dem Display aus sechs Kurzbezeichnungen, in Home Assistant frei bis 24 Zeichen.
- Die 40 bereits existierenden ESPHome-Entitäten bleiben erhalten und sind jetzt als Konfigurationsentitäten einsortiert. Zusätzlich fünf Namen, ein Verwaltungstext und ein Belegungssensor.
- **Lokal gespeicherte Belegung** (5-Bit-Maske im ESP32-Flash) und einmalige Erkennung bereits konfigurierter Wecker aus 0.1.0.

Die bislang funktionierende Alarm-Engine bleibt erhalten: wiederkehrende und einmalige Zeiten, fünf Original-RTTTL-Melodien, lokale Auslösung, 60-Sekunden-Lautstärkeanstieg, maximal fünf Minuten Alarmdauer, drei Schlummer-Wiederholungen à 5/10 Minuten. Der ESP32 bleibt führend.

## Installation – GitHub/Tessera

1. **Sicherheit:** Vor Aktualisierung vorhandene Weckzeiten dokumentieren. Bestehende Wecker während des Updates deaktivieren; für wichtige Termine einen zweiten Wecker verwenden.
2. Das ZIP entpacken. Dateien aus dem **ZIP-Hauptverzeichnis** direkt in den Root des GitHub-Repositories `https://github.com/Karlstahl1982/tessera-alarmclock` hochladen. Vorhandene Dateien ersetzen, auf `main` committen. Keine zusätzliche Ordnerstufe anlegen.
3. Tessera → Plugins → AlarmClock bzw. „Mit einem Link hinzufügen“ → **Branch testen** → `main` neu lesen.
4. Neues GitHub-Commit kontrollieren. **Erst Firmware erstellen/kompilieren.** Bei Fehlern den Log sichern und nicht flashen.
5. Nach erfolgreichem Build auf **Hilde** installieren. Lautsprecher-YAML (RTTTL-ID `guition_tone`) bleibt unverändert.
6. In Tessera → Display „Hilde“ → Layout die AlarmClock-Kachel auf **2 × 3 / ganze Seite** vergrößern → „Speichern & senden“. Diese Layoutänderung erfordert keinen erneuten Firmware-Build.
7. Im Touchdisplay erst einen neuen Wecker anlegen, Tonprobe durchführen und einen einmaligen Alarm in 2–3 Minuten auslösen.

### Home Assistant-Karte

Die optionale Bedienkarte ist in [`ha_card/README.md`](ha_card/README.md) beschrieben. Einbau unter `/config/www` und Registrierung als JavaScript-Modul-Ressource. Zusätzlich die Beispieldatei `ha_card/LOVELACE_KARTE.yaml` verwenden. Die bisherige Rohdatenansicht kann danach vom Dashboard entfernt werden; die ESPHome-Geräteentitäten bleiben technisch bestehen.

## Wesentliche technische Hinweise

- **Kein eigenes RTC-Modul:** Nach einem kompletten Neustart ohne WLAN/NTP gibt es keine verlässlich gültige Uhrzeit. Ein bereits synchronisiertes, laufendes Gerät prüft Wecker lokal auch bei HA-/WLAN-Unterbrechung.
- Tessera kann seine reguläre Touchoberfläche bei längerem HA-Verbindungsverlust sperren. Die **vollständige Offline-Touchbedienbarkeit ist nicht durch einen Geräteversuch belegt**.
- Audio-Ausgänge und NS4168-Verstärker des Guition müssen weiterhin hardwareseitig auf Lautsprecherbetrieb geschaltet sein. Kein zweites RTTTL-Audiomodul definieren.
- Die bisherige Alarmansicht nutzt ein **breites Popup**, kein garantiertes echtes randloses Vollbild.
- Eine gesonderte Alarmhelligkeitsregelung (10–100 %) ist noch nicht implementiert.
- Die Wiedergabe-Lautstärke ist wie bisher per RTTTL-Gain softwareseitig auf 15 % begrenzt; bei Knarzen reduzieren.
- **Kein nachgewiesener vollständiger ESPHome-2026.9.0-Firmware-Build in dieser Umgebung**. Parser-/Struktur-/Browser-Syntax-Tests im Paket ersetzen das Testen auf dem Gerät nicht.
- Der HTML/JS-Teil ist ein zusätzliches Home-Assistant-Dashboard-Modul; aus dem Tessera-Plugin allein wird keine Lovelace-Karte automatisch installiert.

## Rückbau

Auf den vorherigen GitHub-Commit mit Version 0.1.0 zurückgehen, Tessera-Plugin neu laden und Firmware bauen. Die zusätzliche Lovelace-Ressource kann separat entfernt werden; bestehender Audio-Override wird nicht verändert.

## Verzeichnis

- `plugin.yaml`: ESPHome-Entitäten und Binding
- `tessera-plugin.yaml`: Tessera-Metadaten/Kachelgröße
- `components/alarmclock/`: Python-Codegen und C++-UI/Alarmsteuerung
- `translations/`: Anzeigeübersetzungen
- `ha_card/`: optionale Home-Assistant-Oberfläche
- `tests/`: statische Paketvalidierung und Browserlogiktests
- `ZUSAMMENFASSUNG.md`: Fortschritt, Schwierigkeiten, Installation
