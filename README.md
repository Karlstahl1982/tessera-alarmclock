# Tessera AlarmClock v0.3.4 (Beta)

Lokaler Wecker für das **Guition ESP32-4848S040** mit Tessera. Der ESP32 speichert und verwaltet bis zu fünf Wecker. Home Assistant dient ab dieser Version nur noch der zentralen Statusanzeige und Unterdrückung.

## Was ist neu?

1. **Lautstärke:** Der 60-Sekunden-Anstieg wird statt in 6-Sekunden-Sprüngen mit kontinuierlich berechnetem Gain und 1-Sekunden-Updates ausgegeben. Die Alarmanzeige zeigt den aktuellen Software-Pegel in %, das Ziel in % und einen Countdown bis zum Zielpegel.
2. **Fortschrittsbalken:** Gefüllter Balken und Hintergrund liegen jetzt als eigene, explizit platzierte LVGL-Rechtecke übereinander. Damit wird die vorherige mögliche Abschneidung durch den Container vermieden.
3. **Schlummern:** Ein schlummernder Wecker bleibt unter „Meine Wecker“ mit `Zz` und Restzeit sichtbar, auch wenn sein einmaliger Termin bereits automatisch deaktiviert wurde. Die Hauptkachel zeigt die verbleibende Schlummerzeit.
4. **Home Assistant aufgeräumt:** Nur die drei zentralen Entitäten **AlarmClock Nächster Wecker**, **AlarmClock Wecker unterdrücken** und **AlarmClock Alarm stoppen** werden veröffentlicht. Die Konfiguration der fünf Wecker findet ausschließlich am Display statt. Die eigene HA-Karte zeigt nur diese zentrale Bedienung.

## Voraussetzungen

- Tessera/ESPHome (getesteter Projektstand: ESPHome 2026.9.0 und Tessera Screens v0.4.96).
- Funktionierende lokale Audio-YAML mit `guition_tone` (RTTTL) und `guition_speaker` (I²S); die Audio-Brücken R21–R23 müssen richtig bestückt sein.
- Erfolgreicher Stand v0.3.3 als Rückfalloption empfohlen.

## Installation (sequenziell)

1. ZIP vollständig entpacken und **den Inhalt**, nicht den übergeordneten Ordner, in den Stamm von `https://github.com/Karlstahl1982/tessera-alarmclock` hochladen. Die Dateien `plugin.yaml`, `tessera-plugin.yaml`, `components/…` müssen direkt im Repository liegen.
2. Auf `main` committen. In Tessera über **Plugins → Mit Link hinzufügen → Branch testen**, Repository und Branch `main`, die neue Version **0.3.4** mit dem neuen Commit auswählen.
3. **Zuerst nur Firmware erstellen.** Bei fehlerfreiem ESPHome-Build auf das Display übertragen. Die vorhandenen Wecker sollen aufgrund der unveränderten ESPHome-IDs erhalten bleiben; vor dem Flashen die fünf Zeiten/Einstellungen notieren.
4. AlarmClock am Display öffnen. Testalarm starten und prüfen: Balken wächst über 60 Sekunden, aktuelle Lautstärke steigt bis zum Ziel, Countdown zählt herunter. Den Testalarm per Stoppen quittieren.
5. Einen einmaligen Wecker zum Testen anlegen und schlummern lassen. Erwartung: Wecker bleibt in „Meine Wecker“ sichtbar, mit `Zz` und herunterzählender Restzeit, und erscheint weiterhin als nächster Alarm auf der Hauptkachel.
6. Home-Assistant-Karte aktualisieren: `ha_card/tessera-alarmclock-card.js` nach `/homeassistant/www/tessera-alarmclock-card.js` kopieren und überschreiben. In **Einstellungen → Dashboards → Ressourcen** die URL auf `/local/tessera-alarmclock-card.js?v=0.3.4` ändern und Browser mit `Strg+F5` neu laden.
7. Zentralen Schalter „Wecker unterdrücken“ kurz testen und danach **wieder ausschalten**. Ein aktiver Testalarm lässt sich mit dem Button „Alarm stoppen“ beenden.

## Bedeutung der Unterdrückung

**AN:** Ein bereits klingelnder oder schlummernder Alarm wird beendet. Neue fällige Alarme werden übersprungen, nicht nur stumm abgespielt. Auch einmalige Wecker gelten beim Auslösen als erledigt. Es gibt **kein Nachholen**, wenn die Unterdrückung anschließend ausgeschaltet wird.

**AUS:** Wieder regulärer Weckbetrieb. Nach einem Neustart steht die globale Unterdrückung sicherheitshalber auf **AUS**.

## Wichtige Hinweise

- Die 5 Einzel-Wecker-Entitäten und ihre Einstellungen sind ab sofort `internal: true`. Sie werden nicht mehr über die ESPHome-API für Home Assistant angeboten, bleiben aber als lokale Komponenten erhalten. **Bereits registrierte Entitäten können in Home Assistant als „nicht verfügbar“ verbleiben**; deren Registry-Einträge und alte Dashboard-Kacheln gegebenenfalls manuell bereinigen.
- Eine ältere HA-Karte, die Wecker direkt über HA-Entitäten bearbeitet, funktioniert mit dieser Version nicht mehr; sie muss aktualisiert werden.
- Die % sind digitale Software-Sollwerte, keine gemessenen Schalldruckpegel.
- Keine RTC-Hardware vorhanden. Nach vollständigem Stromverlust ohne neue Zeitquelle ist eine korrekte Uhrzeit nicht garantiert.
- **Reale ESPHome-Kompilierung, OTA-Übertragung und akustische Prüfung stehen noch aus.** Automatisierte Mock-Tests ersetzen diese nicht. Bis zum erfolgreichen Praxistest bitte einen zweiten Wecker zur Absicherung verwenden.
