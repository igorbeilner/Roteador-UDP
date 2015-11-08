#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

#define INFINITO 999999
#define BUFLEN 100     /* Tamanho maximo da mensagem enviada pelo usuario */
#define IPLEN 15

/******************************/
/***** Pacote de dados ********/
typedef struct packet_t {
	int id;					/* ID do roteador destino */
	int idSrc;				/* ID do roteador fonte para envio de confirmacao */
	char type;				/* Tipo da mensagem (0-Envio, 1-Confirmacao */
	int sequence;			/* Numero de sequencia do pacote */
	char message[BUFLEN];	/* Dados que estao sendo enviados */
}packet_t;

/******************************/
/**** Enlaces do roteador *****/
typedef struct enlace_t {
	int id;				/* ID do roteador vizinho */
	int port;			/* Port que o roteador vizinho ouve */
	char ip[IPLEN];		/* IP do roteador vizinho */
}enlace_t;

/******************************/
/**** Estrutura do roteador ***/
typedef struct header_t {
	int N;				/* Numero de roteadores da rede */
	int E;				/* Numero de roteadores vizinhos */
	int *sequence;
	int *link;			/* Roteadores vizinhos */
	enlace_t *data;		/* IP e Porta dos roteadores vizinhos */
}header_t;

/************************************/
/******** Variaveis globais *********/
extern pthread_mutex_t lock;
extern char _ACK;				/* Indica recepcao de confirmacao (0-Nao recebeu, 1-recebeu) */
extern header_t _ROTEADOR;		/* Informacoes da rede */
extern int _ID;					/* ID do roteadore instanciado */

/*******************************************************************/
/********************* protótipos das funções **********************/
void		die 				(char *s);
void		packetSend			(packet_t buf);
void		packetReceive		(void);
void		interface			(void);
void 		linkInit 			(int N);
void 		ipCopy				(char *IP, int adjCount);
void 		showEnlacesConfig	(void);
void		showRoteadorConfig	(void);
void		routerInit			(void);
