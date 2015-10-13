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

digraph_t *G;
pthread_mutex_t lock;
char _ACK;

int main() {
	int ID;
	pthread_t recThread, sendThread;
	_ACK = 0;

/****** Bloco de inicializacao da rede ********/
	G = init();									// Abre os arquivos e configura a rede
	printf("ID: ");								// Solicita o ID do roteator instanciado
	scanf("%d", &ID);
	ID -= 1;
	printf("\n");
	if(ID > G->V || ID < 0) {					// Valida o ID informado
		printf("ID do roteador invalido\n");
		return 0;
	}
/*********************************************/
	G->ID = ID;
/***********************************************************************/
/** Calcula os proximo salto para alcancar todos os outros roteadores **/
	nextHop(G, ID);
/***********************************************************************/

/***********************************************************************/
/**** Cria uma thread para ouvir a porta e outra para enviar dados *****/
	pthread_mutex_init(&lock, NULL);


	pthread_create(&sendThread, NULL, 	(void *)interface, 	NULL);
	pthread_create(&recThread, 	NULL, 	(void *)packetReceive, 	NULL);

	pthread_join(sendThread, 	NULL);
	pthread_cancel(recThread);

	pthread_join(recThread, 	NULL);
/***********************************************************************/

	digraphExit(G, ID);
	return 0;
}
