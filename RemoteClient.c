/* RemoteClient.c
   Se introducen las primitivas necesarias para establecer una conexión simple
   dentro del lenguaje C utilizando sockets.
*/
/* Cabeceras de Sockets */
#include <sys/types.h>
#include <sys/socket.h>
/* Cabecera de direcciones por red */
#include <netdb.h>
/**********/
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

/*
  El archivo describe un sencillo cliente que se conecta al servidor establecido
  en el archivo RemoteServer.c. Se utiliza de la siguiente manera:
  $cliente IP port
 */

void error(char *msg){
  exit((perror(msg), 1));
}

int sock;
struct addrinfo *resultado;

void *enviarMensajes(void* nada);

void cortarConexion(int signal);

int main(int argc, char **argv) {

  /*Chequeamos mínimamente que los argumentos fueron pasados*/
  if(argc != 3){
    fprintf(stderr,"El uso es \'%s IP port\'", argv[0]);
    exit(1);
  }

  /* Inicializamos el socket */
  if( (sock = socket(AF_INET , SOCK_STREAM, 0)) < 0 )
    error("No se pudo iniciar el socket");

  /* Buscamos la dirección del hostname:port */
  if (getaddrinfo(argv[1], argv[2], NULL, &resultado)){
    fprintf(stderr,"No se encontro el host: %s \n",argv[1]);
    exit(2);
  }

  if(connect(sock, (struct sockaddr *) resultado->ai_addr, resultado->ai_addrlen) != 0)
    /* if(connect(sock, (struct sockaddr *) &servidor, sizeof(servidor)) != 0) */
    error("No se pudo conectar :(. ");
  
  signal(SIGINT, cortarConexion);

  printf("La conexión fue un éxito!\n");

  pthread_t thread;
  pthread_create(&thread, NULL, enviarMensajes, NULL);
  char buf[1024];
  while(1) {
    recv(sock, buf, sizeof(buf), 0);
    buf[1023] = '\0';
    if(!strcmp(buf, "/exit"))
      break;
    printf("%s\n", buf);
  }

  /* Cerramos :D!*/
  freeaddrinfo(resultado);
  close(sock);

  return 0;
}

void *enviarMensajes(void* nada) {
  char buf[1024];
  while(1) {
    scanf(" %[^\n]", buf);
    if(!strcmp(buf, "/exit"))
      raise(SIGINT);
    send(sock, buf, sizeof(buf), 0);
  }
}

void cortarConexion(int signal) {
  char buf[1024] = "/exit";
  send(sock, buf, sizeof(buf), 0);
  freeaddrinfo(resultado);
  close(sock);
  exit(0);
}