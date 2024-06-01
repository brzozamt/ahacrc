#include<stdlib.h>
#include <stdio.h>

#define SIZECRC 4
#define degreegen 32
#define kropka 46
#define BUFSIZE 4


int nowa_nazwa(char *nazwa_pliku, char *rozszerzenie, char *nazwa_pliku_nowa) 
{
    int i,n;
   	char *w_nazwy;
    
    for (n=0; n<24; n++)
       nazwa_pliku_nowa[n]=0;

    n=0;
    w_nazwy=nazwa_pliku;
    nazwa_pliku_nowa[0]=*w_nazwy;
    while ((nazwa_pliku_nowa[n]!=kropka)&&(nazwa_pliku_nowa[n]!='\0'))
           {
           n++;
           w_nazwy++;
           nazwa_pliku_nowa[n]=*w_nazwy;
           }
    n++;
    i=0;
    nazwa_pliku_nowa[n]=rozszerzenie[0];
    while (rozszerzenie[i]!='\0')
           {
           n++;
           i++;
           nazwa_pliku_nowa[n]=rozszerzenie[i];
           }         
    return 0;
}    


int dzielenie(char *nazwa_pliku_in, unsigned char reszta[], unsigned char wielomian[])
{
 unsigned char bufor_wejscia[BUFSIZE], kolejka[SIZECRC+BUFSIZE];   
 unsigned char start=0, koniec=0, maska=128, ostatni_bit, pierwszy_bit;
 int n,m;
 int licznik_bitow=0;
 int licznik_bajtow_in=0, licznik_buforu=0;
     
    for (int i=0; i<SIZECRC+BUFSIZE; i++)
       kolejka[i]=0;   
 
    FILE *wskaznik_pliku_in=NULL;
	wskaznik_pliku_in=fopen(nazwa_pliku_in,"rb"); 

    if (wskaznik_pliku_in == NULL)
	{
	  printf("Nie mozna otworzyc pliku: %s  \n", nazwa_pliku_in);
	  exit(EXIT_FAILURE);
	}

    while(n=fread(bufor_wejscia,sizeof(unsigned char),BUFSIZE,wskaznik_pliku_in)) 
    {
        
        for (int i=0; i<n; i++)
                  kolejka[SIZECRC+i]=bufor_wejscia[i];
        licznik_bitow=8*n;
        licznik_bajtow_in+=n;
                
        while (licznik_bitow)
        {
          licznik_buforu=1+(licznik_bitow-1)/8; 
          
          pierwszy_bit=maska&kolejka[0];    
         
          for (int i=0; i<SIZECRC+licznik_buforu; i++)
               {
                 kolejka[i]=(kolejka[i]<<1);
                 if (i<(SIZECRC+licznik_buforu-1))
                    ostatni_bit=(kolejka[i+1])>>7;
                 else
                    ostatni_bit=0;   
                 kolejka[i]=kolejka[i]|ostatni_bit; 
               
               }
          if (pierwszy_bit!=0)  //jesli pierwszy bit jest jedynka
               for(int i=0; i<SIZECRC; i++)
                  {
                    kolejka[i]=kolejka[i]^wielomian[i];          
                   
                  }     
          licznik_bitow--;  
                   
        }                     
    }   
    
    
    licznik_bitow=degreegen;
      
        while (licznik_bitow>0)
        {
         
          pierwszy_bit=maska&kolejka[0];    
         
          for (int i=0; i<SIZECRC; i++)
               {
                 kolejka[i]=(kolejka[i]<<1);
                 ostatni_bit=kolejka[i+1]>>7;
                 kolejka[i]=kolejka[i]|ostatni_bit; 
               }  
               
          if (pierwszy_bit!=0)  //jesli pierwszy bit jest jedynka                              
                 for(int i=0; i<SIZECRC; i++)
                 {
                    kolejka[i]=kolejka[i]^wielomian[i];          
                    
                 }     
          licznik_bitow--;       
   
        }
 
 for(int i=0; i<SIZECRC; i++)
      reszta[i]=kolejka[i];   
             
 fclose(wskaznik_pliku_in);
                 
 return licznik_bajtow_in;   

}

int nowa_nazwa_zCRC(char *nazwa_pliku, char *suffix, char *nazwa_pliku_nowa) 
{
    int n = 0;
    int len = 0;
    int base_len = 0;
    int dot_pos = -1;

    for (n = 0; n < 24; n++)
        nazwa_pliku_nowa[n] = 0;

    while (nazwa_pliku[len] != '\0') 
    {
        if (nazwa_pliku[len] == '.') 
        {
            dot_pos = len;
        }
        len++;
    }

    if (dot_pos == -1) 
    {
        base_len = len;
    } 
    else 
    {
        base_len = dot_pos;
    }

    for (n = 0; n < base_len; n++)
    {
        nazwa_pliku_nowa[n] = nazwa_pliku[n];
    }

    
    int i = 0;
    while (suffix[i] != '\0')
    {
        nazwa_pliku_nowa[base_len + i] = suffix[i];
        i++;
    }

    if (dot_pos != -1) 
    {
        int j = dot_pos;
        while (nazwa_pliku[j] != '\0')
        {
            nazwa_pliku_nowa[base_len + i + (j - dot_pos)] = nazwa_pliku[j];
            j++;
        }
    }

    return 0;
}


int main()
{
    unsigned char wielomian[SIZECRC], reszta[SIZECRC];
    char n_plik[24];
    char wartoscCRC[] = "CRC";
    char dodaneCRC[] = "_zCRC";
    char n_plik_CRC[24], n_plik_zCRC[24];
    int n, licznik = 0;
    unsigned char bajt_out;
    unsigned char bufor_wejscia[BUFSIZE];

    wielomian[0] = 0x04;
    wielomian[1] = 0xc1;
    wielomian[2] = 0x1d;
    wielomian[3] = 0xb7;

    for (int i = 0; i < SIZECRC; i++)
    {
        printf("Wartosc %d. bajtu wielomianu to: %02x\n", i + 1, wielomian[i]);
    }

    for (int i = 0; i < 24; i++)
    {
        n_plik[i] = 0;
    }
    printf("Podaj nazwe pliku, dla ktorego wyznaczana jest CRC:\n");
    scanf("%s", n_plik);

    printf("Nazwa przetwarzanego pliku to: %s\n", n_plik);

    n = dzielenie(n_plik, reszta, wielomian);
    printf("Z pliku %s odczytano %d bajtow.\n", n_plik, n);

    // Zapisanie wartosci CRC w odrebnym pliku   
    n = nowa_nazwa(n_plik, wartoscCRC, n_plik_CRC);
    FILE *wskaznik_pliku_vCRC = NULL;
    wskaznik_pliku_vCRC = fopen(n_plik_CRC, "wb");

    if (wskaznik_pliku_vCRC == NULL)
    {
        printf("Nie mozna otworzyc pliku: %s \n", n_plik_CRC);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < SIZECRC; i++)
    {
        bajt_out = reszta[i];
        printf("Bajt %d wartosci CRC: %#x\n", i, reszta[i]);
        fwrite(&bajt_out, sizeof(unsigned char), 1, wskaznik_pliku_vCRC);
    }

    fclose(wskaznik_pliku_vCRC);
    printf("Wartosc CRC zostala zapisana do pliku %s\n", n_plik_CRC);

    // Dopisywanie do pliku zrodlowego
    FILE *wskaznik_pliku_in = NULL;
    wskaznik_pliku_in = fopen(n_plik, "rb");

    if (wskaznik_pliku_in == NULL)
    {
        printf("Nie mozna otworzyc pliku: %s  \n", n_plik);
        exit(EXIT_FAILURE);
    }

    n = nowa_nazwa_zCRC(n_plik, dodaneCRC, n_plik_zCRC);

    FILE *wskaznik_pliku_zCRC = NULL;
    wskaznik_pliku_zCRC = fopen(n_plik_zCRC, "wb");

    if (wskaznik_pliku_zCRC == NULL)
    {
        printf("Nie mozna otworzyc pliku: %s \n", n_plik_zCRC);
        exit(EXIT_FAILURE);
    }
    // Przepisywanie pliku
    licznik = 0;
    while (n = fread(bufor_wejscia, sizeof(unsigned char), BUFSIZE, wskaznik_pliku_in))
    {
        for (int i = 0; i < n; i++)
        {
            bajt_out = bufor_wejscia[i];
            fwrite(&bajt_out, sizeof(unsigned char), 1, wskaznik_pliku_zCRC);
        }
        licznik += n;
    }
    printf("Przepisano %d bajtow do pliku %s\n", licznik, n_plik_zCRC);

    fclose(wskaznik_pliku_in);
    // Dopisanie CRC

    for (int i = 0; i < SIZECRC; i++)
    {
        bajt_out = reszta[i];
        fwrite(&bajt_out, sizeof(unsigned char), 1, wskaznik_pliku_zCRC);
    }
    fclose(wskaznik_pliku_zCRC);
    printf("Dopisano %d bajtow CRC do pliku %s\n", SIZECRC, n_plik_zCRC);

    // Sprawdzenie CRC pliku z dopisana wartoscia (kodem) CRC

    printf("Sprawdzenie wartosci CRC dla pliku %s\n", n_plik_zCRC);
    n = dzielenie(n_plik_zCRC, reszta, wielomian);
    printf("Z pliku %s odczytano %d bajtow.\n", n_plik_zCRC, n);

    return 0;
}

