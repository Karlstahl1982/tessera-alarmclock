# Home Assistant – AlarmClock v0.3.4

Diese Karte zeigt **ausschließlich zentrale Funktionen**: nächster Alarm / Alarmstatus, alle Wecker unterdrücken und laufenden Alarm stoppen. Kein Bearbeiten einzelner Wecker in Home Assistant. Die Konfiguration bleibt am Tessera-Display.

1. `tessera-alarmclock-card.js` nach `/homeassistant/www/tessera-alarmclock-card.js` kopieren (überschreiben).
2. In Home Assistant unter **Einstellungen → Dashboards → Ressourcen** das JavaScript-Modul auf `/local/tessera-alarmclock-card.js?v=0.3.4` ändern.
3. Browser mit `Strg+F5` neu laden.
4. Bestehende Lovelace-Karte kann bleiben: `type: custom:tessera-alarmclock-card`.
5. **Erst Firmware v0.3.4 installieren**, da diese die neuen zentralen Entitäten bereitstellt.

### Bedeutung von »Wecker unterdrücken«

Solange der Schalter aktiv ist, werden neu fällige Alarme übersprungen, einschließlich einmaliger Alarme. Bereits klingelnde oder schlummernde Alarme werden beim Einschalten beendet. Beim Ausschalten wird **kein** zuvor unterdrückter Alarm nachgeholt. Der Schalter startet nach einem Neustart sicherheitshalber **AUS**.

Alte Home-Assistant-Einzelentitäten können nach einem Firmware-Update als **nicht verfügbar** in der Entitätenregistrierung verbleiben. Sie werden von ESPHome nicht mehr veröffentlicht. Alte manuell angelegte Kacheln oder Automationen, die diese Entitäten verwenden, müssen entfernt bzw. angepasst werden. Die Registrierung in Home Assistant wird nicht automatisch gelöscht.
