#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Vertex int
#define INFINITO 999999
#define MAXCARACTER 100

/*roteador_t: contém o conteúdo do vértice e o ponteiro pro próximo*/
typedef struct roteador_t {
    Vertex w;
    int cost;
    struct roteador_t *next;
}roteador_t;

/*digraph_t: contém o número de vértices do grafo e o número de arestas.
adj é a lista de adjacencia.*/
typedef struct digraph_t {
    int V;
    int A;
    roteador_t **adj;
}digraph_t;

/********************* protótipos das funções **********************/
digraph_t   *digraphInit(int V);
void         digraphInsertA(digraph_t *G, Vertex v, Vertex w, int cost);
roteador_t  *newNode(Vertex w, roteador_t *next, int cost);
void         digraphShow(digraph_t *G);\
void         dijkstra(digraph_t *G, Vertex s);
digraph_t   *init();
void         digraphExit(digraph_t *G);
void         undoloc(roteador_t *roteador);