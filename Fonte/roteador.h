#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

#define Vertex int
#define INFINITO 999999
#define BUFLEN 100     /* Tamanho maximo da mensagem enviada pelo usuario */
#define IPLEN 15

/*roteador_t: contém o conteúdo do vértice e o ponteiro pro próximo*/
typedef struct roteador_t {
	Vertex w;
	int cost;
	int port;
	int *nextHop;
	char ip[IPLEN];
	struct roteador_t *next;
}roteador_t;

/*digraph_t: contém o número de vértices do grafo e o número de arestas.
adj é a lista de adjacencia.*/
typedef struct digraph_t {
	int V;
	int A;
	int ID;
	int *sequence;
	roteador_t **adj;
}digraph_t;

/******************************/
/***** Pacote de dados ********/
typedef struct packet_t {
	int id;
	int idSrc;
	char ack;
	int sequence;
	char message[BUFLEN];
}packet_t;


extern digraph_t *G;
extern pthread_mutex_t lock;
extern char _ACK;

/********************* protótipos das funções **********************/
digraph_t	*digraphInit		(int V);
void		 digraphInsertA		(digraph_t *G, Vertex v, Vertex w, int cost);
roteador_t	*newNode			(Vertex w, roteador_t *next, int cost);
void		 digraphShowUnion	(digraph_t *G);
void		 digraphShowConfig	(digraph_t *G);
void		 dijkstra			(digraph_t *G, Vertex s, Vertex *parent);
digraph_t	*init 				(void);
void		 digraphExit 		(digraph_t *G, int ID);
void		 undoloc			(roteador_t *roteador);
void		 die 				(char *s);
void		 packetSend			(packet_t buf);
void		 packetReceive		(void);
void		 nextHop			(digraph_t *G, int ID);
void		 interface			(void);