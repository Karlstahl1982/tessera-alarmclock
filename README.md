# Tessera AlarmClock 0.0.2

Kompakter **Integrationstest** für das Guition ESP32-4848S040 (Display **Hilde**) unter Tessera.

**Noch kein funktionsfähiger Wecker!** Die Kachel "Nächster Wecker" öffnet eine Ansicht mit fünf leeren Weckerplätzen. Es erfolgt weder Alarmauslösung noch Speicherung oder Home-Assistant-Synchronisierung. Nicht als zuverlässigen Wecker verwenden.

## Installation über GitHub

1. Dieses ZIP **entpacken**. Alle Dateien und Ordner aus dem entpackten Verzeichnis (insbesondere `tessera-plugin.yaml`, `plugin.yaml`, `components/` und `translations/`) direkt in das Hauptverzeichnis des Repositories `Karlstahl1982/tessera-alarmclock` hochladen. **Keinen zusätzlichen `alarmclock/`-Unterordner anlegen.**
2. Den Upload als Commit auf dem Standardbranch `main` speichern.
3. In Tessera → **Plugins** → **Mit einem Link hinzufügen** → **Branch testen** öffnen. GitHub-Link `https://github.com/Karlstahl1982/tessera-alarmclock` und Branch `main` angeben. Alternativ kann bei einem Repository ohne Release der reine GitHub-Link unter "Release" den Standardbranch testen.
4. **Plugin lesen**. Bei erfolgreicher Prüfung dem Display `Hilde` hinzufügen und Firmware bauen/aktualisieren.
5. Im Layout eine Kachel "Nächster Wecker" platzieren, speichern und auf dem Display öffnen. Erwartet: 5 Platzhalter "Wecker 1" bis "Wecker 5".

Vor dem Firmware-Build ein Backup der funktionierenden ESPHome/Tessera-Konfiguration erstellen. Die vorhandene Lautsprecher-YAML wird von diesem Plugin **nicht** verändert.

## Diagnose

- Wird das Plugin nicht gefunden, zuerst prüfen, ob `tessera-plugin.yaml` im GitHub-**Hauptverzeichnis** liegt und ob der Commit im richtigen Branch gespeichert ist.
- Erkennt Tessera den Plugin-Eintrag, aber ein Firmware-Build schlägt fehl, das vollständige Buildlog sichern.
- Der Build wurde hier nicht auf echter Hardware getestet.

## Rückbau

Tessera → Display → Plugins → AlarmClock entfernen und die Firmware erneut erstellen. Bestehende Audio-Override-YAML bleibt unverändert.
