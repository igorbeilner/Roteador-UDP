/*
 * Project name:
	Simulador de um protocolo de roteamento entre roteadores de uma rede
	usando sockets UDP

 * Autor:
	Igor Beilner

 * Revision History:
	Versão 1.0
	10/10/2015

 * Description:


* Test configuration:

 * NOTES:
*/

#include "roteador.h"

int main() {
	int ID;
	digraph_t *G;

/****** Bloco de inicializacao da rede ********/
	G = init();                                  // Abre os arquivos e configura a rede
	printf("ID: ");                              // Solicita o ID do roteator instanciado
	scanf("%d", &ID);
	printf("\n");
	if(ID > G->V || ID < 1) {                    // Valida o ID informado
		printf("ID do roteador invalido\n");
		return 0;
	}
/*********************************************/
	dijkstra(G, ID-1);
	digraphShowConfig(G);

	digraphExit(G);
	return 0;
}
