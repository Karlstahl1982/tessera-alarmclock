# Changelog – Tessera AlarmClock

## 0.3.8 – Unnötiges Bearbeiten-Symbol entfernt
- Im Normalbetrieb wird unten rechts auf der großen Weckerkachel **kein Symbol mehr** angezeigt.
- Bei aktivem Schlummern bleibt das rote X zum gezielten Abbrechen sichtbar und bedienbar.
- Tippen auf den übrigen Kachelbereich öffnet unverändert „Meine Wecker“.
- Keine Änderungen an Weckzeiten, Audio, Lautstärkekurve oder Home-Assistant-Entitäten.
- Regressionstest zur Sichtbarkeit des Symbols ergänzt.

## 0.3.7 – Hauptkachel: Schlummern abbrechen
- Das bisherige blaue, aus Rechtecken gezeichnete Bearbeiten-Symbol wurde auf der Startkachel als Stiftsymbol beibehalten und leicht nachgezeichnet.
- Während eines laufenden Schlummermodus zeigt die Hauptkachel stattdessen einen roten Kreis mit deutlich sichtbarem weißem X.
- Antippen dieses Buttons beendet ausschließlich den in der Hauptkachel angezeigten Schlummeralarm, einschließlich des vorgemerkten Wiederanlaufs.
- Antippen außerhalb des X öffnet wie bisher die Weckerübersicht.
- Die bestehenden Audio-, Weckzeit-, Fortschritts- und Home-Assistant-Funktionen bleiben unverändert.
- Neuer C++-Laufzeittest prüft die Touchbereiche und das gezielte Beenden eines Schlummeralarms.

## 0.3.6
- Lautstärkestart bei 5 %, danach +5 Prozentpunkte alle 15 Sekunden bis zur individuellen Grenze.
