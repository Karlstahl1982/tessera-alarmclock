# Testbericht – Tessera AlarmClock 0.3.1

**Automatisierte Prüfungen:** 18/18 bestanden (Python unittest, einschließlich C++-Syntaxprüfung mit nachgebildeten Tessera-/ESPHome-Schnittstellen und JavaScript-Runtime-Smoke-Test).

## Geprüft

- Manifest v0.3.1 und vollständige Plugin-Paketstruktur
- fünf Weckerslots und ESPHome-Entity-Bindings
- deutsche und englische Übersetzungsschlüssel
- vorhandene Audio- und Vollbildalarm-Funktionen unverändert
- grafische Listen-/Bearbeitungsansichten und Detailmenüs
- Interaktionszonen und vertikale Textabstände für 480x360, 480x380 und 480x480 Kartenansichten
- Home-Assistant-Serviceaufrufe, JavaScript-Syntax und UI-Smoketest
- C++-Syntax (`g++ -fsyntax-only`) mit lokalen Mocks

## Nicht geprüft

- Vollständiger ESPHome-Build auf Guition ESP32-4848S040 / Tessera screens-v0.4.96
- Tatsächliche Textmetriken, Touch-Reaktionszeit und Rendering auf dem physischen Display
- Betrieb bei WLAN-/HA-Ausfall und Audio nach OTA-Flash

**Freigabe:** Beta für kontrollierten Test, **nicht** als alleiniger zuverlässiger Wecker, bevor die Realtests erfolgreich waren.
