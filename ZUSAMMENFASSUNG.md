# Projektzusammenfassung – Tessera AlarmClock 0.2.0

## Ziel

Ein intuitiv bedienbarer eigenständiger 5-Wecker für das Guition ESP32-4848S040 mit Tessera, Touch-Drehrad und Home-Assistant-Karte, wobei der ESP32 die führende Instanz bleibt. Der Funktionsumfang soll schlank bleiben.

## Arbeitsschritte / Status

- [x] Ausgangspunkt 0.1.0 übernehmen (Lautsprecher und bisherige Weckerlogik behalten).
- [x] Breite 2×3-Kachel für das gesamte 4-Zoll-Layout konfigurieren.
- [x] Touch-Weckerliste auf angelegte Wecker reduzieren; Hinzufügen/Bearbeiten/Löschen einbauen.
- [x] Touch-Wizard mit Stunden-/Minutendrehrädern und Wochen-/Datumseingabe implementieren.
- [x] Namen je Wecker über Home Assistant; Namensvorlagen am Touchdisplay.
- [x] Lokale Belegungsmaske persistent speichern, bestehenden Stand heuristisch übernehmen.
- [x] Native ESPHome-Entitäten als Konfiguration gruppieren.
- [x] Eigenständige Home-Assistant-Lovelace-Karte ohne weitere Bibliothek mit Wizard, Aktivierung und Test/Stopp integrieren.
- [x] YAML/JSON/JS-Syntax und strukturelle Tests durchführen.
- [ ] ESPHome-2026.9.0-Firmware **auf der realen Tessera-Installation kompilieren**.
- [ ] Touch-Navigation, Kachel-2×3 und Speicherverhalten auf Hilde praktisch testen.
- [ ] In Home Assistant Ressourcen-Karte einbinden und Ende-zu-Ende-Bedienung testen.
- [ ] Nach Reboot und WLAN-/HA-Ausfall Weckfunktion prüfen.
- [ ] Echtes randloses Alarm-Vollbild / Alarmhelligkeit: technisch gesondert entwickeln; in dieser Version nicht enthalten.

## Probleme / Workarounds

1. **Viele HA-Entitäten:** Die ESPHome-Entities werden für Synchronisierung benötigt. Wir verschieben sie in die Kategorie „Konfiguration“ und stellen eine eigene bedienbare Karte bereit; sie werden nicht deaktiviert, damit die Karte weiter funktioniert.
2. **Zu viele feste Weckerplätze:** Neue Flash-Maske verwaltet, welche Plätze wirklich angelegt sind. Bereits aktivierte bzw. abweichend konfigurierte Wecker aus 0.1.0 werden beim ersten Start erkannt; unberührte Standardplätze bleiben ausgeblendet. Bei gleichzeitig deaktivierten und vollständig standardkonfigurierten Weckern ist die frühere Belegung nicht erkennbar.
3. **Freie Texteingabe am 4-Zoll-Touch:** Um eine große Bildschirmtastatur zu vermeiden, stehen am Touchdisplay Namensvorlagen bereit. Freie Namen werden über die HA-Karte gesetzt und lokal gespeichert.
4. **Tessera-Fullscreen:** Eine 2×3-Kachel füllt die Standardseite. Der akustische Alarm erscheint weiterhin im breiten Popup, da die dokumentierte Plugin-API kein beliebiges randloses Alarm-Fenster garantiert.
5. **Kein realer Firmware-Build:** Die hier durchgeführten statischen Tests validieren keine vollständige Tessera/ESPHome-Kompilierung; dies bleibt ein notwendiger Installationstest.
6. **Keine RTC / HA-Offline-Sperrbildschirm:** Ohne gültige Uhrzeit nach Neustart kein zuverlässiger Weckzeitpunkt; normale Touchbedienung kann bei Tessera-Verbindungsverlust blockieren. Diese Einschränkung bleibt offen.

## Installation (kurz)

1. Aktive Wecker vor dem Update ausschalten und Weckzeiten notieren.
2. Alle ZIP-Dateien und Ordner (außer den gekennzeichneten Hilfsanleitungen, die ebenfalls im Repo bleiben können) in das Root von `Karlstahl1982/tessera-alarmclock` hochladen, `main` committen.
3. Tessera → Plugins → „Branch testen“ → Repository/main neu laden, Plugin auf Hilde aktualisieren.
4. Firmware zunächst nur erstellen. Bei erfolgreichem Build installieren.
5. Tessera-Layout → AlarmClock-Kachel auf **2×3** erweitern und senden.
6. Für HA die Datei `ha_card/tessera-alarmclock-card.js` nach `/config/www/` kopieren, Ressource `/local/tessera-alarmclock-card.js` als Modul einrichten und `ha_card/LOVELACE_KARTE.yaml` als Dashboard-Karte hinzufügen.
7. Einmaligen Testwecker in wenigen Minuten anlegen, Klingeln/Schlummern/Stoppen sowie Persistenz prüfen.

## Rückbau

GitHub auf Plugin-Commit 0.1.0 zurücksetzen, Tessera neu laden und Firmware aktualisieren. Optional die HA-Dashboard-Ressource und Karte entfernen. Die bisherige lokale Lautsprecher-YAML bleibt erhalten.
