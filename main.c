#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "gamelib.h"

char titolo[] = "FORSENNAIT";

int main()
{
    // inizializza rand
    time_t t;
    srand((unsigned) time(&t));

    // inizializza automaticamente una scacchiera
    auto_scacchiera();

    // stampa il titolo
    stampa_titolo(titolo);

    // variabili controllo menu
    int scelta_principale;
    int stai_in_menu = true;

    do{
        int spazi = stampa_titolo_menu("Menu principale");
        stampa_scelta_menu(spazi, "1. Crea Mappa");
        stampa_scelta_menu(spazi, "2. Gioca");
        stampa_scelta_menu(spazi, "3. Esci dal gioco");
        stampa_prompt();

        scanf("%d", &scelta_principale);
        while(getchar()!= '\n');

        switch(scelta_principale){

            case 1:
                crea_schacchiera();
                break;

            case 2:
                gioca();
                break;

            case 3:
                stampa_titolo_menu("Esci dal Gioco\n");
                stai_in_menu = false;
                break;

            default:
                stampa_centrato("ERRORE: scelta non disponibile");
                stampa_centrato("scegli un numero tra 1 e 3");
                break;
        }

    }while(stai_in_menu);

    free_scacchiera();

    return 0;
}
