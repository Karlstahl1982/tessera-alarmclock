# Projektzusammenfassung – Tessera AlarmClock 0.2.4

## Ziel

Fünf lokal betriebene Wecker für Tessera auf Guition ESP32-4848S040 mit intuitiver Touchbedienung, Home-Assistant-Anbindung, akustischem Alarm und bewusstem Quittieren.

## Behebung in Version 0.2.4

**Auslöser:** Der reale ESPHome-Build auf Hilde (ESPHome 2026.9.0, Tessera `screens-v0.4.96`) bricht bei `lv_button_create()` ab. In dieser Buildkonfiguration ist das optionale LVGL-Button-Widget nicht verfügbar. Der bisherige C++-Mock enthielt diese Funktion fälschlich und verdeckte das Problem.

**Korrektur:** Die beiden Alarm-Aktionen werden jetzt mit `lv_obj_create()` aufgebaut, als klickbar markiert und mit denselben Stoppen-/Schlummern-Callbacks verbunden. Der Mock wurde angepasst. Keine weiteren Funktionsänderungen.

## Arbeitsschritte

- [x] ESP32 als führende Datenquelle; fünf Slots, Wiederholungen und lokale Speicherung aus dem vorhandenen Projekt übernehmen.
- [x] Neue grafische Bearbeitungsübersicht auf dem Guition-Display (Zeit, Wiederholung, Klang, Optionen, Aktivierung, Löschen).
- [x] Grafischen Bearbeitungsbildschirm in der Home-Assistant-Karte ergänzen; neue Wecker weiterhin per Wizard.
- [x] Bildschirmfüllendes LVGL-Overlay für Alarme; Hintergrund-Taps werden nicht als Schließen verarbeitet.
- [x] Nach fünf Minuten akustisch stummschalten, Anzeige bis zu Stoppen/Schlummern offen halten; Testalarm nach 15 Sekunden stummschalten.
- [x] Bestehende Datenstruktur, Entitäten und Lautsprecheransteuerung beibehalten.
- [x] Automatisierte JS-/Python- und C++-Mock-Checks ergänzen.
- [x] Compilerfehler bei LVGL-Button-Erstellung korrigieren und Regressionstest ergänzen.
- [ ] Echten ESPHome-Firmware-Build in Tessera und Update auf Display Hilde durchführen.
- [ ] Touch-Blocking, Stopp-/Schlummer-Tasten und Layout am echten 4-Zoll-Display verifizieren.
- [ ] Standby/Backlight-Verhalten sowie mehrere gleichzeitig klingelnde Alarme in der Praxis prüfen.

## Probleme und umgangene Schwierigkeiten

- Tessera-Popups können sich bei Hintergrund-Taps schließen. Umgangen durch eine eigene Ebene über `lv_layer_sys`, die das Display vollständig bedeckt und Touch-Ereignisse selbst annimmt. Dies muss mit der realen Tessera-Firmware geprüft werden.
- Bei automatischer Beendigung nach fünf Minuten verschwand die Alarmoberfläche auch ohne Nutzeraktion. Tonende und Bestätigung sind jetzt getrennt.
- Nachträgliches Bearbeiten führte durch den mehrstufigen Neuanlage-Wizard. Bestehende Einträge öffnen jetzt direkt eine grafische Bearbeitungsübersicht.
- Ohne RTC ist ein Wecker nach vollständigem Stromverlust bis zur neuen Zeitsynchronisation nicht zuverlässig; bewusst unverändert.

## Installation

1. ZIP entpacken, Dateien direkt ins Root-Verzeichnis des GitHub-Repositories `Karlstahl1982/tessera-alarmclock` hochladen und auf Branch `main` committen.
2. Plugin in Tessera neu laden, Version 0.2.4 auswählen, Firmware für **Hilde** erzeugen und installieren.
3. **Keine Änderung an Home Assistant erforderlich.** Die mitgelieferte Karte ist inhaltlich identisch mit v0.2.3. Bestehende Ressource und Browser-Konfiguration unverändert lassen.
4. Nach erfolgreichem Build/Flash einen Testalarm durchführen. Prüfen, dass Hintergrund-Taps den Dialog nicht schließen und **Stoppen** sowie **Schlummern** reagieren.
5. Während des Praxistests einen unabhängigen Ersatzwecker einsetzen.

## Teststatus

Automatisierte Prüfungen im ZIP unter `tests/`. Der Build von v0.2.3 ist an `lv_button_create()` gescheitert; der korrigierte Build von v0.2.4 steht noch aus; daher Beta/Testversion und **keine** Zusage eines für wichtige Termine ausfallsicheren Weckers.
