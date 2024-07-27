#include <stdio.h>
#include <stdlib.h>
#include "lef.h"

struct evento_t *cria_evento (int tempo, int tipo, int dado1, int dado2) {

	  struct evento_t *novo;

      novo = malloc (sizeof (struct evento_t));

      if (novo == NULL)
          return NULL;

      novo->tempo = tempo;
      novo->tipo = tipo;
      novo->dado1 = dado1;
      novo->dado2 = dado2;

      return novo;
}

struct evento_t *destroi_evento (struct evento_t *e) {

	free (e);
    e = NULL;
	    
	return NULL;

}

struct lef_t *cria_lef () {

    struct lef_t *novo;

    novo = malloc (sizeof (struct lef_t));

    if (novo == NULL)
		return NULL;
			    
	novo->primeiro = NULL;
	return novo;

}

struct lef_t *destroi_lef (struct lef_t *l) {

    struct nodo_lef_t *a, *b;

    a = l->primeiro;

	while (a != NULL) {
        a->evento = destroi_evento (a->evento);
        b = a;

		a = a->prox;

		free (b);
		b = NULL;
    }

    free (l);
    return NULL;
}

int insere_lef (struct lef_t *l, struct evento_t *e) {
	/* nodos para percorrer a lef, b é o anterior ao a */
	struct nodo_lef_t *a_nodo, *b_nodo;
	struct nodo_lef_t *novo_nodo;

	novo_nodo = malloc (sizeof (struct nodo_lef_t));

    if (novo_nodo == NULL)
        return 0;

    novo_nodo->evento = e;

	/* insere no inicio */
    if ((vazia_lef (l)) || (e->tempo < l->primeiro->evento->tempo)) {
        novo_nodo->prox = l->primeiro;
        l->primeiro = novo_nodo;
        return 1;
    }

	/* caso tenha pelo menos 1 nodo e o tempo seja > que o primeiro, precisa percorrer */

	a_nodo = l->primeiro;
    /* nodo a encontra evento->tempo > ou == ao procurado */
    while ((a_nodo != NULL) && (a_nodo->evento->tempo < e->tempo)) {
        b_nodo = a_nodo;
        a_nodo = a_nodo->prox;
    }
    /* insere no fim */
    if (a_nodo == NULL) {
        b_nodo->prox = novo_nodo;
        novo_nodo->prox = NULL;
    }
    /* insere na frente de a seguindo FIFO, encontrou == */
	else if (a_nodo->evento->tempo == e->tempo) { 
        novo_nodo->prox = a_nodo->prox;
        a_nodo->prox = novo_nodo;
    }
    /* insere na frente de b, caso geral */
    else {
        novo_nodo->prox = b_nodo->prox;
        b_nodo->prox = novo_nodo;
    }

    return 1;
}

struct evento_t *retira_lef (struct lef_t *l) {
   
	struct nodo_lef_t *aux;
	struct evento_t *res;

	if (vazia_lef (l))
        return NULL;

	res = l->primeiro->evento;

	aux = l->primeiro;

	l->primeiro = l->primeiro->prox;

	free (aux);

    return res;

}

int vazia_lef (struct lef_t *l) {
	if (l->primeiro == NULL)
		return 1;

    return 0;
}

void imprime_lef (struct lef_t *l) {

	struct nodo_lef_t *aux;
	int total = 0;

    aux = l->primeiro;

	printf ("LEF:\n");
    while (aux != NULL) {
        printf ("\ttempo %d tipo %d d1 %d d2 %d\n", 
		        aux->evento->tempo, 
		        aux->evento->tipo, 
		        aux->evento->dado1, 
		        aux->evento->dado2);

        total++;
        aux = aux->prox;
	}

    printf ("\tTotal %2d eventos\n", total);

}

