# Changelog

## 0.3.3 (Beta) – Sofort ablaufenden Erstalarm beheben
- Kritischen Zeitstempel-Fehler im Tick behoben: Der beim Scheduler erfasste Zeitwert wurde mit einem später ermittelten `millis()`-Startzeitwert verglichen. Bereits 1 ms negative Differenz löste einen sofortigen 5-Minuten-Timeout aus.
- Einheitlichen Zeitstempel für Alarmbeginn, Audio-Start, Lautstärke-Rampe und Zeitlimit eingeführt.
- Zusätzliche Schutzfunktion für leicht negative Zeitdifferenzen und Zählerüberlauf.
- Regressionstest reproduziert den Fehler auf v0.3.2 und verifiziert die Behebung inklusive Schlummern.
- Keine Änderungen an Wecker-Datenmodell, ESPHome-Entitäten, Lautstärkeprofil oder Home-Assistant-Oberfläche.

# Changelog

## 0.3.2 (Beta) – Erster Alarmton und Lautstärke-Countdown

- Audio wird beim Start des ersten Alarms sofort angestoßen, bevor das Vollbild aufgebaut wird; aktive Tonvorschau wird beendet.
- Kaltstart-Wiederholung: Falls der I²S-Lautsprecher nicht in Betrieb geht, bis zu zwei begrenzte Startversuche nach jeweils vier Sekunden; kein endloses Wiederholen.
- Lautstärkeanstieg in 10 definierten Schritten alle 6 Sekunden innerhalb einer Minute. Start bei 50 % des vom Benutzer gesetzten Zielpegels; danach schrittweise bis zum Ziel.
- I²S-Lautstärke wird während laufender RTTTL-Wiedergabe aktiv nachgeregelt (nicht nur `rtttl.set_gain`).
- Digitaler Maximalpegel von bisher 15 % auf konservativ begrenzte 35 % angehoben, da der frühere Startpegel von 0,9 % bei Sollwert 60 % häufig kaum hörbar war. Keine Schalldruckkalibrierung.
- Alarm-Vollbild: aktueller Sollpegel in %, Zielpegel in %, Balken sowie Countdown „Nächste Erhöhung in X s“. Anzeigen für Audioinitialisierung und möglichen Startfehler.
- Der Testalarm dauert nun 75 Sekunden, um alle Lautstärkestufen beobachten zu können (jederzeit manuell stoppbar). Regulärer Alarm weiterhin maximal 5 Minuten Ton.
- Ergänzte dynamische C++-Tests für Audio-Erststart, fehlgeschlagenen I²S-Kaltstart und echte Lautstärkeänderungen.
- Bestehende fünf Wecker, Grafikkacheln, Vollbild-Stoppen/Schlummern sowie HA-Karte unverändert.

## 0.3.1
- Grafisches Redesign von Kacheln und Untermenüs.

## 0.3.0
- Erster grafischer Weckerraster-Aufbau.

## 0.2.4
- Blockierende Alarm-Vollbildansicht.
