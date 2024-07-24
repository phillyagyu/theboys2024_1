#include <stdio.h>
/* demais includes */
#include "conjunto.h"
#include "fila.h"
#include "lef.h"

/* funcoes que voce ache necessarias aqui */
struct mundo_t {
    unsigned int NHerois;
    struct heroi_t Herois[10];
    unsigned int NBases;
    struct base_t Bases[5];
    unsigned int NMissoes;
    struct missao_t Missoes[50];
    unsigned int NHabilidades;
    unsigned int TamanhoMundo;
    unsigned long Relogio;
}

struct heroi_t {
    unsigned int id;
    struct conjunto hab;
    unsigned int pac;
    unsigned int vel;
    unsigned int exp;
    unsigned int base_id;
};

struct base_t {
    unsigned int id;
    unsigned int lot;
    struct conjunto pres;
    struct fila esp;
    unsigned int x, y;
};

struct missao_t {
    unsigned int id;
    struct conjunto hab;
    unsigned int x, y;
};

unsigned int main (){
    /* declaracoes de variaveis aqui */
    struct mundo_t Mundo;

    srand (0); /* use zero, nao faca com time (0) */

    /* coloque seu codigo aqui */

    /* init das estruturas de dados do mundo */
    Herois[0].id = 0;
    Herois[0].hab = cria_cjt (rand());
    Herois[0].pac = rand();
    Herois[0].exp = 0;
    Herois[0].base_id = rand();

    Bases[0].id = 0;
    Bases[0].lot = rand();
    Bases[0].pres = cria_cjt (rand());
    Bases[0].esp = fila_cria();
    Bases[0].x = rand();
    Bases[0].y = rand();

    Missoes[0].id = 0;
    Missoes[0].hab = cria_cjt (rand());
    Missoes[0].x = rand();
    Missoes[0].y = rand();


    return 0;
}
