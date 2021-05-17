#ifndef __SLIST_H__
#define __SLIST_H__

#include <stddef.h>

typedef void (*FuncionVisitante) (int dato);

typedef struct _SNodo
{   
    void* dato;
    struct _SNodo *sig;
    
}SNodo;

typedef SNodo * SList;

typedef void (*Destruir) (void* dato);

/**
 * Devuelve una lista vacía.
 */
SList slist_crear();

/**
 * Destruccion de la lista.
 */
void slist_destruir(SList lista, Destruir d);


/**
 * Determina si la lista es vacía.
 */
int slist_vacia(SList lista);

/**
 * Agrega un elemento al final de la lista.
 */
SList agregar_nodo_final(void* dato, SList lista);

/**
 * Agrega un elemento al inicio de la lista.
 */
SList agregar_nodo_inicio(void* dato, SList lista);

/**
 * Une 2 listas en una sola 
 */
SList append(SList izq, SList der);

#endif /* __SLIST_H__ */
