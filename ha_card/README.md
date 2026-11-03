# Home Assistant – zentrale AlarmClock-Karte

Die JavaScript-Karte ist gegenüber v0.3.5 **unverändert**. Für das reine Plugin-Update auf **v0.3.6** ist **keine** neue HA-Ressourceninstallation oder Cache-Aktualisierung erforderlich.

Falls die Karte noch nicht installiert ist:

1. `ha_card/tessera-alarmclock-card.js` nach `/homeassistant/www/tessera-alarmclock-card.js` kopieren.
2. Unter **Einstellungen → Dashboards → Ressourcen** die URL `/local/tessera-alarmclock-card.js?v=0.3.4` als JavaScript-Modul eintragen (diese Kartenfassung ist kompatibel).
3. In der Dashboard-Verwaltung eine manuelle Karte mit `type: custom:tessera-alarmclock-card` anlegen.
4. Benötigt eine Firmware ab **v0.3.4** mit den drei zentralen Entitäten (Nächster Wecker, Wecker unterdrücken, Alarm stoppen).

Die einzelnen Wecker werden **nur am Display** konfiguriert; in HA gibt es keine Einzel-Wecker-Steuerung.
