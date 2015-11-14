#include "roteador.h"

/***************************************************/
/* Inicializa todos os enlaces como custo infinito */
void linkInit(int N) {
	int i;
	for(i=0; i<=N; i++) {
		_ROTEADOR.link[i] = INFINITO;
		_ROTEADOR.sequence[i] = 0;
		_ROTEADOR.nextHop[i] = 254;
		_ROTEADOR.nextTable[i] = (unsigned char*)malloc((1+_ROTEADOR.N)*(sizeof(unsigned char)));
	}
}

/*******************************************************/
/* Inicializa o vetor de proximo salto para roteamento */
void nextHopInit(void) {
	int i;
	_ROTEADOR.nextHop[_ID] = (unsigned char)_ID;
	for(i=0; i<=_ROTEADOR.N; i++)
		if(_ROTEADOR.link[i] != INFINITO && i != _ID)
			_ROTEADOR.nextHop[i] = _ROTEADOR.nextTable[i][_ID] = i;
}

/****************************************************/
/* Copia o IP lido do arquivo para o IP do roteador */
void ipCopy(char *IP, int adjCount) {
	int i;
	for(i=0; IP[i] != '\0'; i++)
		_ROTEADOR.data[adjCount].ip[i] = IP[i];
	_ROTEADOR.data[adjCount].ip[i] = '\0';
}

/****************************************************/
/************* Libera a memoria alocada *************/
void undoLock(void) {
	int i;
	free(_ROTEADOR.link);
	free(_ROTEADOR.data);
	free(_ROTEADOR.sequence);
	free(_ROTEADOR.nextHop);
	for(i=0; i<=_ROTEADOR.N; i++)
		free(_ROTEADOR.nextTable[i]);
	free(_ROTEADOR.nextTable);
}

/****************************************************/
/*********** Imprime os roteadores vizinhos *********/
void showEnlacesConfig(void) {
	int i;
	printf("  Origem  |  Destino  |  Custo\n");
	for(i=1; i<=_ROTEADOR.N; i++)
		if(_ROTEADOR.link[i] != INFINITO)
			printf("   %-6d |   %-7d |   %d\n", _ID, i, _ROTEADOR.link[i]);
}

/****************************************************/
/****** Imprime os IPs dos roteadores vizinhos ******/
void showRoteadorConfig(void) {
	int i;
	printf("Roteador   |   Porta   |   IP\n");
	for(i=1; i<=_ROTEADOR.E; i++)
		printf("   %-5d   |   %-6d  | %s\n", _ROTEADOR.data[i].id, _ROTEADOR.data[i].port, _ROTEADOR.data[i].ip);

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
	_ROTEADOR.link 		= (unsigned char*)	malloc((1+maxVertex)*(sizeof(unsigned char)));
	_ROTEADOR.nextHop 	= (unsigned char*)	malloc((1+maxVertex)*(sizeof(unsigned char)));
	_ROTEADOR.nextTable = (unsigned char**)	malloc((1+maxVertex)*(sizeof(unsigned char*)));
	_ROTEADOR.sequence 	= (int*)			malloc((1+maxVertex)*(sizeof(int)));
	_ROTEADOR.N = maxVertex;

	linkInit(maxVertex);

	fseek(F, 0, SEEK_SET);
	while(fscanf(F, "%d %d %d", &v, &w, &cost) != EOF) {
		if(v == _ID)
			_ROTEADOR.link[w] = cost, adjCount++;
		else if(w == _ID)
			_ROTEADOR.link[v] = cost, adjCount++;
	}
	//_ROTEADOR.link[_ID] = 0;
	_ROTEADOR.data = (enlace_t*)malloc((1+maxVertex)*(sizeof(enlace_t)));

	fclose(F);

/********************************************************************/
/****** Configura as portas e os IPs dos roteadores adjecentes ******/

	F = fopen("config/roteador.config", "rb");

	if(F == NULL) {
		printf("ERRO: Arquivo roteador.config nao encontrado\n");
		exit(1);
	}

	while(fscanf(F, "%d %d %s", &IDRouterF, &Port, IP) == 3)
		if(IDRouterF < 1 || IDRouterF > _ROTEADOR.N)
			break;

	if(fgetc(F) != EOF) {
		printf("ERRO NA LEITURA DO ARQUIVO roteador.config\n");
		fclose(F);
		exit(1);
	}

	adjCount=0;
	fseek(F, 0, SEEK_SET);
	while(fscanf(F, "%d %d %s", &IDRouterF, &Port, IP) == 3) {
		if(_ROTEADOR.link[IDRouterF] != INFINITO || IDRouterF == _ID) {
			adjCount++;
			_ROTEADOR.data[adjCount].id 	= IDRouterF;
			_ROTEADOR.data[adjCount].port 	= Port;
			ipCopy(IP, adjCount);
		}
	}
	_ROTEADOR.E = adjCount;

	fclose(F);

}
