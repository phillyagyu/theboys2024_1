#include <stdio.h>
#include <stdlib.h>
/* demais includes */
#include "conjunto.h"
#include "fila.h"
#include "lef.h"


#define T_INICIO 		0
#define T_FIM_DO_MUNDO 		365
#define N_TAMANHO_MUNDO 	100
#define N_HABILIDADES 		10
#define N_HEROIS 		N_HABILIDADES * 5
#define N_BASES 		N_HEROIS / 6
#define N_MISSOES 		T_FIM_DO_MUNDO / 100


/* funcoes que voce ache necessarias aqui */
struct heroi_t {
    int id;
    struct conjunto *hab;
    int pac;
    int vel;
    int exp;
    int base;
};

struct base_t {
    int id;
    int lot;
    struct conjunto *pres;
    struct fila *esp;
    int x, y;
};

struct missao_t {
    int id;
    struct conjunto *hab;
    int x, y;
};

struct mundo_t {
    int NHerois;
    struct heroi_t Herois[N_HEROIS];
    int NBases;
    struct base_t Bases[N_BASES];
    int NMissoes;
    struct missao_t Missoes[N_MISSOES];
    int NHabilidades;
    int TamanhoMundo;
    int Relogio;
};

/* Retorna inteiro aleatorio entre min e max, inclusos */
int aleat (int piso, int teto) {
	return (rand() % (teto - piso + 1)) + piso; 
}

/* CHEGA (T, H, B) - 	TIPO [ 0 ] */
void chega (int t, struct heroi_t *h, struct base_t *b, struct lef_t *Tempo) {

	int espera, tem_vaga, eh_vazia;

	struct evento_t *NovoEvento;

	/* ponteiros auxiliares */
	struct conjunto *c;
	struct fila *f;

	/* atualiza base de h */
	h->base = b->id;

	/* fila de espera */
	f = b->esp;

	/* conjunto de presentes */
	c = b->pres;

	printf ("%6d: CHEGA  HEROI %2d BASE %d\n", t, h->id, b->id);


	tem_vaga = ((c->card) < (c->max));  
	eh_vazia = fila_vazia (f);	

	if (tem_vaga && eh_vazia)
		espera = 1;
	else
		espera = ((h->pac) > (10 * f->tamanho));


	if (espera) {
		/* insere ESPERA */
		NovoEvento = cria_evento (t, 1, h->id, h->base);
		insere_lef (Tempo, NovoEvento);
	}
	else {
		/* insere DESITE */
		NovoEvento = cria_evento (t, 2, h->id, h->base);
		insere_lef (Tempo, NovoEvento);
	}	
}

/* ESPERA (T, H, B) - 	TIPO [ 1 ] */
void espera (int t, struct heroi_t *h, struct base_t *b, struct lef_t *Tempo) {
	
	struct evento_t *NovoEvento;

	/* ponteiro auxiliar */
	struct fila *f;

	/* fila de espera */
	f = b->esp;

	printf ("%6d: ESPERA HEROI %2d BASE %d\n", t, h->id, b->id);

	/* adiciona H ao fim da fila de espera de B */
	enqueue (f, h->id);

	/* insere AVISA */
	NovoEvento = cria_evento (t, 3, h->id, h->base);
	insere_lef (Tempo, NovoEvento);

}


/* DESISTE (T, H, B) - 	TIPO [ 2 ] */
void desiste (int t, struct heroi_t *h, struct base_t *b, struct lef_t *Tempo) {
	
	int base_aleat;
	
	struct evento_t *NovoEvento;

	printf ("%6d: DESIST HEROI %2d BASE %d\n", t, h->id, b->id);
	
	/* escolhe uma base destino D aleatória */
	base_aleat = aleat (0, N_BASES - 1);

	/* insere VIAJA */
	NovoEvento = cria_evento (t, 6, h->id, base_aleat);
	insere_lef (Tempo, NovoEvento);
	
}

/* AVISA (T, B) - 		TIPO [ 3 ] */
void avisa (int t, struct base_t *b, struct lef_t *Tempo) {

	int heroi_entrando;

	struct evento_t *NovoEvento;
	
	/* ponteiros auxiliares */
	struct conjunto *c;
        struct fila *f;
        
	/* fila de espera */
        f = b->esp;

	/* conjunto de presentes */
	c = b->pres;

	/* enquanto houver vaga em B e houver heróis esperando na fila */
	while ((c->card < c->max) && (!fila_vazia (f))) {

		printf ("%6d: AVISA  PORTEIRO BASE %d (%2d/%2d) FILA ", t, b->id, c->card, c->max);
		imprime_fila (f);	
		dequeue (f, &heroi_entrando);
		
		printf ("%6d: AVISA  PORTEIRO BASE %d ADMITE %2d\n", t, b->id, heroi_entrando);
		insere_cjt (c, heroi_entrando);
		
		/* insere ENTRA */
		NovoEvento = cria_evento (t, 4, heroi_entrando, b->id);
		insere_lef (Tempo, NovoEvento);
	}


}

/* ENTRA (T, H, B) - 	TIPO [ 4 ] */
void entra (int t, struct heroi_t *h, struct base_t *b, struct lef_t *Tempo) {

	int TPB;
	
	struct evento_t *NovoEvento;
	
	/* ponteiro auxiliar */
	struct conjunto *c;

	/* conjunto de presentes */
	c = b->pres;

	/* calcula TPB = tempo de permanência na base */
	TPB = 15 + (h->pac) * aleat (1, 20);

	printf ("%6d: ENTRA  HEROI %2d BASE %d (%2d/%2d) SAI %d\n", t, h->id, b->id, c->card, c->max, t + TPB);
	
	/* insere SAI */
	NovoEvento = cria_evento (t + TPB, 5, h->id, h->base);
	insere_lef (Tempo, NovoEvento);
	
}

/* SAI (T, H, B) - 		TIPO [ 5 ] */
void sai (int t, struct heroi_t *h, struct base_t *b, struct lef_t *Tempo) {

	int base_aleat;

        struct evento_t *NovoEvento;

	/* ponteiro auxiliar */
	struct conjunto *c;

	/* conjunto de presentes */
	c = b->pres;

	/* retira H do conjunto de heróis presentes em B */
	retira_cjt (c, h->id);

	printf ("%6d: SAI    HEROI %2d BASE %d (%2d/%2d)\n", t, h->id, b->id, c->card, c->max);

	/* escolhe uma base destino D aleatória */
        base_aleat = aleat (0, N_BASES - 1);


	/* insere VIAJA */
	NovoEvento = cria_evento (t, 6, h->id, base_aleat);
	insere_lef (Tempo, NovoEvento);

	/* insere AVISA */
        NovoEvento = cria_evento (t, 3, h->id, b->id);
        insere_lef (Tempo, NovoEvento);

}

/* VIAJA (T, H, D) - 	TIPO [ 6 ] */
void viaja (int t, struct heroi_t *h, struct base_t *d) {
	printf ("%6d: VIAJA  HEROI %2d BASE %d VEL %d\n", t, h->id, d->id, h->vel);
}


int main (){
	/* declaracoes de variaveis aqui */
    	struct mundo_t Mundo;

	struct lef_t *Tempo;
	struct evento_t *NovoEvento, *e;
	struct conjunto *TodasHab; 
	struct heroi_t *h;
	struct base_t *b;
	struct missao_t *m;

	int i;

    	srand (0); /* use zero, nao faca com time (0) */

    	/* ESTADO INICIAL */
	Mundo.NHerois = N_HEROIS;
	Mundo.NBases = N_BASES;
	Mundo.NMissoes = N_MISSOES;
	Mundo.NHabilidades = N_HABILIDADES;
	Mundo.TamanhoMundo = N_TAMANHO_MUNDO;
	Mundo.Relogio = T_INICIO;

	/* visualizator*/
	printf ("Inicio Mundo!\n");
	printf ("NHerois = %d\n", Mundo.NHerois);
	printf ("NBases = %d\n", Mundo.NBases);
	printf ("NMissoes = %d\n", Mundo.NMissoes);
	printf ("NHabilidades = %d\n", Mundo.NHabilidades);
	printf ("TamanhoMundo = %d\n", Mundo.TamanhoMundo);
	printf ("Relogio = %d\n", Mundo.Relogio);



	/* Gera conjunto com todas habilidiades */
	TodasHab = cria_cjt (Mundo.NHabilidades);
	for (i = 1; i <= Mundo.NHabilidades; i++)
		insere_cjt (TodasHab, i);

	/* Inicializacao de cada Heroi */
	for (i = 0; i < Mundo.NHerois; i++) {
		h = &Mundo.Herois[i];
		h->id = i;
		h->hab = cria_subcjt_cjt (TodasHab, aleat (1, 3));
	    	h->pac = aleat (0, 100);
		h->vel = aleat (50, 5000);
		h->exp = 0;
	}

	/* visualizator */ 
	for (i = 0; i < Mundo.NHerois; i++) {
		h =  &Mundo.Herois[i]; 
		printf ("Heroi [%2d] | pac = %3d | vel = %4d | ", i, h->pac, h->vel);
		imprime_cjt (h->hab);
	}






	/* Inicializacao de cada Base */
	for (i = 0; i < Mundo.NBases; i++) {
		b = &Mundo.Bases[i];
		b->id = i;
		b->x = aleat (0, Mundo.TamanhoMundo);	
		b->y = aleat (0, Mundo.TamanhoMundo);
		b->lot = aleat (3, 10);
		b->pres = cria_cjt (b->lot);
		b->esp = fila_cria ();
	}
	
	/* visualizator */
	for (i = 0; i < Mundo.NBases; i++) {
		b =  &Mundo.Bases[i]; 
		printf ("Base [%d] | (%3d, %3d) | lot %2d\n", i, b->x, b->y, b->lot);	
	}






	/* Inicializacao de cada Missao */
	for (i = 0; i < Mundo.NMissoes; i++) {
		m = &Mundo.Missoes[i];
		m->id = i;
		m->x = aleat (0, Mundo.TamanhoMundo);	
		m->y = aleat (0, Mundo.TamanhoMundo);
		m->hab = cria_subcjt_cjt (TodasHab, aleat (6, 10));
	}
	
	/* visualizator */
	for (i = 0; i < Mundo.NMissoes; i++) {
		m =  &Mundo.Missoes[i]; 
		printf ("Missao [%d] | (%3d, %3d) | Hab. Req. ", i, m->x, m->y);	
		imprime_cjt (m->hab);
	}
	
	TodasHab = destroi_cjt (TodasHab);



	/* EVENTOS INICIAIS */

	Tempo = cria_lef ();

	for (i = 0; i < Mundo.NHerois; i++) {
		h = &Mundo.Herois[i];
		h->base = aleat (0, Mundo.NBases - 1);
		
		/* insere CHEGA */
		NovoEvento = cria_evento (aleat (0, 4320), 0, h->id, h->base);
		insere_lef (Tempo, NovoEvento);
	}

	/* LOOP PRINCIPAL */
	while (Tempo->primeiro != NULL) {
		e = retira_lef (Tempo);
		Mundo.Relogio = e->tempo;

		h = &Mundo.Herois[e->dado1];
		b = &Mundo.Bases[e->dado2];
		
		switch (e->tipo) {
			case 0:
				chega (e->tempo, h, b, Tempo);
				break;
			case 1:
				espera (e->tempo, h, b, Tempo);
				break;	
			case 2:
				desiste (e->tempo, h, b, Tempo);
				break;	
			case 3:
				avisa (e->tempo, b, Tempo);
				break;
			case 4:
				entra (e->tempo, h, b, Tempo);
				break;	
			case 5:
				sai (e->tempo, h, b, Tempo);
				break;	
			case 6:
				viaja (e->tempo, h, b);
				break;	
		}

    
		e = destroi_evento (e);
	}
	


	/* LIBERANDO MEMORIA */
	
	/* Herois */ 
	for (i = 0; i < Mundo.NHerois; i++) {
		h = &Mundo.Herois[i];
		h->hab = destroi_cjt (h->hab);
	}
	
	/* Bases */
	for (i = 0; i < Mundo.NBases; i++) {
		b =  &Mundo.Bases[i]; 
		fila_destroi (&b->esp);
		b->pres = destroi_cjt (b->pres);
	}

	/* Missoes */
	for (i = 0; i < Mundo.NMissoes; i++) {
		m = &Mundo.Missoes[i];
		m->hab = destroi_cjt (m->hab);
	}
	
	Tempo = destroi_lef (Tempo);
	
	return 0;
}

