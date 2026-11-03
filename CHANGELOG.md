# Änderungsprotokoll

## 0.2.3 – UI/Alarm-Bedienung

- Bestehende Wecker erhalten eine kompakte, grafische Bearbeitungsübersicht am Display; der Wizard bleibt für die Erstanlage.
- Home Assistant: grafische Bearbeitungsansicht statt vierstufigem Wizard für vorhandene Wecker.
- Dismissible Tessera-Popup durch blockierendes LVGL-System-Overlay ersetzt; Touches außerhalb der Aktionen werden abgefangen.
- Ton verstummt nach der eingestellten Höchstdauer, die Quittierungsansicht bleibt bis Stoppen/Schlummern bestehen.
- Native ESPHome-Entitäten und gespeicherte Weckerdaten bleiben kompatibel.

## 0.2.2 – Lautstärkeregler

- Live-Anzeige des Prozentwertes.

## 0.2.1 – Home-Assistant-Zeit-/Datumsdienste

- `time.set_value` und `date.set_value` statt unpassendem `datetime.set_value`.

## 0.2.0 – Neugestaltung

- Große Dashboard-Kachel, fünf Wecker, Touch-Wizard, eigene HA-Karte.
