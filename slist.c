#include "slist.h"
#include <stdlib.h>

SList slist_crear() {
  return NULL;
}

void slist_destruir(SList lista, Destruir d) {
  SList nodoAEliminar;
  while (lista != NULL) {
    nodoAEliminar = lista;
    lista = lista->sig;
    d(nodoAEliminar->dato);
    free(nodoAEliminar);
  }
}

int slist_vacia(SList lista) {
  return lista == NULL;
}

SList agregar_nodo_final(void* dato, SList lista) {
    SList nuevoNodo = malloc(sizeof(SNodo));
    nuevoNodo->dato = dato;
    nuevoNodo->sig = NULL;
    if (lista == NULL)
        return nuevoNodo;

    SList nodo = lista;
    for (;nodo->sig != NULL;nodo = nodo->sig);

    nodo->sig = nuevoNodo;
    return lista;
}

SList agregar_nodo_inicio(void* dato, SList lista) {
  SList nuevoNodo = malloc(sizeof(SNodo));
  nuevoNodo->dato = dato;
  nuevoNodo->sig = lista;
  return nuevoNodo;
}

SList append(SList izq, SList der) {
  SList inicio = izq;
  while (izq->sig != NULL) {
    izq = izq->sig;
  }
  izq->sig = der;
  while (izq->sig != NULL) {
    izq = izq->sig;
  }
  izq->sig = NULL;
  return inicio;
}
