# Tessera AlarmClock – Projektstatus v0.0.4

## Ziel
Ein kompakter, offline nach Uhrzeitsynchronisierung funktionierender 5-Wecker für Tessera auf Guition ESP32-4848S040 (Hilde), mit Touchbedienung, ESPHome-Entitäten für Home Assistant und vorhandener RTTTL-Audiohardware.

## Arbeitsschritte (aktueller Stand)
- [x] GitHub-Repository vorhanden und Tessera erkennt die Plugin-Metadaten.
- [x] v0.0.3 erfolgreich auf der Zielhardware kompiliert (Nutzerbestätigung).
- [x] v0.0.3: Kachel öffnet fünf Platzhalter auf dem Display (Nutzerbestätigung).
- [x] v0.0.4: Erste Einmal-Weckzeit (Stunde/Minute), Aktivierung, lokale Prüfung und Tonausgabe implementiert.
- [x] v0.0.4: ESPHome-Entitäten (Stunde/Minute/Schalter) und Buttons (Test/Stop) eingebaut.
- [x] v0.0.4: Softwarestrukturen statisch überprüft; ZIP-Inhalt vollständig.
- [ ] Version 0.0.4 im GitHub-Repository auf `main` aktualisieren.
- [ ] v0.0.4 in Tessera neu einlesen, Firmware kompilieren/installieren – **noch nicht getestet**.
- [ ] 15-Sekunden-Testalarm und einmalige Weckzeit am echten Display prüfen.
- [ ] Plätze 2–5 als reale Wecker ergänzen.
- [ ] Einstellen über die eigene Weckerseite, wiederkehrende Wochentage, Snooze, Melodien, 60s-Lautstärkerampe, Displayhelligkeit, lokaler Alarmbildschirm ergänzen.
- [ ] Offline-Verhalten bei unterbrochenem WLAN/HA und 5-Minuten-Nachholen nach Neustart verifizieren/implementieren.

## Probleme und Umgehungen
- v0.0.2 führte beim Kompilieren `.c_str()` auf `const char*` aus; behoben in 0.0.3.
- Ursprüngliche lokale `/config/tessera-plugins/`-Installation war falsch; Installationsweg ist GitHub-Branch.
- Nachgerüstete Lautsprecher-Hardware verwendet die GPIO-Umstellung R21–R23; die Audio-YAML ist separat und darf nicht überschrieben werden.
- v0.0.4 nutzt absichtlich die bereits funktionierende RTTTL-ID `guition_tone`. Ist sie nicht vorhanden, schlägt die Konfigurationsprüfung mit einer klaren unbekannten-ID-Fehlermeldung fehl.
- Der vollständige ESPHome-Build kann hier mangels installierter Ziel-Firmware-Abhängigkeiten nicht ausgeführt werden. **Kompilierbarkeit und Laufzeit müssen auf dem Zielgerät bestätigt werden.**
- RTTTL-Audio darf nicht gleichzeitig von mehreren Funktionen verwendet werden. Die Weckerfunktion hat noch keine Ressourcenkoordination mit anderen Plugins.
- Nur ein Einmalwecker ist echt; nach Auslösen wird der Aktivierungsschalter ausgeschaltet. Kein Snooze/Offline-Neustart-Recovery. Nicht als alleinige Wecklösung verwenden.

## Installation
1. Firmware/Override sichern; Audio-Override mit `guition_tone` bestehen lassen.
2. `Tessera_AlarmClock_0.0.4.zip` entpacken und **alle enthaltenen Dateien/Ordner** in die Wurzel des Repository `Karlstahl1982/tessera-alarmclock` hochladen; Commit auf `main` erstellen.
3. In Tessera: Plugins → Mit einem Link hinzufügen → Branch testen → Repository-URL und Branch `main` → Plugin lesen.
4. Neue Version 0.0.4 dem Display Hilde zuordnen, Firmware aktualisieren.
5. Home Assistant → ESPHome-Gerät Hilde → `AlarmClock Testalarm` drücken; Ton 15 Sekunden, mit Stoppen beenden.
6. Einmalwecker über Stunde/Minute/Schalter für die nächste kommende Uhrzeit aktivieren, Audio und automatische Deaktivierung prüfen.
7. Bei Fehlern letzte funktionsfähige v0.0.3 wiederherstellen und vollständiges Log weitergeben.

## Versionshinweise
- v0.0.4 ist eine **Entwicklungs- und Testversion**, keine zuverlässige Alarmuhr.
- Keine Hardwareänderungen. Kein Eingriff in bestehende Tessera-Tiles oder Audio-Override.
