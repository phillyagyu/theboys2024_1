#include <stdlib.h>
#include <stdio.h>
#include "fila.h"

struct fila *fila_cria () {
	struct fila *f;

	f = malloc (sizeof (struct fila));

	if (f == NULL)
		return NULL;

	f->ini = NULL;
	f->fim = NULL;
	f->tamanho = 0;

	return f;
}

void fila_destroi (struct fila **fila) {
	struct nodo *a, *b;

	a = (*fila)->ini;
		
	while (a != NULL) {
		b = a;
		a = a->prox;
		free (b);
		b = NULL;
	}
	free (*fila);
	*fila = NULL;
}

int enqueue (struct fila *fila, int dado) {
	struct nodo *novo;

	novo = malloc (sizeof (struct nodo));

	if (novo == NULL)
		return 0;

	/* atribuicoes de novo */
	novo->chave = dado;
	novo->prox = NULL;

	/* caso seja o primeiro da fila */
	if (fila->tamanho == 0) 
		fila->ini = novo;
	/* caso segundo */
	else if (fila->ini == fila->fim) 
		fila->ini->prox = novo;
	/* caso geral, inserir no fim da fila */
	else 
		fila->fim->prox = novo;
	
		
	fila->fim = novo;
	fila->tamanho++;
	return 1;
}

int dequeue (struct fila *fila, int *dado) {
	struct nodo *aux;

	if (fila_vazia (fila))
		return 0;
	
	/* elemento a ser retirado */
	aux = fila->ini;
	*dado = aux->chave;

	/* caso seja o unico nodo */
	if (fila->ini == fila->fim) {
		fila->ini = NULL;
		fila->fim = NULL;
	}
	/* caso tenha pelo menos um */
	else
		fila->ini = fila->ini->prox;

	/* liberando mem e aterrando */
	free (aux);
	aux = NULL;

	fila->tamanho--;
	return 1;
}
 
int fila_tamanho (struct fila *fila) {
	return fila->tamanho;	
}

int fila_vazia (struct fila *fila) {
	if (fila->ini == NULL)
		return 1;

	return 0;
}

/* Imprime a fila no formato "[ e1 e2 ... en ]" 
 * Se a fila for vazia imprime "[ ]" */
void fila_imprime (struct fila *fila) {
	
	struct nodo *aux;

	if (fila_vazia (fila)) {
		printf ("[ ]\n");
		return;
	}

	aux = fila->ini;
	
	printf ("[");
	while (aux != NULL) {
		printf (" %d", aux->chave);
		aux = aux->prox;
	}
	printf (" ]\n");

}
