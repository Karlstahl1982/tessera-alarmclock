# Tessera AlarmClock 0.0.2 – Projektzusammenfassung

## Ziel
Ein bewusst kleines, eigenständiges Wecker-Plugin für Tessera auf Guition ESP32-4848S040 ("Hilde"). Zielarchitektur: fünf Wecker, lokale Auslösung im ESP32, Touchbedienung und Home-Assistant-Synchronisierung; weiterlaufender Alarm ohne HA/WLAN, solange Strom und gültige Uhrzeit vorhanden sind.

## Arbeitsschritte
- [x] Umfang auf kleinen MVP reduziert und offene Komfortfunktionen zurückgestellt.
- [x] Das GitHub-Repository `Karlstahl1982/tessera-alarmclock` als Ziel bestätigt (zum Prüfzeitpunkt leer).
- [x] Installationsformat auf das offizielle GitHub-Link-Verfahren angepasst: Plugin im Repository-Hauptverzeichnis.
- [x] Maintainer und Version 0.0.2 aktualisiert; README und Changelog angepasst.
- [x] YAML, JSON und Python statisch geprüft; ZIP-Inhalt und Pfade geprüft.
- [ ] Dateien auf GitHub in Branch `main` hochladen.
- [ ] Tessera: "Mit einem Link hinzufügen" / "Branch testen" / "Plugin lesen" prüfen.
- [ ] Firmware auf Display Hilde erfolgreich bauen und Kachel testen.
- [ ] Fünf Wecker lokal speichern und schedulen (einmalig/wöchentlich).
- [ ] Stoppen/Schlummern und Ton über NS4168 integrieren.
- [ ] ESPHome-Entitäten und bidirektionale HA-Bedienung einbauen.
- [ ] Offline-Zugriff auf Vollbild-Alarm trotz Tessera-Sperrbildschirm nachweislich umsetzen.
- [ ] Später: Melodien, 60-s-Lautstärkeanstieg, Alarmhelligkeit, Nachholregel.

## Probleme und Umgehungen
- Falsche frühere Pfadannahme: `/config/tessera-plugins/` ist für lokale Plugin-Tests zwar dokumentiert, aber es wird nicht automatisch ein solcher Ordner angelegt. Wir verwenden stattdessen GitHub-Installation.
- V0.0.1 enthielt einen `alarmclock/`-Unterordner. Für das neue separate GitHub-Repository liegt die Plugin-Wurzel direkt auf Repository-Ebene.
- Lautsprecherfunktion auf dieser Guition-Revision wurde bereits per SMD-Brückenumbau und ESPHome-YAML ermöglicht. Audio bleibt bewusst unangetastet.
- Plugin-Build, Touch-Bedienung, Audio-API und Verhalten ohne HA sind noch nicht hardwareverifiziert. Statische Prüfungen ersetzen den Test nicht.
- Trotz späterem Ziel "zuverlässig offline" ist derzeit **kein** nutzbarer Wecker implementiert. Nicht für echte Weckzeiten einsetzen.

## Installationsanweisungen (nur aktueller Arbeitsschritt)
1. ZIP `Tessera_AlarmClock_0.0.2.zip` entpacken.
2. Alle entpackten Dateien und Ordner direkt auf die oberste Ebene von `https://github.com/Karlstahl1982/tessera-alarmclock` hochladen; Commit auf `main`.
3. Prüfen, ob `tessera-plugin.yaml` und `plugin.yaml` neben `README.md` sichtbar sind (nicht in einem zusätzlichen Ordner).
4. **Hier anhalten und Bestätigung geben.** Der Tessera-Installationsschritt folgt erst danach.

Später: Tessera > Plugins > Mit einem Link hinzufügen > Branch testen > GitHub URL + `main` > Plugin lesen. Vor Firmware-Build bestehendes Display-Backup erstellen.

## Verwendete Dokumentation
- https://github.com/MaxGramser/tessera-plugins/blob/main/docs/MAKING_A_PLUGIN.md
- https://github.com/MaxGramser/tessera-plugins/blob/main/docs/MANIFEST.md
- https://github.com/MaxGramser/tessera-plugins/blob/main/docs/TESTING.md
- https://github.com/MaxGramser/tessera-plugins/blob/main/docs/FIRMWARE_API.md
