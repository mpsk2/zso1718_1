AlienOS symulator
*****************

Instalacja
##########

.. code:: sh

	make

powinno wystarczyć

Opis rozwiązania
################

Program zaczyna działanie jako jedne proces.
Następnie forkuje się na 2. 
Dziecko podpina sygnał pod wyjście rodzica, 
uruchamia opcje bycia śledzonym i przełącza się
na proces w podanym pliku argv[1], pierwszy parametr

Rodzic śledzi dziecko, a mianowicie jego wywołania 
syscall.
Każde jest śledzone oddzielnie. 
Jako rand używam wywołania systemowego 318 getrandom
Do obsługi terminala używam biblioteki ncurses

Jako tło pod literkami ustalam czarny kolor (można zmienić flagę kompilacji -DBG_COLOR=kod ncurses
Kolory oszukuję, nie robię jasnego i ciemnego tylko pogrubiam, bo jest jaśniej.
Domyślny terminal daje zdefiniować 8 kolorów (ograniczenie stałej COLORS).

Do wstawiania (argumenty) informacji do pamięci dziecka używam process_vm_writev,
a do odczytywania (tekst do wyświetlenia) process_vm_readv.

Wstawiając parametry do pamięci zakładam, że jest jedna sekcja .params i że są w takiej kolejności jak parametry podane.
Jeżeli tej sekcji nie ma, to nie wstawiam, żadnych parametrów.

Program powinien skończyć działanie gdy dostanie:

- Zabijemy rodzica
- Dziecko skończy działanie
- Dziecko zawoła syscall end
- Argumenty nie będą liczbami, lub nie da się ich sparsować (np. za długie)
- Któraś z kluczowych używanych funkcji skończy się niepowodzeniem
- Inne

Autor
#####

Michał Piotr Stankiewicz

numer indeksu 335789


