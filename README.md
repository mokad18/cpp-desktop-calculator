# C++ Desktop Calculator

Ein minimalistischer, vollständig objektorientierter Desktop-Taschenrechner, geschrieben in C++. Die grafische Benutzeroberfläche wurde mit dem FLTK-Framework realisiert und verwendet ein modernes Dark-Pastel Farbschema.

## Architektur & Features
Das Projekt legt Wert auf Clean Code und eine saubere Softwarearchitektur:
- **Objektorientiert (OOP):** Vollständige Kapselung von Zustand und GUI-Logik in einer zentralen Klasse (`Calculator`).
- **Keine globalen Variablen:** Der gesamte Rechenzustand wird sicher innerhalb der Instanz verwaltet.
- **Erweiterte Mathematik:** Neben den Grundrechenarten sind Funktionen für Quadratwurzeln (√x), Quadrate (x²) und Prozentrechnung integriert.
- **Flüssige UX:** Der komplette Rechenweg bleibt im Display sichtbar, bis das Ergebnis berechnet wird.

## Voraussetzungen & Installation der Abhängigkeiten

Das Projekt benötigt einen C++ Compiler (C++17), CMake und das FLTK-Framework.

### macOS
Die Installation erfolgt am einfachsten über Homebrew:
```bash
brew install fltk