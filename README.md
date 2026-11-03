# Tessera AlarmClock v0.2.3

Lokaler Wecker für Tessera auf dem Guition ESP32-4848S040. Bis zu fünf Wecker, eigener Lautsprecher über bestehendes ESPHome-RTTTL-Setup, Steuerung direkt am Display und über eine Home-Assistant-Karte.

## Änderungen gegenüber 0.2.2

- **Grafisches Bearbeiten direkt am Touchdisplay:** Große Uhrzeit, Felder für Wiederholung, Klang, Lautstärke, Schlummern/Status. Keine erneute mehrstufige Neuanlage beim Bearbeiten. Alle Felder sind berührbar; Uhrzeit bleibt über zwei LVGL-Roller einstellbar.
- **Grafisches Bearbeiten in Home Assistant:** Zusammenhängende Bearbeitungsseite mit großer Uhrzeit, Karten für Wiederholung, Klang, Lautstärke, Schlummern und Status. Anlegen bleibt ein Wizard.
- **Blockierende Alarmansicht:** Eigenständiges, bildschirmfüllendes LVGL-Overlay auf der System-Ebene, das Navigation und Hintergrund-Taps abfängt. Die Alarmansicht verschwindet nur nach explizitem Stoppen oder Schlummern (bzw. Stoppen über die Home-Assistant-Aktion). Bei maximaler Schlummerzahl wird Schlummern ausgeblendet.
- **Maximale Alarmdauer:** Nach 5 Minuten wird der Ton stummgeschaltet. Die Bestätigungsansicht bleibt sichtbar, bis der Nutzer handelt. Der 15-Sekunden-Testalarm verhält sich entsprechend.
- **Bestehende Entitäten und Speicherung:** unverändert, keine Datenmigration erforderlich.

## Installation / Update

1. **Vor dem Update:** Aktuelle AlarmClock-Konfiguration und vorhandene Wecker prüfen; bei kritischen Weckzeiten unabhängigen Ersatzwecker verwenden.
2. Alle Dateien **aus dem Inhalt dieses ZIPs** direkt ins Repository `https://github.com/Karlstahl1982/tessera-alarmclock` auf Branch `main` hochladen. Im Repository-Hauptverzeichnis müssen `plugin.yaml` und `tessera-plugin.yaml` liegen.
3. In Tessera unter **Plugins → AlarmClock** Branch `main` aktualisieren/neu lesen, Version **0.2.3** prüfen und Firmware für **Hilde** erstellen und aufspielen. Es handelt sich um eine **Firmware-Änderung**, nicht nur um eine Home-Assistant-Karte.
4. Optional: `ha_card/tessera-alarmclock-card.js` nach `/homeassistant/www/tessera-alarmclock-card.js` kopieren. Die Lovelace-Ressource auf `/local/tessera-alarmclock-card.js?v=0.2.3` setzen und den Browser vollständig neu laden.
5. Auf dem Display eine vorhandene Weckzeit antippen, grafisch bearbeiten und speichern. Danach **Testalarm** auslösen. Prüfen, dass Hintergrund-Taps und Zurück-Gesten die Alarmansicht nicht schließen und dass **Stoppen** zuverlässig funktioniert.

## Architektur

Die ESP32-Komponente `components/alarmclock` verwaltet die Wecker und veröffentlicht native ESPHome-Entitäten. Der Display-Dialog ist über Tessera als `overview` registriert. Für aktiven Alarm wird eine LVGL-System-Ebene (`lv_layer_sys`) erstellt; sie fängt die Touch-Ereignisse ab. Der Wecker arbeitet bei bekannter Uhrzeit unabhängig von WLAN und Home Assistant. Ohne RTC kann er nach vollständigem Neustart erst mit gültiger Uhrzeit zuverlässig auslösen.

## Prüfen

Im entpackten Projektordner:

- `python -m unittest discover -s tests -v` (benötigt PyYAML, Node.js und für C++-Mock-Test g++)
- `node --check ha_card/tessera-alarmclock-card.js`
- `node tests/test_frontend.js`

**Grenzen:** Mock-/Syntaxprüfungen ersetzen keinen echten ESPHome-Build. Das LVGL-System-Overlay und seine Touch-Abschirmung müssen auf deinem Guition mit Tessera `screens-v0.4.96` oder neuer getestet werden. Bildschirmabschaltung/Standby während eines Alarms ist noch nicht separat geprüft. Bei Stromausfall besteht ohne RTC/Akku keine Weckgarantie.
