# Tessera AlarmClock 0.0.3

Kompakter **Integrationstest** für das Guition ESP32-4848S040 (Display **Hilde**) unter Tessera.

**Noch kein funktionsfähiger Wecker!** Die Kachel "Nächster Wecker" öffnet eine Ansicht mit fünf leeren Weckerplätzen. Es erfolgt weder Alarmauslösung noch Speicherung oder Home-Assistant-Synchronisierung. Nicht als zuverlässigen Wecker verwenden.

## Installation über GitHub

1. Dieses ZIP **entpacken**. Alle Dateien und Ordner aus dem entpackten Verzeichnis (insbesondere `tessera-plugin.yaml`, `plugin.yaml`, `components/` und `translations/`) direkt in das Hauptverzeichnis des Repositories `Karlstahl1982/tessera-alarmclock` hochladen. **Keinen zusätzlichen `alarmclock/`-Unterordner anlegen.**
2. Den Upload als Commit auf dem Standardbranch `main` speichern.
3. In Tessera → **Plugins** → **Mit einem Link hinzufügen** → **Branch testen** öffnen. GitHub-Link `https://github.com/Karlstahl1982/tessera-alarmclock` und Branch `main` angeben. Alternativ kann bei einem Repository ohne Release der reine GitHub-Link unter "Release" den Standardbranch testen.
4. **Plugin lesen**. Bei erfolgreicher Prüfung dem Display `Hilde` hinzufügen und Firmware bauen/aktualisieren.
5. Im Layout eine Kachel "Nächster Wecker" platzieren, speichern und auf dem Display öffnen. Erwartet: 5 Platzhalter "Wecker 1" bis "Wecker 5".

Vor dem Firmware-Build ein Backup der funktionierenden ESPHome/Tessera-Konfiguration erstellen. Die vorhandene Lautsprecher-YAML wird von diesem Plugin **nicht** verändert.

## Änderung 0.0.3

- Behebt den C++-Buildfehler `request for member c_str` in `alarmclock.cpp`: Die Tessera-API `text()` liefert `const char*` und wird direkt an `snprintf` übergeben.
- Das bestehende Test-Plugin bleibt sonst unverändert.
- Die ESPHome-Warnungen zu GPIO19/20, `image:` und `ota` gehören nicht zu diesem Fehler und werden hier nicht verändert.
- Ein erfolgreicher Build muss anschließend noch in der Zielumgebung bestätigt werden.

## Update über GitHub

1. Alle Dateien der Version 0.0.3 nach `Karlstahl1982/tessera-alarmclock` auf Branch `main` hochladen und vorhandene Dateien überschreiben (Commit erstellen).
2. Kontrollieren, dass `tessera-plugin.yaml` im Hauptverzeichnis Version 0.0.3 enthält.
3. In Tessera über **Mit einem Link hinzufügen → Branch testen** Repository und Branch `main` erneut einlesen. Tessera muss den **neuen Commit** verwenden; im Build-Log darf nicht mehr der alte Commit `d8139fc6e689ec9ade5357477fc019b7d34f9210` erscheinen.
4. Plugin auf dem Display aktualisieren und die Firmware erneut kompilieren. Falls weitere Fehler auftreten, das neue Build-Log sichern.

## Diagnose

- Wird das Plugin nicht gefunden, zuerst prüfen, ob `tessera-plugin.yaml` im GitHub-**Hauptverzeichnis** liegt und ob der Commit im richtigen Branch gespeichert ist.
- Erkennt Tessera den Plugin-Eintrag, aber ein Firmware-Build schlägt fehl, das vollständige Buildlog sichern.
- Der Build wurde hier nicht auf echter Hardware getestet.

## Rückbau

Tessera → Display → Plugins → AlarmClock entfernen und die Firmware erneut erstellen. Bestehende Audio-Override-YAML bleibt unverändert.
