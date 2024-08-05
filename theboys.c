#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/* demais includes */
#include "conjunto.h"
#include "fila.h"
#include "lef.h"


#define T_INICIO 		0
#define T_FIM_DO_MUNDO 		525600
#define N_TAMANHO_MUNDO 	20000
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
    struct base_t *base;
};

struct base_t {
    int id;
    int lot;
    struct conjunto *pres;
    struct fila *esp;
    int x, y;
};

struct dist_t {
    int d;
    int base;
};

struct missao_t {
    int id;
    struct conjunto *hab;
    int tentativa;
    int concluida;
    struct dist_t Distancias[N_BASES];
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
	h->base = b;

	/* fila de espera */
	f = b->esp;

	/* conjunto de presentes */
	c = b->pres;

	printf ("%6d: CHEGA  HEROI %2d BASE %d (%2d/%2d) ", t, h->id, b->id, c->card, c->max);

	tem_vaga = ((c->card) < (c->max));  
	eh_vazia = fila_vazia (f);	

	if (tem_vaga && eh_vazia)
		espera = 1;
	else
		espera = ((h->pac) > (10 * f->tamanho));


	if (espera) {
		printf ("ESPERA\n");
		/* insere ESPERA */
		NovoEvento = cria_evento (t, 1, h->id, h->base->id);
		insere_lef (Tempo, NovoEvento);
	}
	else {
		printf ("DESISTE\n");
		/* insere DESITE */
		NovoEvento = cria_evento (t, 2, h->id, h->base->id);
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
	NovoEvento = cria_evento (t, 3, h->id, h->base->id);
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
		fila_imprime (f);	
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
	NovoEvento = cria_evento (t + TPB, 5, h->id, h->base->id);
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
void viaja (int t, struct heroi_t *h, struct base_t *d, struct lef_t *Tempo) {

	int dist, durac;

  	struct evento_t *NovoEvento;

	/* calcula duração da viagem */
	dist = sqrt (pow (h->base->x - d->x, 2) + pow (h->base->y - d->y, 2));
	durac = dist / h->vel;

	printf ("%6d: VIAJA  HEROI %2d BASE %d BASE %d DIST %d VEL %d CHEGA %d\n", 
			t, h->id, h->base->id, d->id, dist, h->vel, t + durac);

	/* insere CHEGA */
	NovoEvento = cria_evento (t + durac, 0, h->id, d->id);
	insere_lef (Tempo, NovoEvento);

}

/* FIM (T) -    	TIPO [ 7 ] */
void fim (int t, struct mundo_t *Mundo) {
	
	int i, missoes_concluidas, tentativas, tent_min, tent_max;

	float media, precisao;

	struct heroi_t *h;
	struct missao_t *m;
	
	printf ("%6d: FIM\n", t);

	/* dados dos herois */
	for (i = 0; i < Mundo->NHerois; i++) {
		h = &Mundo->Herois[i];
		printf ("HEROI %2d PAC %3d VEL %4d EXP %4d HABS ", h->id, h->pac, h->vel, h->exp);
		imprime_cjt (h->hab);
	}

	/* dados das missoes */
	missoes_concluidas = 0;

	tentativas = 0;
	tent_min = Mundo->TamanhoMundo;
	tent_max = 0;
	for (int i = 0; i < Mundo->NMissoes; i++) {
		m = &Mundo->Missoes[i];
		
		if (m->concluida == 1)
			missoes_concluidas++;

		if (m->tentativa < tent_min)
			tent_min = m->tentativa;
		if (m->tentativa > tent_max)
			tent_max = m->tentativa;

		tentativas += m->tentativa;
	}

	precisao = ((float)missoes_concluidas/Mundo->NMissoes)*100;
	media = ((float)(tentativas / Mundo->NMissoes));

	printf ("MISSOES CUMPRIDAS: %d/%d (%.2f%%)\n", missoes_concluidas, Mundo->NMissoes, precisao);
	printf ("TENTATIVAS/MISSAO: MIN %d, MAX %d, MEDIA %.2f\n", tent_min, tent_max, media);
}

/* MISSAO (T, M) - 	TIPO [ 8 ] */
void missao (int t, struct missao_t *m, struct lef_t *Tempo, struct mundo_t *Mundo) {

	int i, sucesso, heroi_id, base_id, dist_base_id;

	struct evento_t *NovoEvento;
	
	struct heroi_t *h;
	struct base_t *b;
	struct conjunto *uniao, *c;

	m->tentativa++;

	printf ("%6d: MISSAO %4d TENT %d HAB REQ: ", t, m->id, m->tentativa);
	imprime_cjt (m->hab);

	/* loop atraves das prioridades */
	sucesso = 0;
	for(i = 0; i < Mundo->NBases && !sucesso; i++) {
		base_id = m->Distancias[i].base;
		dist_base_id = m->Distancias[i].d;
		
		b = &Mundo->Bases[base_id];
		
		uniao = cria_cjt (Mundo->NHabilidades);

		printf ("%6d: MISSAO %4d BASE %2d DIST %d HEROIS ", t, m->id, base_id, dist_base_id);
		imprime_cjt (b->pres);

		/* loop atraves do cjt de presentes */
		inicia_iterador_cjt (b->pres);
		while (incrementa_iterador_cjt (b->pres, &heroi_id)) {
			h = &Mundo->Herois[heroi_id];
			printf ("%6d: MISSAO %4d HAB HEROI %2d: ", t, m->id, heroi_id);
			imprime_cjt (h->hab);

			c = uniao;
			uniao = uniao_cjt (c, h->hab);
			c = destroi_cjt (c);
		}

		/* uniao de habilidades de pres em base i */
		printf ("%6d: MISSAO %4d UNIAO HAB BASE %2d: ", t, m->id, b->id);
		imprime_cjt (uniao);

		/* check p ver se completam */
		if (contido_cjt (m->hab, uniao)) {
			sucesso = 1;
		}
	
		/* libera uniao */	
		uniao = destroi_cjt (uniao);

	}


	if (sucesso) {
		m->concluida = 1;
		printf ("%6d: MISSAO %4d CUMPRIDA BASE %d\n", t, m->id, base_id);
		
		/*incrementa xp*/
		inicia_iterador_cjt (b->pres);
		while (incrementa_iterador_cjt (b->pres, &heroi_id)) {
			h = &Mundo->Herois[heroi_id];
			h->exp++;
		}
	} else {
		printf ("%6d: MISSAO %d IMPOSSIVEL\n", t, m->id);
		
		/* insere MISSAO */
		NovoEvento = cria_evento (t + 24*60, 8, 0, m->id);
		insere_lef (Tempo, NovoEvento);
	}
}

int main () {
	/* declaracoes de variaveis aqui */
   	struct mundo_t Mundo;

	struct lef_t *Tempo;
	struct evento_t *NovoEvento, *e;
	struct conjunto *TodasHab; 
	struct heroi_t *h;
	struct base_t *b;
	struct missao_t *m;

	int i, j, k, min_valor, min_indice;
	struct dist_t aux;

   	srand (0); /* use zero, nao faca com time (0) */

   	/* ESTADO INICIAL */
	Mundo.NHerois = N_HEROIS;
	Mundo.NBases = N_BASES;
	Mundo.NMissoes = N_MISSOES;
	Mundo.NHabilidades = N_HABILIDADES;
	Mundo.TamanhoMundo = N_TAMANHO_MUNDO;
	Mundo.Relogio = T_INICIO;

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
	

	/* Inicializacao de cada Missao */
	for (i = 0; i < Mundo.NMissoes; i++) {
		m = &Mundo.Missoes[i];
		m->id = i;
		m->tentativa = 0;
		m->concluida = 0;

		m->x = aleat (0, Mundo.TamanhoMundo);	
		m->y = aleat (0, Mundo.TamanhoMundo);

		/* calcula a distancia de cada base e guarda em um vetor,
		 * associado ao indice = id da base */
		for (j = 0; j < Mundo.NBases; j++) {
			b = &Mundo.Bases[j];
			m->Distancias[j].d = sqrt (pow (m->x - b->x, 2) + pow (m->y - b->y, 2));
			m->Distancias[j].base = j;
		}

		/* ordena as distancias via selection sort ajustando a prioridade */
		for (j = 0; j < Mundo.NBases; j++) {
			/* assumindo min a priori */
			min_valor = m->Distancias[j].d;
			min_indice = j;
			
			for (k = j+1; k < Mundo.NBases; k++) {
				if (m->Distancias[k].d < min_valor) {
					min_valor = m->Distancias[k].d;
					min_indice = k;
				}
			}
			
			/* necessidade de swap */
			if (min_indice != j) {
				aux = m->Distancias[j];
				m->Distancias[j] = m->Distancias[min_indice];
				m->Distancias[min_indice] = aux;
			}	
		}
	
		m->hab = cria_subcjt_cjt (TodasHab, aleat (6, 10));
	}
		

	TodasHab = destroi_cjt (TodasHab);



	/* EVENTOS INICIAIS */

	Tempo = cria_lef ();

	/* Chegada dos herois em suas respectivas bases */
	for (i = 0; i < Mundo.NHerois; i++) {
		h = &Mundo.Herois[i];
		h->base = &Mundo.Bases[aleat (0, Mundo.NBases - 1)];
		
		/* insere CHEGA */
		NovoEvento = cria_evento (aleat (0, 4320), 0, h->id, h->base->id);
		insere_lef (Tempo, NovoEvento);
	}

	/* Criacao dos eventos missao */
	for (i = 0; i < Mundo.NMissoes; i++) {
		m = &Mundo.Missoes[i];

		/* insere MISSAO */
		NovoEvento = cria_evento (aleat (0, T_FIM_DO_MUNDO), 8, 0, i);
		insere_lef (Tempo, NovoEvento);
	}

	
	/* insere FIM */
	NovoEvento = cria_evento (T_FIM_DO_MUNDO, 7, 0, 0);	
	insere_lef (Tempo, NovoEvento);

	/* LOOP PRINCIPAL */

	do {
		e = retira_lef (Tempo);
		Mundo.Relogio = e->tempo;

		switch (e->tipo) {
			case 0:
				h = &Mundo.Herois[e->dado1];
				b = &Mundo.Bases[e->dado2];

				chega (e->tempo, h, b, Tempo);
				break;
			case 1:
				h = &Mundo.Herois[e->dado1];
				b = &Mundo.Bases[e->dado2];

				espera (e->tempo, h, b, Tempo);
				break;	
			case 2:
				h = &Mundo.Herois[e->dado1];
				b = &Mundo.Bases[e->dado2];

				desiste (e->tempo, h, b, Tempo);
				break;	
			case 3:
				b = &Mundo.Bases[e->dado2];

				avisa (e->tempo, b, Tempo);
				break;
			case 4:
				h = &Mundo.Herois[e->dado1];
				b = &Mundo.Bases[e->dado2];

				entra (e->tempo, h, b, Tempo);
				break;	
			case 5:
				h = &Mundo.Herois[e->dado1];
				b = &Mundo.Bases[e->dado2];

				sai (e->tempo, h, b, Tempo);
				break;	
			case 6:
				h = &Mundo.Herois[e->dado1];
				b = &Mundo.Bases[e->dado2];

				viaja (e->tempo, h, b, Tempo);
				break;	
			case 7:
				fim (e->tempo, &Mundo);
				break;
			case 8:
				m = &Mundo.Missoes[e->dado2];
			
				missao (e->tempo, m, Tempo, &Mundo);
				break;
		}

    
		e = destroi_evento (e);
	} while (Mundo.Relogio < T_FIM_DO_MUNDO);
	


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

