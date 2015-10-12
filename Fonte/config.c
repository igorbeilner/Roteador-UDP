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
void digraphExit(digraph_t *G, int ID) {
	Vertex v;
	free(G->adj[ID]->nextHop);
	for(v=0; v<G->V; v++) {
		undoloc(G->adj[v]);
	}
	free(G->adj);
	free(G);
}

/********************************************************/
/***************** Imprime o grafo da rede **************/
void digraphShowUnion(digraph_t *G) {
	roteador_t *a;
	Vertex i;
	printf("  Origem  |  Destino  |  Custo\n");
	for(i = 0;i < G->V; i++)
		for(a = G->adj[i];a != NULL; a = a->next)
			printf("   %-6d |   %-7d |   %d\n", (i+1), (a->w+1), a->cost);

}

void digraphShowConfig(digraph_t *G) {
	Vertex v;
	printf("Roteador   |   Porta   |   IP\n");
	for(v=0; v<G->V; v++)
		printf("   %-5d   |   %-6d  | %s\n", v, G->adj[v]->port, G->adj[v]->ip);
	printf("\n");

}

void digraphConfig(digraph_t *G, int IDRouter, int Port, char *IP) {
	int i;
	for(i=0; IP[i] != '\0'; i++)
		G->adj[IDRouter]->ip[i] = IP[i];
	G->adj[IDRouter]->ip[i] = '\0';
	G->adj[IDRouter]->port = Port;
}

/********************************************************/
/**** Le o arquivo para construir os enlades da rede ****/
digraph_t *init(void) {
	digraph_t *G;
	Vertex v, w;
	int cost, maxVertex=0;

	int IDRouterF, Port;
	char IP[IPLEN];

/*****************************************************************/
/*********** Configura os enlaces entre os roteadores ************/

	FILE    *F = fopen("config/enlaces.config", "rb");

	if(F == NULL) {
		printf("ERRO: Arquivo enlaces.config nao encontrado\n");
		exit(1);
	}

	while(fscanf(F, "%d %d %d", &v, &w, &cost) == 3) {
		if(v > maxVertex) maxVertex = v;
		if(w > maxVertex) maxVertex = w;
	}

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

/********************************************************************/
/*********** Configura as portas e os IPs de cada roteador **********/

	F = fopen("config/roteador.config", "rb");

	if(F == NULL) {
		printf("ERRO: Arquivo roteador.config nao encontrado\n");
		exit(1);
	}

	while(fscanf(F, "%d %d %s", &IDRouterF, &Port, IP) == 3)
		if(IDRouterF < 1 || IDRouterF > G->V+1)
			break;

	if(fgetc(F) != EOF) {
		printf("ERRO NA LEITURA DO ARQUIVO roteador.config\n");
		fclose(F);
		exit(1);
	}

	fseek(F, 0, SEEK_SET);
	while(fscanf(F, "%d %d %s", &IDRouterF, &Port, IP) == 3)
		digraphConfig(G, IDRouterF-1, Port, IP);

	fclose(F);
	return G;
}

/********************************************************************
*********************** Algoritmo de Dijkstra **********************/
void dijkstra(digraph_t *G, Vertex s, Vertex *parent) {
	Vertex v, w, v0, w0;
	roteador_t *a;
	double *dist = (double *) malloc(G->V*sizeof(double));
	for (w = 0; w < G->V; w++) {
		parent[w] = -1, dist[w] = INFINITO;
	}
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
		parent[w0] = v0;
		dist[w0] = mindist;
	}
	free(dist);
}

/********************************************************************
**** Calcula o proximo salto para roteamento com menor custo ********/
void nextHop(digraph_t *G, int ID) {
	Vertex v, w;
	Vertex *parent = (Vertex *) malloc(G->V*sizeof(Vertex));
	G->adj[ID]->nextHop = (Vertex *) malloc(G->V*sizeof(Vertex));

	dijkstra(G, ID, parent);

	for(v=0; v<G->V; v++) {
		w = v;
		while(parent[w] != ID)
			w = parent[w];
		G->adj[ID]->nextHop[v] = w;
	}
	free(parent);
}
