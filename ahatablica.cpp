#include<stdlib.h>
#include <stdio.h>
#define kropka 46
#include<iostream>
#define BUFSIZE 1000
using namespace std;
struct symbol_zrodla
{
       int symbol;
       int czestosc;
}symbol_zrodla;

struct element_kodu
{
       int symbol;
       unsigned char slowo[4];
       int liczba_bitow;
};

struct galaz
{
       int ojciec;
       int potomek1;
       int potomek2;
};

struct galaz drzewo_kodowania[255];
struct element_kodu tablica_kodowania[512];
struct element_kodu tablica_kodu[256];
struct symbol_zrodla model_pelny[256];

int porownaj(const void *operand1,const void *operand2)
{
    const struct symbol_zrodla *pointer1 = (const struct symbol_zrodla*) operand1;
    const struct symbol_zrodla *pointer2 = (const struct symbol_zrodla*) operand2;
    if((pointer1->czestosc)==(pointer2->czestosc)) return 0;
    else if((pointer1->czestosc)<(pointer2->czestosc))return 1;
    else return -1;
}



int zrob_drzewo(struct symbol_zrodla model_zrodla[], struct galaz drzewo_kodowania[],int liczba_symboli_modelu, const char *nazwapliku_drzewo,const char *nazwapliku_mmod)
{
int liczba_symboli = liczba_symboli_modelu;
int licznik_symboli;
int licznik_symboli_dodanych = 0;
int n;


    licznik_symboli=liczba_symboli;
    for(int k=0; k<liczba_symboli-1; k++)
    {
     drzewo_kodowania[k].ojciec = 256+k;
     drzewo_kodowania[k].potomek1 = model_zrodla[liczba_symboli_modelu-1-k].symbol;
     drzewo_kodowania[k].potomek2 = model_zrodla[liczba_symboli_modelu-2-k].symbol;
     model_zrodla[liczba_symboli_modelu-2-k].symbol = 256+k;
     model_zrodla[liczba_symboli_modelu-2-k].czestosc = model_zrodla[liczba_symboli_modelu-1-k].czestosc + model_zrodla[liczba_symboli_modelu-2-k].czestosc;
     licznik_symboli--;
     qsort(model_zrodla,licznik_symboli,sizeof(struct symbol_zrodla), porownaj);
     licznik_symboli_dodanych++;
     }

    FILE *wskaznikpliku_mmod=NULL;
	wskaznikpliku_mmod=fopen(nazwapliku_mmod,"wb");

    if (wskaznikpliku_mmod == NULL)
	    {
		printf("Nie mozna utworzyc pliku: %s\n", nazwapliku_mmod);
		exit(EXIT_FAILURE);
	    }
 	else
        {
        for(int k=0; k<liczba_symboli_modelu; k++)
          {
          fprintf(wskaznikpliku_mmod, "%d %d\n ", model_zrodla[k].symbol, model_zrodla[k].czestosc);
          }
        }

    FILE *wskaznikpliku_drzewo=NULL;
	wskaznikpliku_drzewo=fopen(nazwapliku_drzewo,"wb");

    if (wskaznikpliku_mmod == NULL)
	    {
		printf("Nie mozna utworzyc pliku: \n", nazwapliku_drzewo);
		exit(EXIT_FAILURE);
	    }
 	else
        {
        for(int k=0; k<licznik_symboli_dodanych; k++)
          {
          fprintf(wskaznikpliku_drzewo, "%d %d %d\n ", drzewo_kodowania[k].ojciec, drzewo_kodowania[k].potomek1, drzewo_kodowania[k].potomek2 );
          }
        }

        fclose(wskaznikpliku_drzewo);
        fclose(wskaznikpliku_mmod);
        return (licznik_symboli_dodanych);
}



int daj_tablice_kodowania(struct galaz drzewo_kodowania[], struct element_kodu tablica_kodowania[],int liczba_symboli_modelu, const char *nazwa_tkodow)
{
    int symbol1,symbol2;
    int n, indeks_ojca, symbol_ojca;
    int licznik_drzewa=liczba_symboli_modelu-2;
    int licznik_kodu=0;
    int dlugosc_ciagu=0;
    int pozycja, ktory_bajt, ktory_bit;
    unsigned char bajt, bajtek[4], jedynka=1;

    for(int k=0; k<2*liczba_symboli_modelu; k++)
    {
        for(int i=0; i<4; i++)
            tablica_kodowania[k].slowo[i]=0;
        tablica_kodowania[k].symbol=0;
        tablica_kodowania[k].liczba_bitow=0;
    }

    tablica_kodowania[licznik_kodu].symbol=drzewo_kodowania[licznik_drzewa].ojciec;
    while(licznik_drzewa+1)
    {
        symbol_ojca=drzewo_kodowania[licznik_drzewa].ojciec;
        tablica_kodowania[licznik_kodu+1].symbol=drzewo_kodowania[licznik_drzewa].potomek1;
        tablica_kodowania[licznik_kodu+2].symbol=drzewo_kodowania[licznik_drzewa].potomek2;

        indeks_ojca=-1;
        for (int i=0; i<licznik_kodu+1; i++)
        {
            if ((tablica_kodowania[i].symbol)==(symbol_ojca))
            {
                indeks_ojca=i;
                break;
            }
        }
        if (indeks_ojca==-1)
        {
            printf("Blad tworzenie tablicy kodowania\n");
            exit(EXIT_FAILURE);
        }

        dlugosc_ciagu=tablica_kodowania[indeks_ojca].liczba_bitow;
        //Przepisanie ciagu ojca do potomkow
        for (int i=0; i<4; i++)
        {
            tablica_kodowania[licznik_kodu+1].slowo[i]=tablica_kodowania[indeks_ojca].slowo[i];
            tablica_kodowania[licznik_kodu+2].slowo[i]=tablica_kodowania[indeks_ojca].slowo[i];

        }

        pozycja=dlugosc_ciagu;
        ktory_bajt=pozycja/8;
        ktory_bit=pozycja%8;
        jedynka=1;
        jedynka=jedynka<<7-ktory_bit;

        bajt=tablica_kodowania[indeks_ojca].slowo[ktory_bajt];
        bajt=bajt|jedynka;//dopisanie jedynki

        tablica_kodowania[licznik_kodu+1].slowo[ktory_bajt]=bajt;
        tablica_kodowania[licznik_kodu+1].liczba_bitow=dlugosc_ciagu+1;
        tablica_kodowania[licznik_kodu+2].liczba_bitow=dlugosc_ciagu+1;
        licznik_kodu+=2;
        licznik_drzewa--;
    }
    for(int k=0; k<2*liczba_symboli_modelu-1; k++)
    {
        for(int i=0; i<4; i++)
            bajtek[i]=tablica_kodowania[k].slowo[i];
    }

    FILE *wskaznikpliku_tkodu=NULL;
    wskaznikpliku_tkodu=fopen(nazwa_tkodow,"wb");
    if (wskaznikpliku_tkodu == NULL)
    {
        printf("Nie mozna utworzyc pliku: %s \n", nazwa_tkodow);
        exit(EXIT_FAILURE);
    }
    else
    {
        for(int k=0; k<liczba_symboli_modelu; k++)
        {
            for(int i=0; i<4; i++)
                bajtek[i]=tablica_kodowania[k].slowo[i];
            fprintf(wskaznikpliku_tkodu, " %d %d  %d  %d %d %d \n", tablica_kodowania[k].symbol, bajtek[0],bajtek[1],bajtek[2],bajtek[3],tablica_kodowania[k].liczba_bitow);
        }
    }
    fclose(wskaznikpliku_tkodu);

    for(int k=0; k<liczba_symboli_modelu; k++)
    {
        for(int i=0; i<4; i++)
            bajtek[i]=tablica_kodowania[k].slowo[i];
    }

    return 0;
}


int daj_tablice_kodu(struct element_kodu tablica_kodowania[], struct element_kodu tablica_kodu[],int liczba_symboli_modelu, const char *nazwa_tkodu)
{
int symbol, licznik_kodu=0;
unsigned char bajtek[4];

     for (int k=0; k<2*liczba_symboli_modelu-1; k++)
     {
         symbol=tablica_kodowania[k].symbol;
         if (symbol<256)
         {
             tablica_kodu[licznik_kodu].symbol=symbol;
             tablica_kodu[licznik_kodu].liczba_bitow=tablica_kodowania[k].liczba_bitow;
             for (int i=0; i<4; i++)
                tablica_kodu[licznik_kodu].slowo[i]=tablica_kodowania[k].slowo[i];
             licznik_kodu++;
         }
     }

    FILE *wskaznikpliku_tkodu=NULL;
	wskaznikpliku_tkodu=fopen(nazwa_tkodu,"wb");
    if (wskaznikpliku_tkodu == NULL)
	{
		printf("Nie mozna utworzyc pliku: %s \n", nazwa_tkodu);
		exit(EXIT_FAILURE);
	}
	else
    {
     for(int k=0; k<liczba_symboli_modelu; k++)
        {
        for(int i=0; i<4; i++)
          bajtek[i]=tablica_kodu[k].slowo[i];
        fprintf(wskaznikpliku_tkodu, " %d %d  %d  %d %d %d \n", tablica_kodu[k].symbol, bajtek[0],bajtek[1],bajtek[2],bajtek[3],tablica_kodu[k].liczba_bitow);
        }
    }
    fclose(wskaznikpliku_tkodu);

     for(int k=0; k<liczba_symboli_modelu; k++)
        {
        for(int i=0; i<4; i++)
          bajtek[i]=tablica_kodu[k].slowo[i];
        }

    return 0;
}





void model(char* nazwa_in, int& lwz, int& liczba_symboli)
{

     int n=0,k=0,indeks=0;
     unsigned char bufor_wejscia[BUFSIZE];

     for(k=0; k<256; k++)
    {
     model_pelny[k].symbol=k;
     model_pelny[k].czestosc=0;
    }

    FILE *wskaznik_pliku=NULL;
	wskaznik_pliku=fopen(nazwa_in,"rb");

    if (wskaznik_pliku == NULL)
	{
		printf("Nie mozna otworzyc pliku: %s  \n", nazwa_in);
		exit(EXIT_FAILURE);
	}
	else
	{
	    lwz=0;
    printf( "Czytanie pliku %s do wyznaczenia modelu. \n", nazwa_in);
    while(n=fread(bufor_wejscia,sizeof(unsigned char),BUFSIZE,wskaznik_pliku))
    {
            for(int i=0; i<n; i++)
		  {
            indeks = bufor_wejscia[i];
            model_pelny[indeks].czestosc++;
          }
            lwz+=n;
    }
    printf("liczba wszystkich znakow w pliku: %d\n", lwz);
    }
    liczba_symboli=0;
    qsort(model_pelny,256,sizeof(symbol_zrodla), porownaj);
    liczba_symboli=0;
    while(model_pelny[liczba_symboli].czestosc!=0)
        liczba_symboli++;
    fclose(wskaznik_pliku);

}

int nazwa_pliku(char *nazwa_pliku, const char *rozszerzenie, char *nazwa_pliku_nowa)
{
    int i,n;
   	char *w_nazwy;

    for (n=0; n<24; n++)
       nazwa_pliku_nowa[n]=0;

    n=0;
    w_nazwy=nazwa_pliku;
    nazwa_pliku_nowa[n]=*w_nazwy;
    while ((nazwa_pliku_nowa[n]!=kropka)&(nazwa_pliku_nowa[n]!='\0'))
           {
           n++;
           w_nazwy++;
           nazwa_pliku_nowa[n]=*w_nazwy;
           }
    n++;
    i=0;
    nazwa_pliku_nowa[n]=rozszerzenie[i];
    while (rozszerzenie[i]!='\0')
           {
           n++;
           i++;
           nazwa_pliku_nowa[n]=rozszerzenie[i];
           }
    return 0;
}

int main()
{
    char *nazwa_in;
    char *nazwa_out;
    char *nazwa_po;
    int lwz=0;
    int liczba_symboli=0;
    char nazwaplikuin[24];
    char nazwaplikuout[24];
    char nazwaplikuout2[24];
    char nazwaplikuout3[24];
    char nazwaplikuout4[24];
    char nazwaplikuout5[24];
    char nazwaplikupo[24];
	printf("Podaj nazwe pliku:");

    cin.getline(nazwaplikuin, 24);
    nazwa_in=nazwaplikuin;
            
            
    model(nazwa_in,lwz,liczba_symboli);
    nazwa_pliku(nazwaplikuin,"drzewo",nazwaplikuout4);
    nazwa_pliku(nazwaplikuin,"modelSort",nazwaplikuout5);
    int liczba_symboli_dodanych=zrob_drzewo(model_pelny,drzewo_kodowania,liczba_symboli,nazwaplikuout4,nazwaplikuout5);
            
    nazwa_pliku(nazwaplikuin,"tabelaKoduFull",nazwaplikuout2);
    nazwa_pliku(nazwaplikuin,"tabelaKodu",nazwaplikuout3);
    daj_tablice_kodowania(drzewo_kodowania,tablica_kodowania,liczba_symboli,nazwaplikuout2);
    daj_tablice_kodu(tablica_kodowania,tablica_kodu,liczba_symboli,nazwaplikuout3);
          
        
      

}



