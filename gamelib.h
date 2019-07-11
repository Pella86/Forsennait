#ifndef GAMELIB_H_INCLUDED
#define GAMELIB_H_INCLUDED

#define NOME_GIOCATORE_MAX_SIZE 10

// funzioni necessarie al main
void crea_schacchiera(); // utente crea scacchiera
void auto_scacchiera(); // creazione automatica scacchiera
void free_scacchiera();
void gioca(); // funzione di gioco

// funzioni di formattazione
void stampa_titolo(char* titolo);
void stampa_sottotitolo(char* titolo);
int stampa_titolo_menu(char* titolo);
void stampa_scelta_menu(int allineamento, char* titolo);
void stampa_prompt();
void stampa_centrato(char* titolo);

// boolean
typedef enum { false, true } bool;

// enum pericolo
enum Tipo_pericolo{
    p_NESSUNO  = 'x',
    p_TRAPPOLA = 'T',
    p_ALIENO   = 'A'
};

// enum stato giocatore
enum Stato_giocatore{
    g_VULNERABILE = 0x20,
    g_SCUDO_VITA = 0x21,
    g_SOLO_VITA = 0x22,
    g_SOLO_SCUDO = 0x23
};

// enum oggetto
// non cambiare viene usato come indice per l'array zaino
typedef enum Tipo_oggetto{
    o_MEDIKIT = 0,
    o_POZIONE = 1,
    o_MATERIALE = 2,
    o_COLPI_LANCIARAZZI = 3,
    o_NESSUN_OGGETTO = 4
} oggetto_t;

// struttura della torre per lo scontro finale
typedef struct Piano{
    unsigned int n_piano;
    int forza;
    struct Piano* prossimo;
    struct Piano* precedente;
} piano_t;

// struttura giocatore
typedef struct Giocatore{
    char nome[NOME_GIOCATORE_MAX_SIZE];
    enum Stato_giocatore stato;
    // posizione
    int x; int y;
    int zaino[4];
    int alien_kill;
    piano_t* torre;
} giocatore_t;

// cella della scacchiera
typedef struct Cella{
    enum Tipo_pericolo pericolo;
    enum Tipo_oggetto oggetto;
} cella_t;

#endif // GAMELIB_H_INCLUDED
