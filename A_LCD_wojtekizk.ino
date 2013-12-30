/*
Przykład użycia biblioteki MenuBackend do obsługi wielopoziomowego menu:
...by wojtekizk (wk@warcaby.lh.pl) przygotowany na forum majasterkowo.pl
//----------------------------------------------------------------------------------
Opis:
Generalnie tworzenie menu w oparciu o bibliotekę MenuBackend jest bardzo proste,
jeśli pamietać o kilku zasadach:
1) Najważniejsza jest konfiguracja Menu w funkcji menuSetup()
2) Poruszanie po menu powinno byc zgodne ze schematem menu, tzn. jesli
   coś jest submenu, to szukasz tego klawiszem "w prawo", a równorzędne "dół - góra"
3) Warto dla użytkownika wydrukować taka mapkę przy klawiaturce. 
4) Obsługę wszystkich opcji wrzucaj do funkcji void menuUseEvent(MenuUseEvent used)
   Tam zapuszczasz pętle while lub do while i czytasz w niej stan klawiaturki.
   W tej pętli modyfikujesz zachowanie programiku w zależności od tego, co wciśniesz. 
   Jeśli wciśniesz OK to wychodzisz z pętli while i de facto wracasz do loop-a.
Wszystkie ważniejsze kwestie opatrzono dość czytelnym komentarzem.   
*/
// ============= MenuBackend tutorial ===============================================
#include <MenuBackend.h>                 // dołączenie biblioteki
#include <LiquidCrystal.h>               // obsługa wyświetlacza LCD
// --- definiujemy dla LCD własne znaki strzałek: dół, lewo, prawo, gora-dół i powrót ---
uint8_t arrowUpDown[8] = {0x4,0xe,0x15,0x4,0x15,0xe,0x4};
uint8_t arrowDown[8]  = {0x4,0x4,0x4,04,0x15,0xe,0x4};
uint8_t arrowRight[8] = {0x0,0x4,0x2,0x1f,0x2,0x4,0x0};
uint8_t arrowLeft[8] = {0x0,0x4,0x8,0x1f,0x8,0x4,0x0};
uint8_t arrowBack[8] = {0x1,0x1,0x5,0x9,0x1f,0x8,0x4};
LiquidCrystal lcd(26, 24, 22, 23, 25, 27, 29);
    // definicja pinów dla LCD (sprawdź piny w swoim LCD)
volatile int zm =-1;               // to dla kontroli zmiany stanu klawiatury
volatile int x=-1;                 // zmienna pomocnicza
volatile int stan_Analog;          // wartość na wejściu Analogowym dla klawiaturki analogowej
char *linia1;                      // pierwsza linia wyświetlanego tekstu na LCD
char *linia2;                      // druga linia wyświetlanego tekstu na LCD
/* Przykładowe Menu (podobieństwo do IDE Arduino): --------------------------------------------
// mamy 5 głównych opcji PLIK, EDYCJA, SZKIC, NARZEDZIA, POMOC
// w opcji SZKIC i NARZEDZIA mamy rozbudowane opcje dla 2 i 3 poziomu Menu
// --------------------------------------------------------------------------------------------
      PLIK
         Nowy
         Otworz
         Szkicownik
         Przyklady
         Zapisz
         Zapisz jako
         Zaladuj
         Exit
      EDYCJA
         Wytnij
         Kopiuj
         Wklej
         Zaznacz
         Znajdz
      SZKIC
         Weryfikuj
         Kompiluj
         Importuj
             EEPROM
             GSM
             SD
             MenuBackend
      NARZEDZIA
          Plytka
             Arduino Uno
             Leonardo
             Decimila
             Nano
             LilyPad
          Odczyt
             Temperatura
             COM 2 
             COM 15 
          Programator      
             USBasp
             AVR ISP
             AVR ISP MK II
      POMOC 
          Jak zaczac
          Srodowisko
          Dokumentacja
          O Arduino
--- Poniżej definicja opcji menu: ------------------------------------------ 
de facto to definicja konstruktora dla obiektu klasy MenuBackend o nazwie menu.
Tutaj definiujemy każdy element menu. Możemy robić to na 2 sposoby:
- bez używania tzw. shortkey, czyli np. MenuItem P1 = MenuItem("PLIK");
- lub z szortkey, czyli MenuItem P1 = MenuItem("PLIK",1); 
Mamy tu dodatkowy parametr i dzieki niemu będziemy mogli decydować co ma się
wyświetlać na wyświetlaczu. W tym przykładzie parametr shortkey identyfikuje 
nam poziom zagnieżdżenia menu. Potem w dość prosty sposób za pomocą strzałek 
pomagamy użytkownikowi w wyborze.
*/
// --- tworzymy wszystkie opcje Menu: ---------------------------------------
// de facto tworzymy obiekty klasy MenuItem, które dziedziczą po klasie MenuBackend
MenuBackend menu = MenuBackend(menuUseEvent,menuChangeEvent); // konstruktor 
   //                        ("                ")
   MenuItem P1 =  MenuItem("NUSTATYMAI",1);
      MenuItem P11 = MenuItem("Pasvietimas",2);
      MenuItem P12 = MenuItem("Irasymas",2);
      MenuItem P13 = MenuItem("Reiksmiu atst",2);
//      MenuItem P14 = MenuItem("  Przyklady",2);
//      MenuItem P15 = MenuItem("    Zapisz",2);
//      MenuItem P16 = MenuItem(" Zapisz jako..",2);
//      MenuItem P17 = MenuItem("   Zaladuj",2);
//      MenuItem P18 = MenuItem("     Exit",2);
      
   MenuItem P2 = MenuItem("AKUMULIACINE",1);
      MenuItem P21 = MenuItem("Ijungimo temp.",2);
      MenuItem P22 = MenuItem("Isjungimo temp.",2);
      MenuItem P23 = MenuItem("Rankinis vald.",2);
//      MenuItem P24 = MenuItem("    Zaznacz",2);
//      MenuItem P25 = MenuItem("    Znajdz",2);
   
   MenuItem P3 = MenuItem("SAULES KOLEKT.",1);
      MenuItem P31 = MenuItem("Ijungimo t.skirt",2);
      MenuItem P32 = MenuItem("Isjungimo t.skirt",2);
      MenuItem P33 = MenuItem("Apsauga nuo salc",3);
      MenuItem P34 = MenuItem("Nuorinimas",3);
//          MenuItem P331 = MenuItem(" Menu Backend",4);
//          MenuItem P332 = MenuItem("    EEPROM",4);
//          MenuItem P333 = MenuItem("   KeyBoard",4);
//          MenuItem P334 = MenuItem("      GSM",4);
   
/*   MenuItem P4 = MenuItem("    IRANKIAI",1);
      MenuItem P41 = MenuItem("    Plytka",3);
          MenuItem P411 = MenuItem("  Arduino Uno",4);
          MenuItem P412 = MenuItem("   Leonardo",4);
          MenuItem P413 = MenuItem("   Decimila",4);
          MenuItem P414 = MenuItem("   LilyPad",4);
          MenuItem P415 = MenuItem("     Nano",4);
      MenuItem P42 = MenuItem("    Odczyt",3);  
          MenuItem P421 = MenuItem(" Temperatura",4);
          MenuItem P422 = MenuItem("     COM 2",4);
          MenuItem P423 = MenuItem("    COM 13",4);
      MenuItem P43 = MenuItem("  Programator",3);  
          MenuItem P431 = MenuItem("    USBasp",4);
          MenuItem P432 = MenuItem("    AVR ISP",4);
          MenuItem P433 = MenuItem(" AVR ISP MK II",4);    
      
      MenuItem P5 = MenuItem("     PAGALBA",1);
          MenuItem P51 = MenuItem("  Jak zaczac",2);
          MenuItem P52 = MenuItem("  Srodowisko",2);
          MenuItem P53 = MenuItem(" Dokumentacja",2);
          MenuItem P54 = MenuItem("  O Arduino",2);
          */
/* --- Teraz pozycjonujemy  menu ( zgodnie z ustawieniem podanym powyżej) ------------
add - dodaje w pionie, addRight - dodaje w poziomie z prawej , addLeft dodaje z lewej
*/
void menuSetup()                       // funkcja klasy MenuBackend 
{
      menu.getRoot().add(P1);          // ustawiamy korzeń Menu, czyli pierwszą opcję
      P1.add(P11);                     // rodzic PLIK ma dziecko Nowy więc dodaje je w pionie 
        P11.add(P12);P11.addLeft(P1);  // poniżej Nowy jest Otworz więc także w pionie 
                                       // a addLeft(P1) pozwoli nam wrócić klawiszem w lewo do PLIK  
        P12.add(P13);P12.addLeft(P1);  // analogicznie robimy ze wszystkimi podopcjami dla PLIK
        P13.addLeft(P1);P13.add(P11);
//        P14.add(P15);P14.addLeft(P1);
//        P15.add(P16);P15.addLeft(P1);
//        P16.add(P17);P16.addLeft(P1);
//        P17.add(P18);P17.addLeft(P1);
//        P18.addLeft(P1);P18.add(P11);  // tutaj zamykamy pętlę i wracamy do pierwszej podopcji
                                       // dzieki temu nie musimy wracać na górę przez uciążliwe 
                                       // klikanie klawisza Up 
                                  
      P1.addRight(P2);                 // po prawej dla PLIK jest EDYCJA
      P2.add(P21);                     // rodzic EDYCJA ma dziecko Wytnij
        P21.add(P22);P21.addLeft(P2);  // poniżej Wytnij jest Kopiuj
        P22.add(P23);P22.addLeft(P2);  // analogicznie dla wszystkich podopcji
        P23.addLeft(P2);P23.add(P21);
//        P24.add(P25);P24.addLeft(P2);
//        P25.addLeft(P2);P25.add(P21);  // i zamknięcie pętli oraz ew. powrót do pierwszej opcji
      P2.addRight(P3);                 // na prawo od EDYCJA jest SZKIC 
      P3.add(P31);                     // rodzic SZKIC ma dziecko Weryfikuj 
        P31.add(P32);P31.addLeft(P3);  // poniżej Weryfikuj jest Kompiluj 
        P32.add(P33);P32.addLeft(P3);
        P33.add(P34);P33.addLeft(P3);
        P34.addLeft(P3);P34.add(P31);    // poniżej kompiluj jest Importuj 
      P3.addRight(P1);
/*        P33.addRight(P331);            // a tu dziecko Importuj ma już własne dziecko MenuBackend
                                       // dodajemy z prawej, ponieważ gdybyśmy dali poniżej to zrobilibyśmy
                                       // kolejne dziecko dla SZKIC, a w projekcie jest inaczej
          P331.add(P332);P331.addLeft(P33);  // poniżej MenuBackend jest EEPROM
          P332.add(P333);P332.addLeft(P33);  // postepujemy analogicznie
          P333.add(P334);P333.addLeft(P33);
          P334.addLeft(P33);P334.add(P331);
        P33.addLeft(P3);P33.add(P31);   // zamknięcie pętli i ew. powrót do pierwszej opcji  
      P3.addRight(P4);                  // dalej podobnie .... 
      P4.add(P41);
        P41.addRight(P411);             // kolejne dziecko, ktore ma dziecko :-)
          P411.add(P412);P411.addLeft(P41);
          P412.add(P413);P412.addLeft(P41);
          P413.add(P414);P413.addLeft(P41);
          P414.add(P415);P414.addLeft(P41);
          P415.addLeft(P41);P415.add(P411); // zamknięcie pętli itd...
        P41.addLeft(P4);
        P41.add(P42);
        P42.addRight(P421);
          P421.add(P422);P421.addLeft(P42);
          P422.add(P423);P422.addLeft(P42);
          P423.addLeft(P42);P423.add(P421); // zamkniecie pętli itd...
        P42.addLeft(P4);
        P42.add(P43);
        P43.addRight(P431);
          P431.add(P432);P431.addLeft(P43);
          P432.add(P433);P432.addLeft(P43);
          P433.addLeft(P43);P433.add(P431); // zamkniecie pętli itd...
        P43.addLeft(P4);P43.add(P41);
        P4.addRight(P5);
      P5.add(P51);
        P51.add(P52);P51.addLeft(P5);
        P52.add(P53);P52.addLeft(P5);
        P53.add(P54);P53.addLeft(P5);
        P54.addLeft(P5);P54.add(P51);     // zamkniecie pętli
      P5.addRight(P1);                    // zamkniecie pętli głównej, czyli poziomej - po POMOC jest PLIK
      */
}
// ----------- uff... nareszcie :-) -----------------------------------------------------------------------
void menuUseEvent(MenuUseEvent used)      // funkcja klasy MenuBackend - reakcja na wciśnięcie OK
                                          // tutaj właśnie oddajemy menu na rzecz akcji obsługi klawisza OK
{
   Serial.print("wybrano:  "); Serial.println(used.item.getName()); // do testów, potem niepotrzebne
   // --- ponizej kilka przykładów obsługi  opcji -----------
   // przykładowa reakcja na wcisnięcie klawisza OK w opcji Otworz :
   if (used.item.getName() == "    Otworz")   // Uwaga - dokładnie taki sam ciąg "    Otworz" jak w menu !!!
                                              // bo przecież getName() pobiera nazwę
      {
        lcd.setCursor(1,0);lcd.print("Otwieram drzwi"); // info 
        digitalWrite(0,HIGH);delay(2000);digitalWrite(0,LOW); // na 2 sekundy pin 0 otrzymał stan wysoki
                                                              // czyli np. otworzyły się drzwi
        lcd.setCursor(1,0);lcd.print("              ");lcd.setCursor(1,0);lcd.print(linia1); //poprzedni stan LCD
      }
    // A teraz coś ambitniejszego :-), bo przekazujemy sterowanie klawiaturką do innej procedury,
    // w tym przykładzie programik czeka aż ustawisz jakąś temperaturę i po wciśnięciu OK wraca do pętli głównej
      if (used.item.getName() == " Temperatura")   // dokładnie taki sam ciąg " Temperatura"
      {
        int temp=21;                         // przykładowo 21 st. C
        lcd.setCursor(0,0);lcd.write(7);     // wyswietlamy nasz symbol strzałki góra-dół
        lcd.print("              ");lcd.setCursor(1,0);lcd.print("Ust.temp. = "); // tekst dla użytkownika
        lcd.setCursor(13,0);lcd.print(temp); // wyświetlamy akt. temperaturę
        int  akcja=-1;delay(1000);         // zmienna pomocnicza, sterująca dla petli while
                                           // jesli nie puścisz klawisza OK w ciągu 1 sek. to powrót do menu    
        while(akcja!=4)                   // ta pętla trwa tak długo aż wciśniesz klawisz OK  
         {
           zm=-1; 
           akcja=czytaj_1(0);//delay(300);   // odczyt stanu klawiatury - funkcja czytaj_1 lub czytaj_2 lub czytaj_3
                                            // opis poniżej przy 3 różnych definicjach funkcji czytaj
           if(zm!=akcja)                    // ruszamy do pracy tylko wtedy gdy zmienił sie stan klawiatury
             {
             if (akcja==1) {temp++;if(temp>99)temp=99;lcd.setCursor(13,0);lcd.print(temp);delay(300);}
               // jesli akcja=1 (czyli wciśnieto klawisz w górę to zwiększono temperaturę
               // ustawiono max próg i wyświetlono obecną temperaturę
             if(akcja==2)  {temp--;if(temp<10)temp=10;lcd.setCursor(13,0);lcd.print(temp);delay(300);}
               // jesli akcja=2 (czyli wciśnieto klawisz w dół to mniejszono temperaturę
               // ustawiono min próg i wyświetlono obecną temperaturę
             if(akcja==4) // jeśli wciśnieto OK 
               {
                 lcd.setCursor(0,0);lcd.print("*Temperatura OK");delay(2000); // pokazujemy OK przez 2 sek.
                 lcd.setCursor(1,0);lcd.print("              "); // czyścimy linię
                 lcd.setCursor(1,0);lcd.print(linia1);           // odtwarzamy poprzedni stan na LCD
               }
             } 
         } zm=akcja;  // aktualizacja zmiennej zm, po to aby reagować tylko na zmiany stanu klawiatury
         // tu WAŻNY MOMENT - kończy się pętla while i zwracamy sterowanie do głównej pętli loop()
      }
// a tutaj obsługa pozostałych opcji :-)  
// ...
// ...
}
// --- Reakcja na wciśnięcie klawisza -----------------------------------------------------------------
void menuChangeEvent(MenuChangeEvent changed)  // funkcja klasy MenuBackend 
{
  /* tak naprawdę to tylko tutaj przydaje się ów shortkey i służy przede wszystkim do wzbogacenia menu
     o symbole strzałek w zależności co wybrano. Wszystko co tutaj się wyprawia jest pokazywane na LCD. 
  */
  int c=changed.to.getShortkey();                         // pobieramy shortkey (1,2,3, lub4)
  lcd.clear();                                            // bez komentarza 
  lcd.setCursor(0,0); 
  if(c==1)                                                // jeśli to menu głowne (shortkey=1) to:
    {
    lcd.write(3);                                         // strzałka w lewo
    strcpy(linia1,changed.to.getName());                  // tworzymy napis w pierwszej linii
    lcd.print(linia1);                                    // wyświetlamy ją
    lcd.setCursor(19,0);lcd.write(4);                     // strzałka w prawo
    lcd.setCursor(0,1);lcd.write(5);                      // strzałka w dół
    lcd.setCursor(19,1);lcd.write(5);                     // strzałka w dół
    }
    if(c==2)                                              // jeśli to podmenu dla dziecka - (shortkey=2) to:
    {
    lcd.print("*");                                       // rysujemy gwiazdkę
    strcpy(linia2,changed.to.getName());                  // tworzymy napis w pierwszej linii
    lcd.print(linia1);                                    // wyświetlamy ją
    lcd.setCursor(19,0);lcd.print("*");                   // gwiazdka 
    lcd.setCursor(0,1);lcd.write(6);                      // druga linia i strzałka powrotu (arrowBack)
    lcd.print(changed.to.getName());                      // wyświetlamy nazwe "dziecka"
    lcd.setCursor(19,1);lcd.write(7);                     // strzałka góra-dół
    }
    if(c==3)                                              // jeśli dziecko  ma dziecko - (shortkey =3) to:
    {
    lcd.print("*");                                       // gwiazdka
    strcpy(linia2,changed.to.getName());                  // kopiujemy akt. nazwe opcji menu do zmiennej linia2
    lcd.print(linia1);                                    // i wyświetlamy pierwszą linię
    lcd.setCursor(19,0);lcd.print("*");                   // gwiazdka
    lcd.setCursor(0,1);lcd.write(6);                      // druga linia i strzałka arrowBack
    lcd.print(changed.to.getName());                      // wyświetlamy wnuka w drugiej linii
    lcd.setCursor(19,1);lcd.write(4);                     // strzałka w prawo bo są wnuki
    }
    
    if(c==4)                                              // jeśli to wnuk  (shortkey =4) to:
    {
    lcd.print("*");                                       // gwaizdka
    lcd.print(linia2);                                    // w pierwszej linii wyświetlamy dziecko ( czyli rodzica wnuka) 
    lcd.setCursor(19,0);lcd.print("*");                   // gwaizdka
    lcd.setCursor(0,1);lcd.write(6);                      // druga linia i strzałka arrowBack
    lcd.print(changed.to.getName());                      // wyświetlamy wnuka
    lcd.setCursor(19,1);lcd.write(7);                     // strzałka góra-dół 
    }
}
/* --- ponżej funkcja odczytująca stan klawiatury -------------------------------------------
przygotowałem 3 różne wersje:
1) dla klawiaturki analogowej z shielda LCDanalogKey firmy DFRobot
2) dla joysticka ( 2 wejscia Analogowe i 1 pin cyfrowy )
   Uwaga dla tej wersji opcji musisz dopisać w funkcji setup konfigurację dla pin, np. tak:
   pinMode(1,INPUT);digitalWrite(1,HIGH);
3) dla 5-ciu pojedynczych przycisków ( potrzeba 5 pinów cyfrowych)
   Uwaga dla tej wersji opcji musisz dopisać w funkcji setup konfigurację dla pinów, np. tak:
   pinMode(1,INPUT);digitalWrite(1,HIGH);
   pinMode(2,INPUT);digitalWrite(2,HIGH);
   pinMode(3,INPUT);digitalWrite(3,HIGH); 
   pinMode(11,INPUT);digitalWrite(11,HIGH); 
   pinMode(12,INPUT);digitalWrite(12,HIGH); 
*/
// --- wersja dla klawiatury 5-cio przyciskowej DFRobot --------------------------------------
volatile int czytaj_1(int analog)
{
   int stan_Analog = analogRead(analog);delay(30);//Serial.println(stan_Analog); 
   if (stan_Analog > 1000) return -1; // dla wartosci poza zakresem
   if (stan_Analog < 50)   return 0;  // w prawo  
   if (stan_Analog < 200)  return 1;  // do gĂłry 
   if (stan_Analog < 400)  return 2;  // w dĂłĹ‚ 
   if (stan_Analog < 600)  return 3;  // w lewo  
   if (stan_Analog < 800)  return 4;  // OK 
   return -1;                         // nic nie wcisnieto
}
// --- wersja dla joysticka (2 wejscia analogowe + pin cyfrowy -------------------------------
int czytaj_2(int poziom, int pion, int pinD)
{
// poziom - nr wejścia analogowego do którego podłączona jest manetka joysticka dla ruchu lewo-prawo
// pion   - nr wejścia analogowego do którego podłączona jest manetka joysticka dla ruchu góra-dół
// pinD   - nr pinu cyfrowego do którego podłączony jest przycisk OK w joysticku
int stan1= analogRead(pion); {delay(60);if(stan1>0)stan1=(stan1+50)/1024+1;}  
   int stan2= analogRead(poziom); {delay(60);if(stan2>0)stan2=(stan2+50)/1024+1;} 
   int stanD=digitalRead(pinD);
   if(stanD==LOW) return 4;           // OK 
   if(stan1==0) return 2;             // w dĂłĹ‚
   if(stan1==2) return 1;             // do gĂłry
   if(stan2==0) return 3;             // w lewo
   if(stan2==2) return 0;             // w prawo
   return -1;                         // nic nie wcisnieto
}
// --- wersja dla 5-ciu przycisków cyfrowych --------------------------------------------------
// dla przykładu jeśli wykorzystujesz piny: 1,2,3,11 i 12 to wołasz : czytaj_2(1,2,3,11,12)
int czytaj_3(int gora, int lewo, int ok, int prawo,int dol)
// gora   - nr pinu cyfrowego do którego podłączony jest przyciski góra
// lewo   - nr pinu cyfrowego do którego podłączony jest przyciski lewo
// ok     - nr pinu cyfrowego do którego podłączony jest przyciski OK
// prawo  - nr pinu cyfrowego do którego podłączony jest przyciski prawo
// dol    - nr pinu cyfrowego do którego podłączony jest przyciski dół
{
if(digitalRead(gora)==LOW) return 1;
if(digitalRead(lewo)==LOW) return 3;
if(digitalRead(ok)==LOW) return 4;
if(digitalRead(prawo)==LOW) return 0;
if(digitalRead(dol)==LOW) return 2;
return -1;
}
// ============================================================================================
// 
void setup()
{
  linia1=new char[16];  // zainicjowanie dynamicznego wskaźnika do tekstu 
  linia2=new char[16];  // to BARDZO WAŻNE, bo wskażnik dynamiczny musi wskazywać na 
                        // z góry określone miejsce w pamieci. Gdybyśmy tego nie zrobili
                        // to wcześniej czy później programik mógłby wskoczyć w nieokreślony 
                        //  bliżej obszar pamięci, co może skutkować nieodwracalnymi konsekwencjami
                        // łącznie z przestawieniem Fuse Bitów !!!  
                        // Proszę uważać na wszystkie dynamiczne wskaźniki, TAKA DOBRA RADA :-)
  Serial.begin(9600);   // inicjacja Seriala, głównie do testów 
  lcd.begin(20, 4);     // inicjacja LCD
  lcd.createChar(3,arrowLeft);    // tworzymy w pamięci LCD 5 własnych znaków dla strzałek
  lcd.createChar(4,arrowRight);
  lcd.createChar(5,arrowDown);
  lcd.createChar(6,arrowBack);
  lcd.createChar(7,arrowUpDown);
  /* tu przykładowe piny cyfrowe dla 3 wersji funkcji czytaj_3(1,2,3,11,12)
   pinMode(1,INPUT);digitalWrite(1,HIGH);
   pinMode(2,INPUT);digitalWrite(2,HIGH);
   pinMode(3,INPUT);digitalWrite(3,HIGH); 
   pinMode(11,INPUT);digitalWrite(11,HIGH); 
   pinMode(12,INPUT);digitalWrite(12,HIGH); 
  */ 
  pinMode(0,OUTPUT);digitalWrite(0,LOW); // do testów 
  menuSetup();          // funkcja klasy MenuBackend - tu tak naprawdę tworzymy nasze menu 
  menu.moveDown();      // idziemy do pierwszej opcji - PLIK, moveDown bo pierwotnie byliśmy w root
                        // to tak jak w Awatarze drzewa rosną korzeniami do góry :-)
}
// --- I nadszedł czas na neverending story :-) -------------------------------------------- 
void loop()    
{
  x=czytaj_1(15);delay(30);             // odczytujemy stan klawiatury:
  /*
  Ja używam funkcji czytaj_1() bo mam akurat klawiaturkę podpiętą pod A0
  Jeśli masz inna klawiaturkę to użyj funkcji czytaj_2 lub czytaj_3 - patrz opis
  Ponadto musisz pamietać że w funkcji obsługo klawisza OK - menuUseEvent(MenuUseEvent used)
  także musisz użyć odpowiedniej wersji funkcji czytaj !!!
  */
  if(zm!=x)                               // jesli była zmiana stanu to :
    {
      switch(x)                           // sprawdzamy co nacisnieto 
      {
      case 0: menu.moveRight();break;     // jesli naciśnięto klawisz w Prawo to przesuń menu w prawo
      case 1: menu.moveUp();break;        // menu do góry
      case 2: menu.moveDown();break;      // menu w dół
      case 3: menu.moveLeft();break;      // menu w lewo
      case 4: menu.use();break;           // wciśnięto OK więc skok do funkcji menuUseEvent(MenuUseEvend used)
                                          // to w tej funkcji właśnie obsługujemy nasze Menu, tu sprawdzamy
                                          // jaką opcję wybrano i tutaj tworzymy kod do obslugi zdarzenia.
      }
    } zm=x;                               // przypisanie zmiennej zm wartości x po to, aby dluższe wciskanie tego
                                          // samego klawisza nie powodowało ponownej generacji zdarzenia. 
                                          // program reaguje na zmianę stanu klawiatury. 
}
// === KONIEC ===========================================================

