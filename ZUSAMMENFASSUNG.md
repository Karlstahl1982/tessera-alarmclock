# Projektzusammenfassung – Tessera AlarmClock 0.3.4

## Ziel

Ein schlanker, am ESP32 autonom laufender Wecker mit maximal fünf Einträgen, Touchbedienung, sauberer Schlummerlogik und klarer grafischer Lautstärkeanzeige. Home Assistant soll nur noch zentrale Funktionen anzeigen und steuern.

## Arbeitsschritte

- [x] Bestehende v0.3.3 als Grundlage übernommen.
- [x] Fortschrittsbalkenlayout korrigiert und 60-Sekunden-Füllstand synchronisiert.
- [x] Audio-Lautstärkeverlauf sekündlich statt in 6-Sekunden-Stufen angepasst.
- [x] Verbleibende Zeit bis zur Ziellautstärke und aktuelle Software-Prozentzahl im Alarmfenster.
- [x] Schlummernde einmalige Wecker weiterhin im 2×3-Raster und auf der Hauptkachel sichtbar.
- [x] Zentrale HA-Entitäten eingerichtet: nächster Wecker, Unterdrückung, Stoppen.
- [x] Per-Wecker-Entitäten intern geschaltet und Home-Assistant-Karte reduziert.
- [x] Automatisierte Mock-, Struktur- und Frontendtests ausgeführt (24 erfolgreich).
- [ ] Echter ESPHome-Build und Flash auf das Guition-Display.
- [ ] Hardwareseitige Funktionsprüfung der neuen Balken- und Lautstärkedarstellung.
- [ ] HA-Entitätenregistrierung und zentrale Bedienung prüfen.

## Probleme und umgangene Fehler

1. **Balken ohne Fortschritt:** Ein verschachteltes LVGL-Objekt konnte durch Container-Style/Padding verdeckt bzw. falsch platziert sein. Abhilfe: zwei unabhängige Rechtecke, definierte Positionen und Null-Padding. Ob das den Fehler vollständig behebt, muss die Hardwareprüfung zeigen.
2. **Akustische Sprünge:** Gain wurde nur in 6-Sekunden-Schritten gesetzt. Jetzt lineare Interpolation und Audio-Update nach jeweils einer Sekunde, während die Anzeige über die volle Minute läuft.
3. **Verschwundener Schlummerwecker:** Einmalige Wecker wurden beim ersten Auslösen deaktiviert und deshalb in der „Nächster Wecker“-Auswahl ignoriert. Nun hat ein anstehender Schlummeralarm Vorrang, unabhängig vom Aktivierungsstatus.
4. **Zu viele HA-Entitäten:** Mit `internal: true` werden die Einzel-Wecker-Entitäten nicht mehr veröffentlicht. Achtung: Home Assistant kann alte Einträge in der Registry behalten; diese werden dort unter Umständen als nicht verfügbar angezeigt und müssen bei Bedarf manuell entfernt werden.
5. **Veraltete HA-Karte:** Die alte Karte kann nach Internalisierung der Wecker nicht mehr funktionieren. Die beigelegte neue Karte verwendet ausschließlich die drei zentralen Entitäten.

## Installation

1. ZIP entpacken und seinen **Inhalt** direkt in das Stammverzeichnis des GitHub-Repositories `Karlstahl1982/tessera-alarmclock` übertragen; auf `main` committen.
2. Tessera → Plugins → Mit Link hinzufügen → Branch testen; Version 0.3.4 und Commit prüfen.
3. Firmware zuerst nur kompilieren. Bei erfolgreichem Build per OTA auf „Hilde“ übertragen.
4. Testalarm starten: Ton, 60-Sekunden-Lautstärkeverlauf, Balken und Countdown prüfen.
5. Einen Wecker schlummern lassen: er muss weiterhin im Raster und in der Hauptkachel auftauchen.
6. `ha_card/tessera-alarmclock-card.js` nach `/homeassistant/www/` kopieren; HA-Ressource auf `/local/tessera-alarmclock-card.js?v=0.3.4` ändern und Browser neu laden.
7. Zentralen Unterdrückungsschalter testen; zum Schluss unbedingt wieder AUS schalten.

**Grenze:** Ein echter Firmware-Build und Hardwaretest waren in dieser Umgebung nicht verfügbar. Bis zur erfolgreichen Praxisprüfung einen unabhängigen Backup-Wecker verwenden.
