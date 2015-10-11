#include "roteador.h"

/********************************************************/
/** recebe o número de vértices, e cria um novo grafo ***/
digraph_t *digraphInit(int V) {
    Vertex v;
    digraph_t *G = malloc(sizeof(digraph_t));
    G->V = V;
    G->A = 0;
    G->adj = malloc(V*sizeof(roteador_t));
    for (v = 0; v < V; v++)
        G->adj[v] = NULL;
    return G;
}
/********************************************************/
/*************** insere uma nova aresta *****************/
void digraphInsertA(digraph_t *G, Vertex v, Vertex w, int cost) {
    roteador_t *a;
    for (a = G->adj[v]; a != NULL; a = a->next)
        if (a->w == w) return;
    G->adj[v] = newNode(w, G->adj[v], cost);
    G->adj[w] = newNode(v, G->adj[w], cost);

    G->A+=2;
}

/********************************************************/
/*** recebe um vértice e o insere em um novo node *******/
roteador_t *newNode(Vertex w, roteador_t *next, int cost) {
    roteador_t *a = malloc(sizeof (roteador_t));
    a->w = w;
    a->cost = cost;
    a->next = next;
    return a;
}

/********************************************************/
/*********** Exclui todas as arestas do grafo ***********/
void undoloc(roteador_t *roteador) {

    if(roteador != NULL) {
        undoloc(roteador->next);
        free(roteador);
    }
}

/********************************************************/
/***************** Exclui o grafo da rede ***************/
void digraphExit(digraph_t *G) {
    Vertex v;
    for(v=0; v<G->V; v++) {
        undoloc(G->adj[v]);
    }
    free(G->adj);
    free(G);
}

/********************************************************/
/***************** Imprime o grafo da rede **************/
void digraphShow(digraph_t *G) {
    roteador_t *a;
    Vertex i;
    printf("  Origem  |  Destino  |  Custo\n");
    for(i = 0;i < G->V; i++)
        for(a = G->adj[i];a != NULL; a = a->next)
            printf("   %-6d |   %-7d |   %d\n", (i+1), (a->w+1), a->cost);

}

/********************************************************************
*********************** Algoritmo de Dijkstra **********************/
void dijkstra(digraph_t *G, Vertex s) {
    Vertex v, w, v0, w0;
    roteador_t *a;
    Vertex *parent = (Vertex *) malloc(G->V*sizeof(Vertex));
    double *dist = (double *) malloc(G->V*sizeof(double));
    for (w = 0; w < G->V; w++)
        parent[w] = -1, dist[w] = INFINITO;
    parent[s] = s;
    dist[s] = 0.0;

    while (1) {
        double mindist = INFINITO;
        for (v = 0; v < G->V; v++)
            if (parent[v] != -1)
                for (a = G->adj[v]; a != NULL; a = a->next)
                    if (parent[a->w] == -1 && mindist > dist[v] + a->cost) {
                        mindist = dist[v] + a->cost;
                        v0 = v, w0 = a->w;
                    }
        if (mindist == INFINITO) break;
        /* A */
        parent[w0] = v0;
        dist[w0] = mindist;
    }
    printf("%d: ", s);
    for(v=0; v<G->V; v++)
        if(v!=s)
            printf("%d - %.0lf | ", v, dist[v]);
    printf("\n");

    for(v=0; v<G->V; v++)
        printf("%d - %d | ", v, parent[v]);
    printf("\n");
}

/********************************************************/
/**** Le o arquivo para construir os enlades da rede ****/
digraph_t *init() {
    digraph_t *G;
    Vertex v, w;
    int cost, maxVertex=0;

    FILE    *F = fopen("config/enlaces.config", "rb");

    if(F == NULL) {
        printf("ERRO: Arquivo enlaces.config nao encontrado\n");
        exit(1);
    }

    while(fscanf(F, "%d %d %d", &v, &w, &cost) == 3)
        maxVertex = v > w ? v : w;

    if(fgetc(F) != EOF) {
        printf("ERRO NA LEITURA DO ARQUIVO enlaces.config\n");
        fclose(F);
        exit(1);
    }

    fseek(F, 0, SEEK_SET);
    G = digraphInit(maxVertex);
    while(fscanf(F, "%d %d %d", &v, &w, &cost) != EOF)
        digraphInsertA(G, (v-1), (w-1), cost);

    fclose(F);
    return G;
}
