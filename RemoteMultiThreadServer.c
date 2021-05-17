/* RemoteMultiThreadServer.c */
/* Cabeceras de Sockets */
#include <sys/types.h>
#include <sys/socket.h>
/* Cabecera de direcciones por red */
#include <netinet/in.h>
/**********/
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
/**********/
/* Threads! */
#include <pthread.h>
#include <signal.h>

#include "slist.h"

typedef struct _client {
  int socket;
  char nickname[1024];
} Client;

SList clientList;
pthread_mutex_t usarLista;

/* Asumimos que el primer argumento es el puerto por el cual escuchará nuestro
servidor */

/* Maxima cantidad de cliente que soportará nuestro servidor */
#define MAX_CLIENTS 25

/* Anunciamos el prototipo del hijo */
void *child(void *arg);
/* Definimos una pequeña función auxiliar de error */
void error(char *msg);

void cortarConexiones(int signal);

int main(int argc, char **argv){
  int sock;
  struct sockaddr_in servidor, clientedir;
  socklen_t clientelen;
  pthread_t thread;
  pthread_mutex_init(&usarLista, NULL);
  signal(SIGINT, cortarConexiones);

  if (argc <= 1) error("Faltan argumentos");

  /* Creamos el socket */
  if( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
    error("Socket Init");

  /* Creamos a la dirección del servidor.*/
  servidor.sin_family = AF_INET; /* Internet */
  servidor.sin_addr.s_addr = INADDR_ANY; /**/
  servidor.sin_port = htons(atoi(argv[1]));

  /* Inicializamos el socket */
  if (bind(sock, (struct sockaddr *) &servidor, sizeof(servidor)))
    error("Error en el bind");

  printf("Binding successful, and listening on %s\n",argv[1]);

  /* Ya podemos aceptar conexiones */
  if(listen(sock, MAX_CLIENTS) == -1)
    error(" Listen error ");

  for(;;){ /* Comenzamos con el bucle infinito*/
    /* Pedimos memoria para el socket */
    Client *soclient = malloc(sizeof(Client));

    /* Now we can accept connections as they come*/
    clientelen = sizeof(clientedir);
    if ((soclient->socket = accept(sock
                          , (struct sockaddr *) &clientedir
                          , &clientelen)) == -1)
      error("No se puedo aceptar la conexión. ");
    
    clientList = agregar_nodo_final(soclient, clientList);

    snprintf(soclient->nickname, sizeof(soclient->nickname), "Anon%d", soclient->socket);

    /* Le enviamos el socket al hijo*/
    pthread_create(&thread , NULL , child, (void *) soclient);

    /* El servidor puede hacer alguna tarea más o simplemente volver a esperar*/
  }

  /* Código muerto */
  close(sock);

  return 0;
}

void broadcast(char *msg, size_t size) {
  printf("%s\n", msg);
  pthread_mutex_lock(&usarLista);
  for(SList curr = clientList; curr; curr = curr->sig) {
    send(((Client*) curr->dato)->socket, msg, size, 0);
  }
  pthread_mutex_unlock(&usarLista);
}

void * child(void *_arg) {
  Client *client = (Client*) _arg;
  char buf[1024];
  sprintf(buf, "%s joined the server", client->nickname);
  broadcast(buf, sizeof(buf));

  while (1) {
    recv(client->socket, buf, sizeof(buf), 0);
    buf[1023] = '\0';
    if(!strcmp(buf, "/exit"))
      break;
    else if(!strncmp(buf, "/nickname", 9)) {
      char newNick[1024];
      if(sscanf(buf, "/nickname %s", newNick) != 1) {
        strcpy(buf, "Usage: /nickname [newNick]\nNo spaces in usernames");
        send(client->socket, buf, sizeof(buf), 0);
      } else {
        int found = 0;
        for(SList curr = clientList; curr; curr = curr->sig) {
          if(!strcmp(newNick, ((Client*) curr->dato)->nickname)) {
            found = 1;
          }
        }
        if(!found) {
          sprintf(buf, "%s is now %s", client->nickname, newNick);
          broadcast(buf, sizeof(buf));
          strcpy(client->nickname, newNick);
        } else {
          strcpy(buf, "Somebody already has that username");
          send(client->socket, buf, sizeof(buf), 0);
        }
      }
    }
    else if(!strncmp(buf, "/msg", 4)) {
      char nick[1024], msg[1024];
      if(sscanf(buf, "/msg %s %[^\n]", nick, msg) != 2) {
        strcpy(buf, "Usage: /msg [userNick] [message]");
        send(client->socket, buf, sizeof(buf), 0);
      } else {
        pthread_mutex_lock(&usarLista);
        int found = 0;
        for(SList curr = clientList; curr; curr = curr->sig) {
          if(!strcmp(nick, ((Client*) curr->dato)->nickname)) {
            sprintf(buf, "(%s): %s", client->nickname, msg);
            buf[1023] = '\0';
            send(((Client*) curr->dato)->socket, buf, sizeof(buf), 0);
            found = 1;
          }
        }
        if(!found) {
          strcpy(buf, "No such user found");
          send(client->socket, buf, sizeof(buf), 0);
        }
        pthread_mutex_unlock(&usarLista);
      }
    } else if(!strncmp(buf, "/list", 5)) {
      buf[0] = '\0';
      for(SList curr = clientList; curr; curr = curr->sig) {
        strcat(buf, ((Client*) curr->dato)->nickname);
        if(curr->sig)
          strcat(buf, ", ");
      }
      send(client->socket, buf, sizeof(buf), 0);
    } else {
      char msg[1024];
      sprintf(msg, "%s: %s", client->nickname, buf);
      msg[1023] = '\0';
      broadcast(msg, sizeof(msg));
    }
  }

  pthread_mutex_lock(&usarLista);
  if (clientList->dato == _arg) {
    SList nodoAEliminar = clientList;
    clientList = clientList->sig;
    free(nodoAEliminar);
  }
  for(SList curr = clientList; curr && curr->sig; curr = curr->sig) {
    if(curr->sig->dato == _arg) {
      SList nodoAEliminar = curr->sig;
      curr->sig = curr->sig->sig;
      free(nodoAEliminar);
    }
  }
  pthread_mutex_unlock(&usarLista);

  sprintf(buf, "%s left the server", client->nickname);
  broadcast(buf, sizeof(buf));

  free((Client*)_arg);
  return NULL;
}

void error(char *msg){
  exit((perror(msg), 1));
}

void destruirCliente(void *cliente) {
  Client c = *(Client*) cliente;
  char buf[1024] = "/exit";
  send(c.socket, buf, 1024, 0);
  close(c.socket);
  free((Client*) cliente);
}

void cortarConexiones(int signal) {
  printf("Closing server. Clients will be disconnected\n");
  pthread_mutex_lock(&usarLista);
  slist_destruir(clientList, destruirCliente);
  pthread_mutex_unlock(&usarLista);
  pthread_mutex_destroy(&usarLista);
  exit(0);
}