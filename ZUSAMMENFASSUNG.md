# Tessera AlarmClock 0.0.3 – Projektzusammenfassung

## Ziel
Schlankes Tessera-Wecker-Plugin für das Guition ESP32-4848S040 (Display „Hilde“): später fünf lokal gespeicherte Wecker, Touchbedienung, Home-Assistant-Entitäten und offlinefähige Alarmauslösung. **Aktuell ist nur ein Integrationstest mit einer Dashboard-Kachel und fünf Platzhaltern enthalten, kein einsatzfähiger Wecker.**

## Arbeitsschritte
- [x] MVP auf kleinen Installationsprototypen reduziert.
- [x] GitHub-Repository `https://github.com/Karlstahl1982/tessera-alarmclock` erstellt und Dateien hochgeladen (laut Nutzer).
- [x] Tessera liest das Plugin und zeigt die Plugin-Karte an (laut Nutzer).
- [x] Fehler aus dem echten ESPHome-Buildlog zugeordnet: `plugin_->text("alarm")` ist `const char*`; `.c_str()` war ungültig.
- [x] Fehler in `components/alarmclock/alarmclock.cpp` behoben, Manifestversion auf 0.0.3 angehoben.
- [x] YAML/JSON/Python statisch geprüft und ZIP-Struktur kontrolliert.
- [ ] Dateien der Version 0.0.3 als neuen GitHub-Commit hochladen.
- [ ] In Tessera Branch `main` erneut lesen und sicherstellen, dass der neue Commit verwendet wird.
- [ ] Firmware erfolgreich kompilieren und Platzhalterkachel auf dem Display testen.
- [ ] In späteren Versionen: Speicherung, lokale Zeitsteuerung, Ton, Snooze und HA-Steuerung implementieren.
- [ ] Offline-Bedienung ohne WLAN/HA tatsächlich testen (Tessera-Verbindungszustand berücksichtigen).

## Probleme und Umgehungen
- Ursprünglich falscher lokaler Pfad: Eine direkte Ordnerinstallation unter `/config/tessera-plugins/` war hier nicht der geeignete Weg. Behoben durch GitHub-Branch-Installation.
- 0.0.2 kompiliert nicht: `request for member 'c_str' ... non-class type 'const char*'` in `AlarmOverview::open()`. In 0.0.3 durch Entfernen von `.c_str()` behoben.
- Im Buildlog genannte Warnungen (USB GPIO19/20, deprecated `image:`, OTA-Password) sind **nicht** die Ursache dieses Abbruchs und wurden bewusst nicht verändert.
- **Offen:** Kein vollständiger Firmware-Build im hiesigen Container möglich; die Korrektur muss auf dem echten ESPHome-System bestätigt werden. Weitere Buildfehler sind nicht ausgeschlossen.
- Tessera referenziert im alten Log Commit `d8139fc6e689ec9ade5357477fc019b7d34f9210`. Nach dem GitHub-Update muss Tessera den neuen Commit einlesen, sonst baut es weiter 0.0.2.

## Installation / Update (aktueller Schritt)
1. `Tessera_AlarmClock_0.0.3.zip` entpacken.
2. Die **vollständigen Inhalte** direkt im Hauptverzeichnis des GitHub-Repositories `Karlstahl1982/tessera-alarmclock` auf Branch `main` hochladen und bestehende Dateien überschreiben; Commit speichern.
3. Prüfen, ob `tessera-plugin.yaml` die Version `0.0.3` anzeigt und `components/alarmclock/alarmclock.cpp` im richtigen Unterordner liegt.
4. **Hier anhalten und Bestätigung geben.** Erst anschließend Tessera-Branch erneut einlesen, Plugin aktualisieren und neu kompilieren.

## Betriebshinweis
Die Lautsprecher-YAML des Displays wird durch dieses Paket nicht verändert. Die 0.0.3 ist weiterhin **nicht zum zuverlässigen Wecken geeignet**.
