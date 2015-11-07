#include "roteador.h"

/***************************************************/
/* Inicializa todos os enlaces como custo infinito */
void linkInit(int N) {
	int i;
	for(i=0; i<=N; i++) {
		_roteador.link[i] = INFINITO;
		_roteador.sequence[i] = 0;
	}
}

/****************************************************/
/* Copia o IP lido do arquivo para o IP do roteador */
void ipCopy(char *IP, int adjCount) {
	int i;
	for(i=0; IP[i] != '\0'; i++)
		_roteador.data[adjCount].ip[i] = IP[i];
	_roteador.data[adjCount].ip[i] = '\0';
}

/****************************************************/
/*********** Imprime os roteadores vizinhos *********/
void showEnlacesConfig(void) {
	int i;
	printf("  Origem  |  Destino  |  Custo\n");
	for(i=0; i<=_roteador.N; i++)
		if(_roteador.link[i] != INFINITO)
			printf("   %-6d |   %-7d |   %d\n", _ID, i, _roteador.link[i]);
}

/****************************************************/
/****** Imprime os IPs dos roteadores vizinhos ******/
void showRoteadorConfig(void) {
	int i;
	printf("Roteador   |   Porta   |   IP\n");
	for(i=0; i<=_roteador.E; i++)
		printf("   %-5d   |   %-6d  | %s\n", _roteador.data[i].id, _roteador.data[i].port, _roteador.data[i].ip);

}

/*****************************************************/
/* Inicializa as adjacencias dos roteadores vizinhos */
void routerInit(void) {
	int v, w, adjCount=0, cost, maxVertex=0;
	int IDRouterF, Port;
	char IP[IPLEN];

/*****************************************************************/
/*************** Encontra os enlaces do roteador *****************/

	FILE    *F = fopen("config/enlaces.config", "rb");
	if(F == NULL) {
		printf("ERRO: Arquivo enlaces.config nao encontrado\n");
		exit(1);
	}

	while(fscanf(F, "%d %d %d", &v, &w, &cost) == 3) { /* verifica o numero de roteadores da rede */
		if(v > maxVertex) maxVertex = v;
		if(w > maxVertex) maxVertex = w;
	}

	if(maxVertex < _ID) {
		printf("ID do roteador invalido\n");
		exit(1);
	}

	if(fgetc(F) != EOF) {
		printf("ERRO NA LEITURA DO ARQUIVO enlaces.config\n");
		fclose(F);
		exit(1);
	}
	_roteador.link = (int*)malloc(maxVertex*(sizeof(int))+1);
	_roteador.N = maxVertex;
	_roteador.sequence = (int*)malloc(maxVertex*sizeof(int)+1);
	linkInit(maxVertex);

	fseek(F, 0, SEEK_SET);
	while(fscanf(F, "%d %d %d", &v, &w, &cost) != EOF) {
		if(v == _ID)
			_roteador.link[w] = cost, adjCount++;
		else if(w == _ID)
			_roteador.link[v] = cost, adjCount++;
	}
	_roteador.data = (enlace_t*)malloc(adjCount*(sizeof(enlace_t)));

	fclose(F);

/********************************************************************/
/****** Configura as portas e os IPs dos roteadores adjecentes ******/

	F = fopen("config/roteador.config", "rb");

	if(F == NULL) {
		printf("ERRO: Arquivo roteador.config nao encontrado\n");
		exit(1);
	}

	while(fscanf(F, "%d %d %s", &IDRouterF, &Port, IP) == 3)
		if(IDRouterF < 1 || IDRouterF > _roteador.N)
			break;

	if(fgetc(F) != EOF) {
		printf("ERRO NA LEITURA DO ARQUIVO roteador.config\n");
		fclose(F);
		exit(1);
	}

	adjCount=-1;
	fseek(F, 0, SEEK_SET);
	while(fscanf(F, "%d %d %s", &IDRouterF, &Port, IP) == 3) {
		if(_roteador.link[IDRouterF] != INFINITO || IDRouterF == _ID) {
			adjCount++;
			_roteador.data[adjCount].id 	= IDRouterF;
			_roteador.data[adjCount].port 	= Port;
			ipCopy(IP, adjCount);
		}
	}
	_roteador.E = adjCount;

	fclose(F);

}
