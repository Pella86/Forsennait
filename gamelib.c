#include "gamelib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*******************************************************************************
* GIOCATORI
*******************************************************************************/
static giocatore_t Ciccio;
static giocatore_t Ninja;

/*******************************************************************************
* Funzioni di formattazione
*******************************************************************************/
#define CARATTERI_CONSOLE 80

static char evento_buffer[CARATTERI_CONSOLE];

void stampa_carattere(int volte, char* c){
    int i;
    for(i = 0; i < volte; i++) printf(c);
}

void stampa_spazi(int volte){
    stampa_carattere(volte, " ");
}

void stampa_prompt(){
    printf(">>");
}

void stampa_titolo(char* titolo){
    stampa_carattere(CARATTERI_CONSOLE - 1, "*");
    printf("\n");

    // calcola quanti spazi per centrare
    int ltitolo = strlen(titolo);
    int spazi = CARATTERI_CONSOLE / 2 - ltitolo / 2;

    stampa_spazi(spazi);
    printf("%s\n", titolo);
    stampa_carattere(CARATTERI_CONSOLE - 1, "*");
    printf("\n");
}

void stampa_sottotitolo(char* titolo){
    // calcola quanti spazi per centrare
    int ltitolo = strlen(titolo) + 2;
    int spazi_prima = CARATTERI_CONSOLE / 2 - ltitolo / 2;
    int spazi_dopo = CARATTERI_CONSOLE - spazi_prima - ltitolo - 1;

    stampa_carattere(spazi_prima, "-");
    printf(" %s ", titolo);
    stampa_carattere(spazi_dopo, "-");
    printf("\n");
}

void stampa_centrato(char* titolo){
    // calcola quanti spazi per centrare
    int ltitolo = strlen(titolo) + 2;
    int spazi_prima = CARATTERI_CONSOLE / 2 - ltitolo / 2;

    stampa_spazi(spazi_prima);
    printf(" %s \n", titolo);
}

void stampa_prompt_centrato(char* titolo){
    // calcola quanti spazi per centrare
    int ltitolo = strlen(titolo);
    int spazi_prima = (CARATTERI_CONSOLE / 2) - (ltitolo / 2);

    stampa_spazi(spazi_prima);
    printf("%s\n", titolo);

    stampa_spazi(spazi_prima);
    stampa_prompt();
}

// funzione stampa un titoletto centrato e ritorna il numero di spazi per
// allineare sotto il titoletto le differenti opzioni
int stampa_titolo_menu(char* titolo){
    int ltitolo = strlen(titolo);

    // calcola quanti spazi per centrare
    int spazi_prima = CARATTERI_CONSOLE / 2 - ltitolo / 2;

    stampa_spazi(spazi_prima);
    printf("%s\n", titolo);

    // crea sottolineatura
    stampa_spazi(spazi_prima);
    int i;
    for(i = spazi_prima; i < (spazi_prima + ltitolo); i++) printf("-");
    printf("\n");

    return spazi_prima;
}

void stampa_scelta_menu(int allineamento, char* titolo){
    stampa_spazi(allineamento);
    printf("%s\n", titolo);
}

void stampa_riga(size_t n){
    int i;
    for(i = 0; i < n; i++){
        printf("\n");
    }
}

/*******************************************************************************
* HELPERS
*******************************************************************************/

// funzioni helper
void clean_input(){
    while(getchar() != '\n');
}

int get_random_prob(){
    return rand() % 100;
}


/*******************************************************************************
* SCACCHIERA
*******************************************************************************/

#define SCACCHIERA_DEBUG 0

// variabili globali scacchiera
static cella_t* scacchiera = NULL;
static size_t dim_lato;
static size_t dim_lato_partenza;

static bool scacchiera_creata = false;

// funzione che restituisce le celle dati gli indici i e j
cella_t* get_cell(int i, int j){
    return &scacchiera[j*dim_lato + i];
}

// controlla se inizializzata
bool scacchiera_inizzializzata(){
    if(scacchiera_creata){
        return true;
    }
    else{
        stampa_centrato("ERRORE: scacchiera non inizializzata");
        return false;
    }
}

/// Funzioni di stampa della scacchiera

// stampa la scacchiera visualizzando tutti gli oggetti e giocatori
void stampa_scacchiera_debug(){
    if(!scacchiera_inizzializzata()){
       return;
    }

    int i, j;
    for(j = 0; j < dim_lato; j++){
        for(i = 0; i < dim_lato; i++){
            cella_t* cella = get_cell(i, j);

            char ciccio_char = (i == Ciccio.x && j == Ciccio.y)? 'C' : '-';
            char ninja_char  = (i == Ninja.x  && j == Ninja.y )? 'N' : '-';

            printf("|p%c_%c_%c_o%d|",
                        cella->pericolo,
                        ciccio_char,
                        ninja_char,
                        cella->oggetto);
        }
        printf("\n");
    }
}

void stampa_scacchiera_gioco(){
    if(!scacchiera_inizzializzata()){
       return;
    }

    size_t riga, col;
    char* normal_tile = "|     ";
    size_t tile_size = strlen(normal_tile);
    int n_col = dim_lato * tile_size; // larghezza scacchiera in caratteri
    int allineamento = CARATTERI_CONSOLE / 2 - n_col / 2;

    for(riga = 0; riga < dim_lato; riga++){

        // stampa spazi per centrare la console
        stampa_spazi(allineamento);
        stampa_carattere(n_col, "-");
        printf("-\n");

        stampa_spazi(allineamento);
        for(col = 0; col < n_col; col++){printf("%c", (col % tile_size == 0)? '|' : ' ');}
        printf("|\n");

        stampa_spazi(allineamento);
        // stampa il giocatore e l'oggetto
        for(col = 0; col < dim_lato; col++){

            bool ciccio_presente = false;
            bool ninja_presente =  false;

            if(Ciccio.y == riga && Ciccio.x == col){
                ciccio_presente = true;
            }

            if(Ninja.y == riga && Ninja.x == col){
                ninja_presente = true;
            }

            if(ciccio_presente && ninja_presente){
                printf("| CxN ");
            }
            else if(ciccio_presente){
                printf("|  C  ");
            }
            else if(ninja_presente){
                printf("|  N  ");
            }
            else{
                printf(normal_tile);
            }
        }
        printf("|\n");

        stampa_spazi(allineamento);

        for(col = 0; col < dim_lato; col++){
            cella_t* cella = get_cell(col, riga);
            if(cella->pericolo == p_ALIENO){
                switch(cella->oggetto){
                    case o_NESSUN_OGGETTO:
                        printf("|.....");
                        break;
                    case o_MEDIKIT:
                        printf("|.MED.");
                        break;
                    case o_POZIONE:
                        printf("|.POZ.");
                        break;
                    case o_MATERIALE:
                        printf("|.MAT.");
                        break;
                    case o_COLPI_LANCIARAZZI:
                        printf("|RAZZI");
                        break;
                }
            }
            else{
                printf("|.....");
            }
        }
        printf("|\n");
    }

    stampa_spazi(allineamento);
    stampa_carattere(n_col, "-");
    printf("-\n");
}

void stampa_scacchiera(){
    if(SCACCHIERA_DEBUG){
        stampa_scacchiera_debug();
    }
    else{
        stampa_scacchiera_gioco();
    }
}

/// Salvataggio probabilità per ricreare la scacchiera

static const size_t num_pericoli = 3;
static int prob_pericoli[3]; // n = 3

enum prob_pericoli_indici{
    prob_nessun_pericolo = 0,
    prob_trappole  = 1,
    prob_alieni = 2
};

// funzione di conversione dal indice di prob_pericoli all'enum Tipo_pericolo
enum Tipo_pericolo get_pericolo_from_idx(int index){
    switch(index){
        case 1:
            return p_TRAPPOLA;
        case 2:
            return p_ALIENO;
        default:
            return p_NESSUNO;
    }
};

static const size_t num_oggetti = 5;
static int prob_oggetti[5]; // n = 5

enum prob_oggetti_indici{
    prob_nessun_oggetto = 0,
    prob_medekit = 1,
    prob_pozione = 2,
    prob_materiale = 3,
    prob_colpi = 4
};

// funzione di conversione dall'indice di prob_oggetti all enum Tipo_oggetto
enum Tipo_oggetto get_oggetto_from_idx(int index){
    switch(index){
        case 1:
            return o_MEDIKIT;
        case 2:
            return o_POZIONE;
        case 3:
            return o_MATERIALE;
        case 4:
            return o_COLPI_LANCIARAZZI;
        default:
            return o_NESSUN_OGGETTO;
    }
};

/// Crea range probabilità

// dato un array di probabilita crea dei range di probabilita
// probabilta nessun pericolo start = 0,  end = 10 (prob nessuno = 10)
// probabilita trappola       start = 10, end = 50 (prob trappola = 40)
// ...
void crea_prob_range(int* prob_start, int* prob_end, int* prob_arr, size_t dim){
    int iprob;
    int somma_probabilita = 0;

    for(iprob = 0; iprob < dim; iprob++){
        prob_start[iprob] = somma_probabilita;
        somma_probabilita += prob_arr[iprob];
        prob_end[iprob] = somma_probabilita;
    }
}


/// Funzione per chiedere le probabilta

int chiedi_probabilita(char* prob_nome){

    int prob;
    do{
        sprintf(evento_buffer, "%s %s", "Inserisci la probabilita di trovare", prob_nome);
        stampa_prompt_centrato(evento_buffer);
        scanf("%d", &prob);
        clean_input();

        if(prob >= 0 && prob <= 100){
            sprintf(evento_buffer, "probilita %u%% ok", prob);
            stampa_centrato(evento_buffer);
            break;
        }
        else{
            stampa_centrato("ERRORE: probabilita fuori range");
            stampa_centrato("inserire un numero da 0 a 100");
        }

    } while(true);

    return prob;
}


/// Termina creazione scacchiera

// termina la creazione
// la funzione restituisce vero o falso a dipendendenza della scelta dell'utente
// l'utente può continuare la creazione semplicemente rispondendo Si o con <return>
bool termina_creazione(){
    stampa_prompt_centrato("Vuoi continuare la creazione? <return>: continua| N, n: termina");
    bool esci = false;
    char c = 0;

    // cerca una n o una N nello stream
    while(c != '\n' && c != '\r'){
        if(c == 'n' || c == 'N'){
            esci = true;
            break;
        }
        c = getchar();
    }

    return esci;
}


/// Funzione che crea la scacchiera dati i paramateri sopra

void param_crea_scacchiera(){

    // inizializza scacchiera
    scacchiera = (cella_t*) malloc(dim_lato * dim_lato * sizeof(cella_t));
    if(scacchiera == NULL){
        stampa_centrato("ERRORE: memoria insufficente, scacchiera non allocata.");
        return;
    }

    // inizializza i range
    int prob_start_pericoli[num_pericoli];
    int prob_end_pericoli[num_pericoli];

    crea_prob_range(prob_start_pericoli, prob_end_pericoli, prob_pericoli, num_pericoli);

    int prob_start_oggetti[num_oggetti];
    int prob_end_oggetti[num_oggetti];

    crea_prob_range(prob_start_oggetti, prob_end_oggetti, prob_oggetti, num_oggetti);

    // crea la scacchiera con le probabilità date
    int i, j, iprob, rnd;
    for(i = 0; i < dim_lato; i++){
        for(j = 0; j < dim_lato; j++){
            // scegli la cella
            cella_t* cella = get_cell(i, j);

            //  assegna pericolo
            rnd = get_random_prob(); // numero che decide la probabilita
            for(iprob = 0; iprob < num_pericoli; iprob++){
                if(rnd >= prob_start_pericoli[iprob] && rnd <= prob_end_pericoli[iprob]){
                    cella->pericolo = get_pericolo_from_idx(iprob);
                }
            }

            // assegna oggetto
            rnd = get_random_prob(); // numero che decide la probabilita
             for(iprob = 0; iprob < num_oggetti; iprob++){
                if(rnd >= prob_start_oggetti[iprob] && rnd <= prob_end_oggetti[iprob]){
                    cella->oggetto = get_oggetto_from_idx(iprob);
                }
            }
        }
    }

    // cambia la flag di inizializzazione
    scacchiera_creata = true;
}

/// Crea scacchiera utente
void crea_schacchiera(){

    scacchiera_creata = false;

    //-------------------- crea la scacchiera ----------------------------------
    stampa_titolo("CREAZIONE MAPPA");
    stampa_sottotitolo("Scegli grandezza mappa");

    do{
        // resetta il valore del lato
        dim_lato = 0;

        stampa_prompt_centrato("Quanto vuoi grande l'aera di gioco?");
        scanf("%u", &dim_lato);
        clean_input();

        if(dim_lato > 0){
            if(dim_lato > 13){
                printf("ATTENZIONE: con un numero di celle cosi' grande la scacchiera superera i 79 caratteri della console.");
            }
            break;
        }
        else{
            printf("ERRORE: La scacchiera deve avere una grandezza minima di 1\n");
        }

    }while(true);

    // conferma l'utente della creazione
    sprintf(evento_buffer, "La scacchiera avrà dimensioni %ux%u\n", dim_lato, dim_lato);
    stampa_centrato(evento_buffer);

    // assegna il lato al lato di partenza
    dim_lato_partenza = dim_lato;

    stampa_riga(1);
    if(termina_creazione()) return;

    //---------------- crea probabilita PERICOLI  ------------------------------
    stampa_sottotitolo("Probabilita' pericoli");

    do{
        prob_pericoli[prob_nessun_pericolo] = chiedi_probabilita("Nessun pericolo");

        prob_pericoli[prob_trappole] = chiedi_probabilita("Trappola");

        prob_pericoli[prob_alieni] = chiedi_probabilita("Alieni");

        int somma = prob_pericoli[prob_nessun_pericolo] +
                        prob_pericoli[prob_trappole] +
                        prob_pericoli[prob_alieni];

        // se la somma è 100% allora esci dal loop
        if(somma == 100){
            break;
        }
        else{
            printf("ERRORE: La somma delle probabilita' dei PERICOLI e' diversa da 100\n");
            if(termina_creazione()) return;
        }
    }while(true);

    stampa_centrato("Probabilita' per i pericoli scelte:");

    sprintf(evento_buffer, "Nessun pericolo: %u%% Trappole: %u%% Alieni: %u%%",
                prob_pericoli[prob_nessun_pericolo],
                prob_pericoli[prob_trappole],
                prob_pericoli[prob_alieni]);
    stampa_centrato(evento_buffer);

    stampa_riga(1);
    if(termina_creazione()) return;

    //------------------ crea probabilita OGGETTI ------------------------------
    stampa_sottotitolo("Probabilita' oggetti");

    do{
        prob_oggetti[prob_nessun_oggetto] = chiedi_probabilita("Nessun oggetto");
        prob_oggetti[prob_medekit] = chiedi_probabilita("Medikit");
        prob_oggetti[prob_pozione] = chiedi_probabilita("Pozione");
        prob_oggetti[prob_materiale] = chiedi_probabilita("Materiale");
        prob_oggetti[prob_colpi] = chiedi_probabilita("Lanciarazzi");

        int somma = 0;
        size_t idx;
        for(idx = 0; idx < num_oggetti; idx++)
            somma += prob_oggetti[idx];

        // se la somma è 100% allora esci dal loop
        if(somma == 100){
            break;
        }
        else{
            printf("ERRORE: La somma delle probabilita' degli OGGETTI e' diversa da 100\n");
            if(termina_creazione()) return;
        }

    }while(true);

    stampa_centrato("Probabilita' per gli oggetti scelte");
    sprintf(evento_buffer, "Nessun oggetto: %u%% medikit: %u%% pozione: %u%% materiale: %u%% lanciarazzi: %u%%\n",
                prob_oggetti[prob_nessun_oggetto],
                prob_oggetti[prob_medekit],
                prob_oggetti[prob_pozione],
                prob_oggetti[prob_materiale],
                prob_oggetti[prob_colpi]);
    stampa_centrato(evento_buffer);

    //------------- alloca e crea la scacchiera -------------------------------
    param_crea_scacchiera();

    stampa_scacchiera();
    stampa_sottotitolo("Scacchiera creata con successo");
}

/// Inizializza la scacchiera con parametri standard

// Questa funzione viene chiamata all'inizio per avere una scacchiera
// gia creata
void auto_scacchiera(){

    dim_lato = 5;
    dim_lato_partenza = dim_lato;

    prob_pericoli[prob_nessun_pericolo] = 20;
    prob_pericoli[prob_trappole] = 10;
    prob_pericoli[prob_alieni] = 70;

    prob_oggetti[prob_nessun_oggetto] = 10;
    prob_oggetti[prob_medekit] = 10;
    prob_oggetti[prob_pozione] = 10;
    prob_oggetti[prob_materiale] = 40;
    prob_oggetti[prob_colpi] = 30;

    param_crea_scacchiera();
}

/// libera scacchiera

void free_scacchiera(){
    free(scacchiera);
}

/// dimezza la scacchiera

void scacchiera_dimezza(){
    stampa_sottotitolo("Dimezza mappa");
    sprintf(evento_buffer, "Dimensione attuale %d\n", dim_lato);
    stampa_centrato(evento_buffer);
    free_scacchiera();

    dim_lato = dim_lato / 2;
    sprintf(evento_buffer, "Dimensione dimezzata %d\n", dim_lato);
    stampa_centrato(evento_buffer);
    param_crea_scacchiera();
}

/// ripristina la scacchiera

void scacchiera_ripristina(){
    stampa_sottotitolo("Ripristina mappa");
    free_scacchiera();
    dim_lato = dim_lato_partenza;
    param_crea_scacchiera();
}




/*******************************************************************************
* MUOVI GIOCATORE
*******************************************************************************/

// funzioni helper per muovere il giocatore
void muovi_nord (giocatore_t* g) {g->y -= 1;}
void muovi_sud  (giocatore_t* g) {g->y += 1;}
void muovi_ovest(giocatore_t* g) {g->x -= 1;}
void muovi_est  (giocatore_t* g) {g->x += 1;}

void muovi(giocatore_t* g){
    // cambia le coordinaze x y del giocatore
    //                            N      S      O     E
    bool mosse_possibili[]   = {false, false, false, false};
    char mosse_possibili_c[] = {'n',     's',   'o',   'e'};

    // controlla quali direzioni sono possibili
    if(g -> y - 1 >= 0)      {mosse_possibili[0] = true;}
    if(g -> y + 1 < dim_lato){mosse_possibili[1] = true;}
    if(g -> x - 1 >= 0)      {mosse_possibili[2] = true;}
    if(g -> x + 1 < dim_lato){mosse_possibili[3] = true;}

    // mosse possibili
    stampa_titolo_menu("Hai la possibilità di muoverti in queste direzioni:");


    sprintf(evento_buffer, "-------");
    stampa_centrato(evento_buffer);

    sprintf(evento_buffer, "|  %c  |",  (mosse_possibili[0])? 'N' : 'X');
    stampa_centrato(evento_buffer);

    sprintf(evento_buffer, "|%c + %c|", (mosse_possibili[2])? 'O' : 'X',
                                        (mosse_possibili[3])? 'E' : 'X');
    stampa_centrato(evento_buffer);

    sprintf(evento_buffer, "|  %c  |",  (mosse_possibili[1])? 'S' : 'X');
    stampa_centrato(evento_buffer);

    sprintf(evento_buffer, "-------");
    stampa_centrato(evento_buffer);

    stampa_centrato("N: Nord, S: Sud, O: Ovest, E: Est, X: non valida");

    // scegli una mossa
    char mossa_scelta;
    bool mossa_valida = false;
    int i;
    do{
        sprintf(evento_buffer, "Scegli una mossa %s", g->nome);
        stampa_prompt_centrato(evento_buffer);

        scanf("%c", &mossa_scelta);
        clean_input();
        mossa_scelta = tolower(mossa_scelta);

        // se la mossa non è tra i caratteri
        for(i = 0; i < 4; i++){
            if(mosse_possibili[i] == true && mosse_possibili_c[i] == mossa_scelta){
                stampa_centrato("La mossa e' valida");
                mossa_valida = true;
                break;
            }
        }

        if(!mossa_valida){printf("ERRORE: mossa non valida.\n");}

    }while(!mossa_valida);

    // esegui la mossa valida
    // questo if è relativamente inutile ma bom...
    switch(mossa_scelta){
        case 'n':
            muovi_nord(g);
            break;
        case 's':
            muovi_sud(g);
            break;
        case 'o':
            muovi_ovest(g);
            break;
        case 'e':
            muovi_est(g);
            break;
    }
}

/*******************************************************************************
* FUNZIONI DI STAMPA
*******************************************************************************/

void stampa_giocatore(giocatore_t g){
    sprintf(evento_buffer, "Statistiche %s", g.nome);
    stampa_sottotitolo(evento_buffer);

    // stampa salute
    stampa_centrato("Salute");

    switch(g.stato){
        case g_SCUDO_VITA:
            stampa_centrato("|SCUDO VITA|");
            break;

        case g_SOLO_SCUDO:
            stampa_centrato("|SCUDO ----|");
            break;

        case g_SOLO_VITA:
            stampa_centrato("|----- VITA|");
            break;

        case g_VULNERABILE:
            stampa_centrato("|----- ----|");
            break;
    }

    // stampa lo zaino
    stampa_centrato("Zaino");

    char zaino_label[] =   "Medikit|Pozioni|Materiale|Lanciarazzi";
    sprintf(evento_buffer, "   %d   |   %d   |    %d    |     %d     ",
               g.zaino[o_MEDIKIT],
               g.zaino[o_POZIONE],
               g.zaino[o_MATERIALE],
               g.zaino[o_COLPI_LANCIARAZZI]);

    stampa_centrato(zaino_label);
    stampa_centrato(evento_buffer);

    // stampa gli alieni uccisi
    sprintf(evento_buffer, "Alieni uccisi: %d", g.alien_kill);
    stampa_centrato(evento_buffer);
    stampa_sottotitolo("--");
}



/*******************************************************************************
* CONTROLLO DEL GIOCO
*******************************************************************************/

void svuota_zaino(int* zaino){
    int i;
    for(i = 0; i < 4; i++){
        zaino[i] = 0;
    }
}

typedef enum StatoGioco{
    sg_continua = 1,
    sg_trappola = 2,
    sg_ucciso = 3,
    sg_scontro_finale = 4,
    sg_uccide = 5
} stato_gioco_t;

bool termina_temp_gioco(){
    stampa_prompt_centrato("Continua a giocare? <enter> per continuare | n, N per uscire");
    bool esci = false;
    char c = 0;

    // cerca una n o una N nello stream
    while(c != '\n' && c != '\r'){

        if(c == 'n' || c == 'N'){
            esci = true;
            break;
        }
        c = getchar();
    }

    return esci;
}

// funzione che controlla di chi è il turno
bool is_turno_ciccio(unsigned int turni){
    if(turni % 2 == 0){
        return true;
    }
    else{
        return false;
    }
}

// funzione che assegna la posizione a caso sulla scacchiera
void assegna_posizione(giocatore_t* g){
    g->x = rand() % dim_lato;
    g->y = rand() % dim_lato;
}

/*******************************************************************************
* INVENTARIO OGGETTO
*******************************************************************************/

void stampa_nessun_oggetto(){
    stampa_centrato("x");
}

void stampa_pozione(){
    stampa_centrato(" _ ");
    stampa_centrato(" | ");
    stampa_centrato("/_\\");
}

void stampa_medikit(){
    stampa_centrato("   _   ");
    stampa_centrato(" _| |_ ");
    stampa_centrato("|_   _|");
    stampa_centrato("  |_|  ");
    stampa_centrato("       ");
}

void stampa_materiale(){
    stampa_centrato("_____");
    stampa_centrato("_|_|_");
    stampa_centrato("_____");
}

void stampa_razzo(){
    stampa_centrato(" .^.");
    stampa_centrato(" |+|");
    stampa_centrato(" |+|");
    stampa_centrato("/ V \\");
}

// stampa oggetto, modo P e la stringa del nome, modo O e il disegno dell ogg
void stampa_oggetto(enum Tipo_oggetto ogg, char mode){
    switch(ogg){
        case o_NESSUN_OGGETTO:
            if(mode == 'P') printf("Nessun oggetto");
            else{stampa_nessun_oggetto();}
            break;

        case o_MEDIKIT:
            if(mode == 'P')printf("Medekit");
            else{stampa_medikit();}
            break;

        case o_POZIONE:
            if(mode == 'P')printf("Pozione");
            else{stampa_pozione();}
            break;

        case o_MATERIALE:
            if(mode == 'P')printf("Materiale");
            else{stampa_materiale();}
            break;

        case o_COLPI_LANCIARAZZI:
            if(mode == 'P') printf("Colpi lanciarazzi");
            else{stampa_razzo();}
            break;
    }
}

typedef enum zaino_mode{
    zaino_scacchiera = 2,
    zaino_scontro_finale = 4
}zaino_mode_t;

bool zaino_vuoto(int* zaino, zaino_mode_t mode){
    int iogg;
    for(iogg = 0; iogg < mode; iogg++){
        if(zaino[iogg] != 0){
            return false;
        }
    }
    return true;
}

oggetto_t scegli_oggetto(giocatore_t* g, zaino_mode_t mode){
    // stampa gli oggetti disponibili
    int spazi = stampa_titolo_menu("Scegli oggetto");
    int i;
    for(i = 0; i < mode; i++){
        stampa_spazi(spazi);
        printf("%d. ", i);
        stampa_oggetto(i, 'P'); // modo P e la stringa del nome
        printf(" quantita': %d", g->zaino[i]);
        printf("\n");
    }

    // chiedi di scegliere l'oggetto
    oggetto_t oggetto_scelto;
    do{
        stampa_prompt();
        int scelta;
        scanf("%d", &scelta);
        oggetto_scelto = (oggetto_t) scelta;
        clean_input();

        if(oggetto_scelto >= 0 && oggetto_scelto < (oggetto_t) mode){
            printf("Hai scelto l'oggetto:\n");
            stampa_oggetto(oggetto_scelto, 'P'); printf("\n");
            stampa_oggetto(oggetto_scelto, 'O');
            break;
        }
    }while(true);

    return oggetto_scelto;
};

void usa_medekit(giocatore_t* g){
    switch(g->stato){
        case g_VULNERABILE:
            g->stato = g_SOLO_VITA;
            printf("Hai recuperato vita\n");
            break;
        case g_SOLO_SCUDO:
            g->stato = g_SCUDO_VITA;
            printf("Hai recuperato vita\n");
            break;
        default:
            printf("Hai sprecato l'oggetto\n");
            break;
    }
}

void usa_pozione(giocatore_t* g){
    switch(g->stato){
        case g_VULNERABILE:
            g->stato = g_SOLO_SCUDO;
            printf("Hai recuperato scudo\n");
            break;
        case g_SOLO_VITA:
            g->stato = g_SCUDO_VITA;
            printf("Hai recuperato scudo\n");
            break;
        default:
            printf("Hai sprecato l'oggetto\n");
            break;
    }
}

void usa_oggetto(giocatore_t* g){
    stampa_riga(2);

    stampa_centrato("Apri lo zaino...");
    stampa_riga(1);

    if(zaino_vuoto(g->zaino, zaino_scacchiera)){
        stampa_centrato("Zaino vuoto, salta il turno.");
        return;
    }

    oggetto_t ogggetto = scegli_oggetto(g, zaino_scacchiera);

    int contentuto = g->zaino[ogggetto];

    if(contentuto > 0){
        g->zaino[ogggetto] -= 1;
        if(ogggetto == o_MEDIKIT){
            usa_medekit(g);
        }
        else if(ogggetto == o_POZIONE){
            usa_pozione(g);
        }
    }
}

/*******************************************************************************
* COMBATTI ALIENO
*******************************************************************************/

void stampa_alieno(){
    stampa_centrato(" \\___/");
    stampa_centrato(" /V V\\");
    stampa_centrato(" \\ - /");
    stampa_centrato(" /m m\\");
}

stato_gioco_t combatti(giocatore_t* g){
    stampa_centrato("Hai inconrato un alieno, combatti e...");
    stampa_alieno();

    stampa_centrato("L'alieno sferra un colpo micidiale");
    int prob_colpito = get_random_prob();

    sprintf(evento_buffer, "Analisi probabilita'... %d%%...\n", prob_colpito);
    stampa_centrato(evento_buffer);

    if(prob_colpito < 35){
        stampa_centrato("Il colpo si avvicina...");
        // instantly killed
        if(g->stato == g_VULNERABILE){
            printf("e uccide %s.\n", g->nome);
            // giocatore muore
            return sg_ucciso;
        }

        // if is not killed it gets hit
        int prob_vitascudo = get_random_prob();
        bool colpito = false;

        // se va a segno il colpo sullo scudo
        if(prob_vitascudo > 35){
            // toglie scudo
            switch(g->stato){
                case g_SCUDO_VITA:
                    sprintf(evento_buffer, "Colpisce %s, che perde lo scudo, ma tiene la vita", g->nome);
                    g->stato = g_SOLO_VITA;
                    colpito = true;
                    break;

                case g_SOLO_SCUDO:
                    sprintf(evento_buffer, "Colpisce %s, che resta vulnerabile.", g->nome);
                    g->stato = g_VULNERABILE;
                    colpito = true;
                    break;
                case g_SOLO_VITA:
                    break;

                default:
                    printf("PROGRAM ERROR line %d in function %s: this shouldnt happen\n", __LINE__, __FUNCTION__);
            }
        }
        // atrementi è la vita a rimetterci
        else{
            // toglie vita
            switch(g->stato){
                case g_SCUDO_VITA:
                    sprintf(evento_buffer, "Colpisce %s, che perde vita, ma tiene lo scudo", g->nome);
                    g->stato = g_SOLO_SCUDO;
                    colpito = true;
                    break;

                case g_SOLO_VITA:
                    sprintf(evento_buffer, "Colpisce %s, che resta vulnerabile.", g->nome);
                    g->stato = g_VULNERABILE;
                    colpito = true;
                    break;

                case g_SOLO_SCUDO:
                    break;

                default:
                    printf("PROGRAM ERROR line %d in function %s: this shouldnt happen\n", __LINE__, __FUNCTION__);
            }
        }
        stampa_centrato(evento_buffer);


        if(!colpito){
            stampa_centrato("L'alieno manca il coplo");
        }
    }
    else{
        stampa_centrato("Il giocatore schiva il colpo");
    }

    stampa_riga(1);

    // togli il pericolo dalla cella e raccogli oggetto

    cella_t* cella = get_cell(g->x, g->y);
    cella->pericolo = p_NESSUNO;

    enum Tipo_oggetto ogg = cella->oggetto;

    if(ogg != o_NESSUN_OGGETTO){
        stampa_titolo_menu("L'alieno faceva da guardia a un' oggetto");
        stampa_centrato("Il giocatore guadagna guadagnato un");
        stampa_spazi(25);
        stampa_oggetto(ogg, 'P'); printf("\n");
        stampa_oggetto(ogg, 'O');
        stampa_centrato("Verra' aggiunto allo zaino del giocatore (max 3)");
        if(g->zaino[ogg] <= 2){
            g->zaino[ogg]++;
        }
    }

    // azzera la cella
    cella->oggetto = o_NESSUN_OGGETTO;

    // uccide l'alieno
    stampa_centrato("Il giocatore uccide l'alieno (+1 kill)");
    g->alien_kill++;

    stampa_giocatore(*g);

    return sg_continua;
}

/*******************************************************************************
* TURNO
*******************************************************************************/

enum MuoviUsa{
    s_muovi = 1,
    s_usa = 2
};

stato_gioco_t turno(giocatore_t* g){

    sprintf(evento_buffer, "TURNO GIOCATORE %s", g->nome);
    stampa_titolo(evento_buffer);
    stampa_giocatore(*g);

    stampa_scacchiera();

    // scegli muovi o usa oggetto
    int scelta;
    do{
        int spazi = stampa_titolo_menu("Cosa vuoi fare?");
        stampa_scelta_menu(spazi, "1. Muovi");
        stampa_scelta_menu(spazi, "2. Usa oggetto");
        stampa_prompt();

        scanf("%d", &scelta);
        clean_input();

        if(scelta == s_muovi || scelta == s_usa){
            break;

        }else{
            printf("ERRORE: la scelta deve essere tra 1 e 2\n");
        }

    }while(true);

    // scelta muovi
    if(scelta == s_muovi){
        // stampa la scacchiera
        stampa_scacchiera();

        // muovi il giocatore
        muovi(g);

        if(Ciccio.x == Ninja.x && Ciccio.y == Ninja.y){
            printf("%s si prepara allo scontro finale\n", g->nome);
            return sg_scontro_finale;
        }

        // controlla la cella
        cella_t* cella = get_cell(g->x, g->y);

        // se il giocatore è su una trappola muore
        if(cella->pericolo == p_TRAPPOLA){
            stampa_centrato("Incontra una trappola e muore...");
            // la funzione ritorna nella funzione gioca()
            return sg_trappola;
        }

        if(cella->pericolo == p_ALIENO){
            stampa_riga(2);
            stampa_centrato("Incontra una alieno e...");
            do{
                stampa_prompt_centrato("Combatti l'alieno? (Y/N)");
                char scelta_combattere;
                scanf("%c", &scelta_combattere);
                clean_input();

                // se sceglie di combattere
                if(scelta_combattere == 'Y'){
                    return combatti(g);
                    break;
                }
                // se sceglie di fuggire
                else if(scelta_combattere == 'N'){
                    printf("%s fugge la scena\n", g->nome);
                    return sg_continua;
                    break;
                }
                else{
                    printf("ERRRORE: la scelta deve essere Yes(Y) o No(N)\n");
                }
            }while(true);

        }
    }
    // usa oggetto
    else{
        usa_oggetto(g);
    }

    return sg_continua;

}

/*******************************************************************************
* TURNI SCONTRO FINALE
*******************************************************************************/

/// inserisci nuovo piano nella torre

void inserisci(piano_t* root){
    int forza = rand() % 20;
    unsigned piano_n = 0;
    char evento_buf[CARATTERI_CONSOLE];
    sprintf(evento_buf, "Nuovo piano creato con forza: %d", forza);
    stampa_centrato(evento_buf);

    piano_t* current = root;
    while(current->prossimo != NULL){
        current = current->prossimo;
        ++piano_n;
    }
    piano_n += 1;

    sprintf(evento_buf, "Piano inserito al piano %u\n", piano_n);
    stampa_centrato(evento_buf);

    piano_t** nuovo_piano = &current->prossimo;

    (*nuovo_piano) = (piano_t*) malloc(sizeof(piano_t));
    (*nuovo_piano)->n_piano = piano_n;
    (*nuovo_piano)->forza = forza;
    (*nuovo_piano)->prossimo = NULL;
    (*nuovo_piano)->precedente = current;
}

/// calcola l'altezza della torre

int altezza_torre(piano_t* root){
    int piani = 0;
    piano_t* current = root;
    while(current->prossimo != NULL){
        current = current->prossimo;
        piani++;
    }
    return piani;
}

/// calcola la foza della torre

int forza_torre(piano_t* root){
    int somma_forze = 20;
    piano_t* current = root;
    while(current != NULL){
        somma_forze += current->forza;
        current = current->prossimo;
    }
    return somma_forze;
}

/// costruisci la torre dati i materiali di un giocatore

void costruisci_torre(giocatore_t* g){
    // inizializza la torre usando i materiali nello zain
    stampa_titolo_menu(g->nome);

    g->torre = (piano_t*) malloc(sizeof(piano_t));
    g->torre->n_piano = 0;
    g->torre->forza = 0;
    g->torre->prossimo = NULL;
    g->torre->precedente = NULL;

    // costruisci torri
    stampa_centrato("Costruzione torre");
    int i;
    for(i = 1; i <= g->zaino[o_MATERIALE]; i++){
        inserisci(g->torre);
    }
    stampa_riga(1);
}

/// rimuovi l'ultimo piano della torre

void rimuovi(piano_t* root){

    piano_t* penultimo = root;
    // cerca il penultimo elemento
    while(penultimo->prossimo->prossimo != NULL){
        penultimo = penultimo->prossimo;
    }

    free(penultimo->prossimo);
    penultimo->prossimo = NULL;
}

/// libera la torre

void free_torre(piano_t* root){
    // empty the list from the top
    piano_t* piano_corrente = root;
    while(piano_corrente != NULL){
        root = piano_corrente->prossimo;
        free(piano_corrente);
        piano_corrente = NULL;
        piano_corrente = root;
    }
}

/// inizializza le torri

void inizializza_torri(){
    // inizializza le due torri usando i materiali nello zaino
    costruisci_torre(&Ciccio);
    costruisci_torre(&Ninja);

    // I materiali sono usati quindi messi a 0
    Ciccio.zaino[o_MATERIALE] = 0;
    Ninja.zaino[o_MATERIALE] = 0;
}

/// ritorna un piano specifico della torre

piano_t* get_piano_at(piano_t* root, int index){
    piano_t* current = root;
    int count = 1;
    while(current != NULL && count <= index){
        count++;
        current = current->prossimo;
    }
    return current;
}

/// stampa le torri

void stampa_torri(giocatore_t* attaccante, giocatore_t* difensore){
    stampa_sottotitolo("Torri");
    int torri_max = 5;
    int i;

    if(strcmp(attaccante->nome, "Ciccio") == 0){
        sprintf(evento_buffer, "-----%s-- | ----%s----", attaccante->nome, difensore->nome);
        stampa_centrato(evento_buffer);
    }
    else{
        sprintf(evento_buffer, "-----%s--- | ----%s---", attaccante->nome, difensore->nome);
        stampa_centrato(evento_buffer);
    }

    for(i = torri_max; i >= 1; i--){
        piano_t* p_att = get_piano_at(attaccante->torre, i);
        piano_t* p_dif = get_piano_at(difensore->torre, i);

        if(p_att && p_dif){
            sprintf(evento_buffer, "## %2u | %2d ## | ## %2u | %2d ##",
                        p_att->n_piano,
                        p_att->forza,
                        p_dif->n_piano,
                        p_dif->forza);
            stampa_centrato(evento_buffer);
        }
        else if(p_att){
            sprintf(evento_buffer, "## %2u | %2d ## |              ", p_att->n_piano, p_att->forza);
            stampa_centrato(evento_buffer);
        }
        else if(p_dif){
            sprintf(evento_buffer, "              | ## %2u | %2d ##", p_dif->n_piano, p_dif->forza);
            stampa_centrato(evento_buffer);
        }
        else{
            //printf("piano not existing\n");
        }
    }
}

static bool init_torri = true; // flag per inizializzare le torri

// enum per le varie scelte del giocatore
typedef enum scelta_scontro_finale{
    scelta_colpisci = 1,
    scelta_usa_oggetto = 2,
    scelta_scava = 3
} scelta_scontro_finale_t;


/// La logica dietro i turni finali (attacco, usa oggetto, scava)

stato_gioco_t turni_finali(giocatore_t* attaccante, giocatore_t* difensore){

    sprintf(evento_buffer, "Attaccante %s x Difensore %s\n", attaccante->nome, difensore->nome);
    stampa_centrato(evento_buffer);
    stampa_giocatore(*attaccante);

    // the function prints inverted... needs revision
    stampa_torri(attaccante, difensore);

    // usa oggetto o colpisci
    scelta_scontro_finale_t scelta;
    do{
        int spazi = stampa_titolo_menu("Scegli un azione");
        stampa_scelta_menu(spazi, "1. Colpisci");
        stampa_scelta_menu(spazi, "2. Usa oggetto");
        stampa_scelta_menu(spazi, "3. Scava");
        stampa_prompt();

        int iscelta;
        scanf("%d", &iscelta);
        scelta = (scelta_scontro_finale_t) iscelta;
        clean_input();

        if(scelta >= scelta_colpisci && scelta <= scelta_scava){
            break;
        }
        else{
            printf("ERRORE: La scelta deve essere tra 1 e 3\n");
        }

    }while(true);

    if(scelta == scelta_colpisci){

        // placeholders per le stringhe che disegnano i giocatori
        char colpito_str[4][22] = {{'\0'}};

        strcpy(colpito_str[0], "  XXX                ");
        strcpy(colpito_str[1], "(0 U 0) /  ((((()))) ");
        strcpy(colpito_str[2], "-HHHHH-/  ((colpito))");
        strcpy(colpito_str[3], "  | |      ((((()))) ");

        char mancato_str[4][22] = {{'\0'}};

        strcpy(mancato_str[0], "  XXX                ");
        strcpy(mancato_str[1], "(0 . 0) /  --------- ");
        strcpy(mancato_str[2], "-HHHHH-/  --mancato--");
        strcpy(mancato_str[3], "  | |      --------- ");

        // colpisci
        // determina probabilità di colpire
        int prob_successo_attacco = forza_torre(attaccante->torre);
        sprintf(evento_buffer, "L'attaccante ha una probabilita di attacco di %d%%", prob_successo_attacco);
        stampa_centrato(evento_buffer);

        // determina la probabilita di schivare
        int prob_difesa = rand() % 50;
        sprintf(evento_buffer, "Il difensore ha una probabilita di schivare di %d%%", prob_difesa);
        stampa_centrato(evento_buffer);

        // se l'attaccante colpisce
        if(prob_difesa <= prob_successo_attacco){
            // se viene colpito perde lo scudo
            int idx;
            for(idx = 0; idx < 4; idx++) stampa_centrato(colpito_str[idx]);
            switch(difensore->stato){
                case g_SCUDO_VITA:
                    difensore->stato = g_SOLO_VITA;
                    break;

                case g_SOLO_SCUDO:
                case g_SOLO_VITA:
                    difensore->stato = g_VULNERABILE;
                    break;

                case g_VULNERABILE:
                    stampa_centrato("L'attacco ha ucciso il difensore");
                    return sg_uccide;
            }
        }
        else{
            int idx;
            for(idx = 0; idx < 4; idx++) stampa_centrato(mancato_str[idx]);
            stampa_centrato("Il difensore sopravvive\n");
        }
        stampa_giocatore(*difensore);
    }
    else if (scelta == scelta_usa_oggetto){

        // scegli l'oggetto
        oggetto_t oggetto = scegli_oggetto(attaccante, zaino_scontro_finale);

        if(attaccante->zaino[oggetto] > 0){
            attaccante->zaino[oggetto] -= 1;

            switch(oggetto){
                // se sceglie Medekit rida vita
                case o_MEDIKIT:
                    usa_medekit(attaccante);
                    break;

                // se sceglie Pozione rida scudo
                case o_POZIONE:
                    usa_pozione(attaccante);
                    break;

                // se sceglie materiale aggiunge un piano
                case o_MATERIALE:
                    if(altezza_torre(attaccante->torre) < 5){
                        printf("Hai aggiunto un piano alla torre\n");
                        inserisci(attaccante->torre);
                    }
                    else{
                        stampa_centrato("Torre ad altezza massima -> hai sprecato il turno");
                    }
                    break;

                // se sceglie lanciarazzi il colpo distrugge un piano
                case o_COLPI_LANCIARAZZI:
                    if(altezza_torre(difensore->torre) >= 1){
                        stampa_centrato("Distrutto un piano della torre");
                        rimuovi(difensore->torre);
                    }
                    else{
                        stampa_centrato("Torre difensore già al minimo -> hai sprecato il turno");
                    }
                    break;
                default:
                    printf("ERRORE PROGRAMMA: oggetto non trovato in scontro finale USA OGGETTO\n");
            }
        }
        else{
            stampa_centrato("Non hai quell oggetto, salta il turno");
        }
    }
    else if (scelta == scelta_scava){
        // scava e hai la possibilita di guadagnare un oggetto
        // stessse probabilità che sulla mappa

        oggetto_t oggetto = (oggetto_t) rand() % num_oggetti;

        if(oggetto == o_NESSUN_OGGETTO){
            stampa_centrato("Non hai trovato oggetti");
        }
        else{
            stampa_centrato("Hai trovato un:");
            stampa_spazi(35);
            stampa_oggetto(oggetto, 'P'); printf("\n");
            stampa_oggetto(oggetto, 'O');

            if(attaccante->zaino[oggetto] < 3){
                attaccante->zaino[oggetto]++;
            }
            else{
                stampa_centrato("Ma il tuo zaino non ha posto");
                stampa_riga(1);
            }
        }
    }
    return sg_continua;
}


/*******************************************************************************
* FUNZIONE GIOCA
*******************************************************************************/

void stampa_statistiche(int partite, int vittorie_ciccio, int vittorie_ninja){
    stampa_sottotitolo("Statistiche gioco");

    sprintf(evento_buffer, "Partite giocate %d", partite);
    stampa_centrato(evento_buffer);

    stampa_riga(1);

    sprintf(evento_buffer, "Vittorie di Ciccio %d", vittorie_ciccio);
    stampa_centrato(evento_buffer);

    sprintf(evento_buffer, "Alieni uccisi %d", Ciccio.alien_kill);
    stampa_centrato(evento_buffer);

    stampa_riga(1);

    sprintf(evento_buffer, "Vittorie di Ninja %d" , vittorie_ninja);
    stampa_centrato(evento_buffer);

    sprintf(evento_buffer, "Alieni uccisi %d", Ninja.alien_kill);
    stampa_centrato(evento_buffer);
}

void gioca(){
    if(!scacchiera_inizzializzata()){
        // torna alla funzione main
        return;
    }

    // assegna i nomi
    strcpy(Ciccio.nome, "Ciccio");
    strcpy(Ninja.nome, "Ninja");

    // asssgna le variabili di gioco della funzione
    bool cont_gioco = true; // flag per indicare quando tornare al main menu
    unsigned int cturni; // contatore turni
    unsigned int partite = 0; // contatore partite

    // contatore vittore
    unsigned int vittorie_ciccio = 0;
    unsigned int vittorie_ninja = 0;

    // se è necessario inizializzare il gioco
    bool init_gioco = true;

    // divide scacchiera
    int turni_divisione = dim_lato * 2;

    // modalita scontro finale
    bool modalita_scontro_finale;

    while(cont_gioco){

        if(init_gioco){
            stampa_titolo("---------- INIZIO PARTITA ---------");

            // ripristina la scacchiera
            scacchiera_ripristina();
            stampa_riga(1);

            // ristora vita e svuota zaino
            Ninja.stato = g_SCUDO_VITA;
            assegna_posizione(&Ninja);
            svuota_zaino(Ninja.zaino);

            Ciccio.stato = g_SCUDO_VITA;
            assegna_posizione(&Ciccio);
            svuota_zaino(Ciccio.zaino);

            // sceglie il turno casualmente
            cturni = rand() % 2;

            // modalita scontro finale
            modalita_scontro_finale = false;

            // for debug
            //modalita_scontro_finale = true;

            init_torri = true;

            // aumenta le partite
            partite++;

            // annulla l'init
            init_gioco = false;
        }

        // sceglie il giocatore
        giocatore_t* g;
        g = (is_turno_ciccio(cturni))? &Ciccio : &Ninja;

        // controlla se i giocatori sono sulla stessa casella
        if(Ciccio.x == Ninja.x && Ciccio.y == Ninja.y){
            modalita_scontro_finale = true;
        }

        // gioca il turno
        stato_gioco_t stato;

        if(modalita_scontro_finale){
            stampa_titolo("SCONTRO FINALE");

            if(init_torri){
                free_torre(Ciccio.torre);
                free_torre(Ninja.torre);

                inizializza_torri();
                // torri inizializzate
                init_torri = false;
            }

            // scegli attaccante e difensore
            giocatore_t* attaccante = (is_turno_ciccio(cturni))? &Ciccio : &Ninja;
            giocatore_t* difensore = (is_turno_ciccio(cturni))? &Ninja : &Ciccio;

            // procedi con l'attacco
            stato = turni_finali(attaccante, difensore);
        }
        else{
            stato = turno(g);
        }

        // analizza il risultato del turno
        if(stato == sg_scontro_finale){
            modalita_scontro_finale = true;
        }

        // se il giocatore muore
        if(stato == sg_trappola || stato == sg_ucciso){

            printf("Il giocatore %s\n", g->nome);

            // scegli la frase per la morte appropriata
            if(stato == sg_trappola){
                stampa_sottotitolo("Ha perso per via di una trappola");
            }

            if(stato == sg_ucciso && !modalita_scontro_finale){
                stampa_sottotitolo("Ha perso perche' un alieno lo ha ucciso");
            }

            if(stato == sg_ucciso && modalita_scontro_finale){
                stampa_sottotitolo("Ha perso perche' un giocatore lo ha ucciso");
            }

            // assegna i punti a dipendenza di chi è morto in questo turno
            // muore ciccio vittoria_ninja++
            if(is_turno_ciccio(cturni)){
                stampa_centrato("Ninja vince");
                vittorie_ninja++;
            }
            else{
                stampa_centrato("Ciccio vince");
                vittorie_ciccio++;
            }

            init_gioco = true;

            stampa_statistiche(partite, vittorie_ciccio, vittorie_ninja);
        }

        if(stato == sg_uccide){
            stampa_centrato("Il giocatore ha ucciso l'avversario");
            if(is_turno_ciccio(cturni)){
                stampa_titolo("Ciccio vince");
                vittorie_ciccio++;
            }
            else{
                stampa_titolo("Ninja vince");
                vittorie_ninja++;
            }
            init_gioco = true;
            stampa_statistiche(partite, vittorie_ciccio, vittorie_ninja);
        }

        if(!modalita_scontro_finale && cturni != 0 && cturni % turni_divisione == 0 && dim_lato > 1){
            stampa_titolo("LA MAPPA SI DIMEZZA");
            printf("Sono passati %d turni\n", turni_divisione);
            scacchiera_dimezza();
            assegna_posizione(&Ninja);
            assegna_posizione(&Ciccio);
            turni_divisione = turni_divisione / 2;
            printf("Ora il campo da gioco e' %dx%d\n", dim_lato, dim_lato);
        }


        cturni += 1;

        if(termina_temp_gioco()) {
            stampa_sottotitolo("Gioco terminato");
            free_torre(Ciccio.torre);
            free_torre(Ninja.torre);
            break;
        }
    }
}








