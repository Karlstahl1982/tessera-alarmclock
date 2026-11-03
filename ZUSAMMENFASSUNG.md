# Projektzusammenfassung – Tessera AlarmClock

**Version:** 0.3.3 (Beta)  
**Ziel:** Verhindern, dass der **erste planmäßige Alarm sofort als „Alarmton beendet“** erscheint. Die lokale Audioausgabe, Lautstärkestufen, Touchbedienung und das Schlummern bleiben erhalten.

## Arbeitsschritte / Status

- [x] Vollständiges Paket v0.3.2 als Basis übernommen; alle Komponenten vorhanden.
- [x] Ursache identifiziert: Beim ersten planmäßigen Alarm vermischte der Tick einen älteren Scheduler-Zeitwert mit einem neueren `millis()`-Wert. Die Subtraktion ohne Vorzeichen lief bereits bei 1 ms Differenz über und markierte den Audio-Timeout als abgelaufen.
- [x] Ein reproduzierbarer Laufzeittest schlägt auf v0.3.2 fehl und besteht nach der Korrektur.
- [x] Startzeit und Timeoutprüfung innerhalb des gesamten Intervalls auf denselben monotonen Zeitstempel umgestellt.
- [x] Zeitdifferenzen gegen leicht negative Abweichungen abgesichert; auch einen echten Zählerüberlauf geprüft.
- [x] Bestehende GUI, fünf Wecker, 60-Sekunden-Lautstärkerampe, 75-Sekunden-Testalarm und bis zu drei Schlummer-Wiederholungen unverändert gelassen.
- [x] Native C++-Mocktests, JS-Prüfungen, Paket-/YAML- und Layouttests bestanden.
- [ ] Firmware mit Tessera/ESPHome 2026.9.0 auf dem Zielsystem kompilieren.
- [ ] OTA-Update auf Guition „Hilde“ durchführen.
- [ ] **Ersten planmäßigen Alarm direkt nach einem Geräteneustart** sowie einen Alarm nach Schlummern akustisch testen.
- [ ] Im echten Betrieb Ton, 6-Sekunden-Countdown und Lautstärkestufen bis 60 Sekunden prüfen.

## Probleme und Umgehungen

1. **Erster Alarm sofort beendet:** `on_interval()` erhielt den vom Framework zuvor erfassten Tick und rief `start_alarm_()` auf, das `millis()` etwas später las. `uint32_t(now - start)` wurde bei negativem Millisekunden-Unterschied zu einem Wert von fast 49 Tagen. **Fix:** ein konsistenter Start-Zeitstempel pro Tick; negative Abweichungen werden defensiv auf null geklemmt.
2. **RTTTL/I²S-Kaltstart und Lautstärkeänderung (aus 0.3.2):** Die bestehenden begrenzten Audio-Neustarts und die direkte Lautstärkestufenregelung am I²S-Speaker bleiben erhalten; diese Mechanismen werden nicht als Lösung des Timeoutfehlers missbraucht.
3. **Hardwaretest noch offen:** Die Testumgebung bildet Tessera und ESPHome nach. Der C++-Mocktest bestätigt die Alarmzustandslogik, kann die reale Lautsprecherausgabe aber nicht messen.
4. **Keine RTC:** Nach völligem Stromausfall ist eine erneute gültige Zeitsynchronisierung erforderlich.

## Installation

1. ZIP vollständig entpacken. Alle Dateien/Unterordner direkt nach `https://github.com/Karlstahl1982/tessera-alarmclock` auf Branch `main` hochladen und bestehende Dateien ersetzen.
2. In Tessera **„Plugins → Mit einem Link hinzufügen → Branch testen“** den Repository-Link und `main` wählen. **Version 0.3.3 und neuen Commit prüfen.**
3. Vorhandene Audio-YAML und Home-Assistant-Karte unverändert lassen.
4. Zunächst **ESPHome-Firmware nur kompilieren**. Bei Erfolg Firmware auf „Hilde“ übertragen.
5. Display neu starten und einen einmaligen Wecker für die nächsten 2–3 Minuten einstellen. Beim ersten Klingeln muss Ton kommen; „Alarmton beendet“ darf nicht sofort erscheinen.
6. Testweise auf **Schlummern** gehen und die erneute Auslösung prüfen. Außerdem Tonstufen (alle 6 s) und Stoppen testen.
7. Solange die erste reguläre Weckzeit nicht in der Praxis geprüft wurde, einen unabhängigen Zweitwecker verwenden.

**Rollback:** Über den vorherigen GitHub-Commit / Plugin-Stand v0.3.2 wiederherstellen, falls der Build scheitert. Hinweis: Dieser Stand enthält den hier behobenen Erstalarm-Fehler.
