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

header_t _ROTEADOR;
char _ACK;
int _ID;

int main() {
	pthread_t recThread, sendThread;
	pthread_t pRefresh;
	_ACK = 0;

/****** Bloco de inicializacao da rede ********/
	printf("ID: ");									/* Solicita o ID do roteator instanciado */
	scanf("%d", &_ID);

	printf("\n");
	if(_ID < 1) {						/* Valida o ID informado */
		printf("ID do roteador invalido\n");
		return 0;
	}
	routerInit();									/* Abre os arquivos e configura a rede */
	nextHopInit();

/***********************************************************************/

/***********************************************************************/

/**** Cria uma thread para realizar o refresh periodicamento, outra ouvir a porta e outra para enviar dados *****/

	pthread_create(&pRefresh, NULL, 	(void *)refresh, 	NULL);
	pthread_create(&recThread, 	NULL, 	(void *)packetReceive, 	NULL);
	pthread_create(&sendThread, NULL, 	(void *)interface, 	NULL);

	pthread_join(sendThread, 	NULL);

	pthread_cancel(pRefresh);
	pthread_cancel(recThread);

	pthread_join(pRefresh, 	NULL);
	pthread_join(recThread, 	NULL);


/***********************************************************************/
	undoLock();
	return 0;
}
