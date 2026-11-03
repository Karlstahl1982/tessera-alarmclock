# Projektzusammenfassung – Tessera AlarmClock v0.3.6

## Ziel
Bis zu fünf lokal gesteuerte Wecker auf Guition ESP32-4848S040 / Tessera, mit akustischem Alarm, Schlummern, Touchbedienung und wenigen zentralen HA-Entitäten. Lautstärke beginnt bei 5 % und steigt stufenweise alle 15 Sekunden um 5 Prozentpunkte.

## Arbeitsschritte
- [x] Vollständigen Stand v0.3.5 inklusive sämtlicher Dateien übernommen.
- [x] Feste Lautstärkekurve 5 %, +5 Prozentpunkte/15 Sekunden implementiert.
- [x] Die konfigurierte Maximal-Lautstärke als Obergrenze beibehalten.
- [x] I²S-Updates auf Stufenwechsel bzw. Audiostart reduziert.
- [x] Fortschrittsbalken und Countdown auf die neue 15-Sekunden-Staffel angepasst.
- [x] Regressionstests und Dokumentation aktualisiert.
- [ ] Firmware mit ESPHome 2026.9.0 / Tessera screens-v0.4.96 tatsächlich bauen.
- [ ] Per OTA auf Hilde installieren und den Lautsprecher bei 5 %, 10 %, 15 % testen.
- [ ] 5-Minuten-Alarm und Übergang in den Schlummermodus auf dem Gerät prüfen.

## Probleme / umgangene Fehler
- Die vorherige Rampe startete bei 50 % der eingestellten Maximallautstärke. Bei 100 % war sie damit sofort bei 50–55 %. Jetzt startet sie immer bei 5 %.
- Vorher wurde die Lautstärke jede Sekunde interpoliert. Jetzt erfolgt ein exakter digitaler Stufenwechsel nur alle 15 Sekunden; der Balken zeigt dieselben Schritte.
- `set_gain()` alleine aktualisiert die Lautstärke nicht während eines laufenden I²S-Audiostreams. Deshalb bleibt `speaker_->set_volume()` am Stufenwechsel erhalten.
- Für individuell begrenzte Wecker (z. B. 60 %) endet die Rampe früher, statt trotz Zielwert weiter anzusteigen.
- 5 % digitaler Pegel kann trotz korrekter Technik sehr leise oder unhörbar sein. Die Prozentwerte sind nicht linear zur wahrgenommenen Lautstärke und keine dB-Angaben.
- Firmware-Kompilierung und Hardwaretest können hier nicht durchgeführt werden; C++-Mock-Tests ersetzen diese nicht.

## Installation
1. Archiv entpacken, alle Dateien in das Stammverzeichnis von `Karlstahl1982/tessera-alarmclock` hochladen und auf `main` committen.
2. In Tessera Plugin über `Branch testen` neu laden und Version 0.3.6/Commit prüfen.
3. ESPHome-Build durchführen und **nur bei Erfolg** auf Hilde übertragen.
4. Testalarm starten; zunächst die Anzeige bei 0/15/30 Sekunden überprüfen und tatsächliche Hörbarkeit prüfen. Nach dem Stoppen prüfen, dass keine Rest-Alarmsequenz aktiv bleibt.
5. Home-Assistant-Entitäten und Dashboard-Ressource müssen nicht angepasst werden.
