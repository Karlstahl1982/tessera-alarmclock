# Changelog

## 0.3.0 (Beta) – grafische Display-Oberfläche

- Neue 2×3-Kachelübersicht mit fünf Weckern und einer Plus-Kachel, solange Platz vorhanden ist.
- Zeit, Name, Status und Wiederholung sind je Wecker visuell getrennt.
- Ein/Aus direkt über die Statusplakette, Antippen der übrigen Kachel öffnet Bearbeiten.
- Neue 2×2-Bearbeitungsansicht mit großen Kacheln für Zeit, Wiederholung, Ton und Ton/Schlummern.
- Große Uhrzeit und Aktivitätsstatus im Kopf der Bearbeitung.
- Die bisherigen Detaildialoge einschließlich Drehrädern sowie Speichern/Löschen bleiben erhalten.
- Alarm-Vollbild/Stoppen/Schlummern und lokale Wecklogik unverändert aus 0.2.4.
- LVGL-Zeichenobjekte werden beim Öffnen der Ansicht vorab erzeugt, nicht bei jedem Render-Durchlauf.
- C++-Mocks und Regressionstests erweitert.

## 0.2.4

- Kompatibilitätskorrektur: Standard-LVGL-Objekt statt optionalem Button-Widget.
