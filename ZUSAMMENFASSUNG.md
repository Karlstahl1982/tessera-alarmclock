# Projektzusammenfassung – Tessera AlarmClock 0.2.3

## Ziel

Fünf lokal betriebene Wecker für Tessera auf Guition ESP32-4848S040 mit intuitiver Touchbedienung, Home-Assistant-Anbindung, akustischem Alarm und bewusstem Quittieren.

## Arbeitsschritte

- [x] ESP32 als führende Datenquelle; fünf Slots, Wiederholungen und lokale Speicherung aus dem vorhandenen Projekt übernehmen.
- [x] Neue grafische Bearbeitungsübersicht auf dem Guition-Display (Zeit, Wiederholung, Klang, Optionen, Aktivierung, Löschen).
- [x] Grafischen Bearbeitungsbildschirm in der Home-Assistant-Karte ergänzen; neue Wecker weiterhin per Wizard.
- [x] Bildschirmfüllendes LVGL-Overlay für Alarme; Hintergrund-Taps werden nicht als Schließen verarbeitet.
- [x] Nach fünf Minuten akustisch stummschalten, Anzeige bis zu Stoppen/Schlummern offen halten; Testalarm nach 15 Sekunden stummschalten.
- [x] Bestehende Datenstruktur, Entitäten und Lautsprecheransteuerung beibehalten.
- [x] Automatisierte JS-/Python- und C++-Mock-Checks ergänzen.
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
2. Plugin in Tessera neu laden, Version 0.2.3 auswählen, Firmware für **Hilde** erzeugen und installieren.
3. Datei `ha_card/tessera-alarmclock-card.js` nach `/homeassistant/www/` kopieren, Ressource `/local/tessera-alarmclock-card.js?v=0.2.3` setzen und Browser hart neu laden.
4. Wecker bearbeiten, Probealarm durchführen, Schließen per Hintergrund-Tap versuchen und dann explizit **Stoppen** drücken.
5. Während des Praxistests einen unabhängigen Ersatzwecker einsetzen.

## Teststatus

Automatisierte Prüfungen im ZIP unter `tests/`. Ein tatsächlicher ESPHome-Build auf der Zielhardware ist noch offen; daher Beta/Testversion und **keine** Zusage eines für wichtige Termine ausfallsicheren Weckers.
