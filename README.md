# Tessera AlarmClock v0.3.3 (Beta)

Eigenständiges Wecker-Plugin für das Guition ESP32-4848S040 mit Tessera und Home Assistant. Komplettpaket für GitHub, **kein einzelner Firmware-Binärdownload**.

## Neu in Version 0.3.3

**Bugfix für den allerersten planmäßigen Alarm:** Bisher konnte bereits eine Millisekunde Unterschied zwischen dem vom Framework erfassten Tick und dem Alarm-Startwert dazu führen, dass der Alarm unmittelbar als „Alarmton beendet“ markiert wurde. Jetzt verwendet die gesamte Intervallprüfung einen konsistenten Zeitstempel und eine robuste Differenzberechnung.

Der seit v0.3.2 vorhandene Audiostart mit begrenzten Retries und die 60-Sekunden-Lautstärkerampe bleiben unverändert. Bei eingestellten 60 % steigt die Software-Anzeige von 30 % in zehn Stufen auf 60 %. Die Prozentwerte sind Sollwerte, keine gemessenen dB.

## Voraussetzungen

- Guition ESP32-4848S040 mit aktivierter Audiobrücke R21/R22/R23 und angeschlossenem Lautsprecher.
- Tessera mit ESPHome 2026.9.0 und bereits funktionierendem Audio-Override.
- Lokale Tessera-YAML muss unverändert `guition_tone` (RTTTL) **und** `guition_speaker` (I²S Speaker) bereitstellen. Neu ist, dass das Plugin beide referenziert.
- Funktionierende Netzwerkverbindung für Firmware-Erstellung/OTA.

## Installation

1. Dieses Archiv vollständig entpacken.
2. Alle Dateien und Unterordner **direkt** in das Stammverzeichnis von `https://github.com/Karlstahl1982/tessera-alarmclock` übernehmen; vorhandene Dateien ersetzen. Auf `main` committen.
3. In Tessera „Plugins → Mit einem Link hinzufügen → Branch testen“ öffnen; Repository-Link und Branch `main` verwenden. Neue Version **0.3.3** und neuen Commit kontrollieren.
4. **Zuerst nur ESPHome-Firmware kompilieren.** Wenn der Build erfolgreich ist, auf das Display „Hilde“ übertragen.
5. Vor dem Toncheck einen moderaten Zielwert (z. B. 40 %) wählen. Testalarm starten und die ersten Sekunden prüfen. Der Test läuft **75 Sekunden**, kann aber jederzeit mit „Stoppen“ beendet werden.
6. Prüfen, dass „Nächste Erhöhung in 6 s“ herunterzählt, die Prozentzahl in 6-Sekunden-Schritten ansteigt und die Tonlautstärke tatsächlich hörbar zunimmt.
7. Für den abschließenden Alltagstest einen einmaligen Wecker für die nächsten Minuten anlegen. **Bis zu diesem Test nicht als alleinigen Wecker verlassen.**

## Unverändert

Fünf gespeicherte Wecker, grafische Kacheln und Drehräder, Home-Assistant-Karte, Wiederholungen, Stoppen/Schlummern und blockierende Alarmansicht. Die Dateien unter `ha_card/` ändern sich gegenüber v0.3.1 nicht; **keine erneute HA-Ressourcenregistrierung** nötig.

## Einschränkungen

- Kein echter ESPHome-/ESP-IDF-Build in dieser Umgebung; automatisierte Tests nutzen nachgebildete Schnittstellen.
- Hardwarebedingte Ausfälle (Stecker, Verstärker, falsche GPIO-Brücken) werden durch Software-Retries nicht beseitigt.
- Kein RTC-Modul; nach vollständigem Neustart ohne Zeitquelle ist die Uhrzeit nicht zuverlässig.

