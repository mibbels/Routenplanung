# Routenplanung

Fullstack-Entwicklung eines Routenplanungstool im Wahlpflichtprojekt des Informatik Masters der HSNR im Wintersemester 2022/23

------------------

## Grobe Planung

- Projektplan erstellen (vorerst in der README)
  - Markdown-Linting via "markdownlint" VSCode-Addon von David Anson
- Projekt aufsetzen
  - C++17 (oder sind C++20-Features erwünscht?)
  - CMake
    - Zur Performanceänderung beachte Compile-Flags
      - set(CMAKE_BUILD_TYPE "Debug")
        - zu Release ändern
      - set(COMPILE_FLAGS "-Wall -Wextra -Wpedantic -Wno-unused-parameter --std=c++17 -g")
        - -g auf -O2 oder -O3 ändern
- Datenverarbeitung
- Algorithmen und Datenstrukturen
- Analysieren anhand von Beispiel
- Präsentation
- Dokumentation

------------------

## Zeitplan (tbd := to be determined)

### Anforderungen

- Projektabgabe: Woche 19.12.22 - 23.12.22 (tbd)
- Vortrag/Präsentation: Am 04.01.2023 (tbd)
- Dokumentation: Mitte - Ende Januar (tbd)

### Termine

- Sep. 2 Termine
- Okt. 4 Termine
- Nov. 5 Termine
- Dez. 3 Termine

------------------

## Implementierung

### Core

- OSM-Daten mit [OSMfilter](https://wiki.openstreetmap.org/wiki/Osmfilter) einlesen und Commandline-Tool zur Entfernung von Informationen wie Autor, Version etc. nutzen
- SAX-Parser zum Einlesen der Datei [???]
  - Einfügen in externe Datenstrukturen zur weiteren Verarbeitung
    - Externe Datenstrukturen - [STxxL](https://stxxl.org/)
      - Datenstrukturen (vector, map, etc.) und Algorithmen (find, sort, ...)
    - Graphalgorithmen benutzen wahrscheinlich [priorityqueue](https://stxxl.org/tags/1.4.1/design_pqueue.html)
      - Datenstrukturen für die Graphen - [Boost Graph Libary](https://www.boost.org/doc/libs/1_80_0/libs/graph/doc/)
      - oder selbst zu implementierende Datenstrukturen wie [hier](https://algo2.iti.kit.edu/schultes/hwy/mobileSubmit.pdf)
        - in-memory möglich?

------------------

### Vorverarbeitung

- Reduzieren von Knoten, die auf gleicher Straße liegen. Heißt alle adjazenten Knoten vom Grad zwei zusammenfassen
- Optimierung für weniger Cache-misses (durch Geo-Hashing)
  - sorgt für Lokalität der Daten (Knoten)

------------------

### Algorithmen

- Verschiedene Algorithmen implementieren und Resultate/Laufzeiten vergleichen
  - "Auswirkungen der external memory Datenstrukturen untersuchen"

------------------

### Nachverarbeitung

- Abiegeinformationen hinzufügen (z.B. durch Kantengraph oder internen Graphen pro Kreuzung)

------------------

### Visualisierung (low prio)

- Darstellung der Route (???)
- GUI (???)
- Welches Framework (SFML???)
