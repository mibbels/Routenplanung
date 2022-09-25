Projektplan erstellen
Algorithmen und Datenstrukturen implementieren 
	- Analysieren anhand von Beispiel
Dokumentation 

Zeitplan
Projektabgabe vorraussichtlich Woche 19. - 23. Dezember <br>
Vortrag 04.01.2023 <br>
Abgabe Dokumentation etwas später? <br>
Sep.   1     Termin <br>
Okt.   4     Termine <br>
Nov.   5    Termine <br>
Dez.   2/3 Termine <br>

------------------

- OSM Daten einlesen
    - Mit [OSMfilter](https://wiki.openstreetmap.org/wiki/Osmfilter) commandline-tool Informationen wie Autor, Version etc. entfernen.
    - SAX-Parser zum Einlesen der Datei [??]
    - Einfügen in externe Datenstrukturen zur weiteren Verarbeitung
	- Externe Datenstrukturen - [STxxL](https://stxxl.org/)
        - Datenstrukturen (vector, map, etc.) und Algorithmen (find, sort, ...)
		- Graphalgorithmen benutzen wahrscheinlich [priorityqueue](https://stxxl.org/tags/1.4.1/design_pqueue.html)


	- Datenstrukturen für die Graphen - [Boost Graph Libary](https://www.boost.org/doc/libs/1_80_0/libs/graph/doc/)
		- oder selbst zu implementierende Datenstrukturen wie [hier](https://algo2.iti.kit.edu/schultes/hwy/mobileSubmit.pdf) 
        - in-memory möglich?

--------------------	

- Vorverarbeitung
    - Reduzieren von Knoten, die auf gleicher Straße liegen. Heißt alle adjazenten Knoten vom Grad zwei zusammenfassen.
    - Optimierung für weniger cache-misses  durch geo-hashing 
        - sorgt für lokalität der Daten (Knoten)
		
---------------------

Verschiedene Algorithmen implementieren und Resultate und Laufzeiten vergleichen
- "Auswirkungen der external memory Datenstrukturen untersuchen"

---------------

Abiegeinformationen hinzufügen z.B. durch Kantengraph oder internen Graphen pro Kreuzung 

-----------------------

Darstellung GUI
