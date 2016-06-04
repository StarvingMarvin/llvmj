# LlvMj

Ovaj projekat je implementacija MicroJava kompajlera i interpretera na LLVM
infrastrukturi.

## Instalacija

Projekat se moze instalirati kompajliranjem iz koda. Kod je razvijan i testiran
na Linuxu, ali bi trebalo da radi i na drugim platformama.

### Zavisnosti

Alati potrebni za izgradnju projekta:
- CMake
- Antlr3
- Odgovarajuce platformsko razvojno okruzenje (Make/GCC, VisualStudio, XCode)

Biblioteke koje treba da su dostupne na sistemu:
- LLVM 3.6
- Antlr3c

Na Debian Linuxu, sve potrebne zavisnosti mogu da se instaliraju na sledeci nacin:

    # apt-get install build-essential cmake antlr3 antlr3c-dev \
              llvm-3.6 llvm-3.6-dev llvm-3.6-tools

### Kompajliranje

Ukoliko su sve zavisnosti zadovoljene, pokretanje cmake alata ce da napravi
odgovarajuce projektne fajlove za trenutnu platformu.

Na Linuxu ce biti generisan `Makefile`, pa kompajliranje projeka izgleda ovako:

    $ cmake
    $ make

## Pokretanje

Interpreter se pokrece komandom `mji` koja od argumenata prima putanju do fajla koji treba izvrsiti.

Kompajler se pokrece komandom `mjc` koja osim putanje do fajla opciono prima jos dva argumenta:
- `-O` - nivo optimizacije, koji moze biti vrednost 0-3 (podrazumevano 2)
- `-t` - tip izlaza koji moze biti
    - `ast` - tekstualna reprezentacija apstraktnog sintaksnog stabla
    - `llvm` - LLVM asembler
    - `bc` - LLVM bajt kod
    - `asm` - platformski asembler
    - `obj` - objektli fajl (podrazumevana vrednost)

Da bi se dobio izvrsni fajl, objektni fajl je potrebno linkovati, na primer na sledeci nacin:

    $ gcc -o program program.o
