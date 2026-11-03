# Home-Assistant-Karte – Tessera AlarmClock 0.3.0

Die im Paket enthaltene `tessera-alarmclock-card.js` entspricht weiterhin dem Stand aus v0.2.3.

**Für das reine Upgrade von v0.2.4 auf v0.3.0 ist kein Austausch dieser Datei erforderlich.** Die Änderungen der v0.3.0 betreffen ausschließlich die Touch-Oberfläche der Tessera-Firmware.

## Neuinstallation der HA-Karte (falls noch nicht vorhanden)

1. `tessera-alarmclock-card.js` nach `/homeassistant/www/tessera-alarmclock-card.js` kopieren.
2. Unter Einstellungen → Dashboards → Ressourcen als JavaScript-Modul registrieren: `/local/tessera-alarmclock-card.js?v=0.2.3`.
3. Browser mit Strg+F5 neu laden.
4. Karte mithilfe von `LOVELACE_KARTE.yaml` in ein Dashboard einfügen.

Die Home-Assistant-Karte ist von der Display-Firmware getrennt. Der ESP32 speichert und verwaltet die Weckzeiten.
