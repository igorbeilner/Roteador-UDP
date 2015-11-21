#include "roteador.h"

void die(char *s) {
	perror(s);
	exit(1);
}

void interface(void) {
	packet_t buf;
	int ID, idMax, i;

	ID = _ID;
	idMax = _ROTEADOR.N;

	while(1) {
		printf("Digite o id de destino e a mensagem: \n");
		scanf("%d", &buf.id);

		if(buf.id < 1 || buf.id > idMax) {
			printf("Desligando.....\n");
			return;
		}
		_ROTEADOR.sequence[buf.id]++;
		buf.sequence = _ROTEADOR.sequence[buf.id];

		scanf("%s", buf.message);
		buf.idSrc = ID;
		buf.type = 0;
		buf.hop = 0;
		packetSend(buf);

		for(i=1; i<=3; i++) {
			usleep(100000);
			if(_ACK == 0) { 		/* Verifica se a confirmacao foi recebida */
				printf("Retransmitindo pacote %d pela %d vez\n", buf.sequence, i);
				packetSend(buf);
			} else {
				printf("Pacote enviado com sucesso!\n");
				_ACK = 0;
				break;
			}
		}
		if(i==4)  {
			printf("Falha ao enviar o pacote!\n");
			_ACK = 0;
		}
	}
}

void packetSend(packet_t buf) {
	int idLocal, Port, next, i, j;
	char IP[IPLEN];

	if(buf.hop > DIAMETRO)
		return;

	idLocal = _ID;
	if(_ROTEADOR.nextHop[buf.id] != 254)
		next = _ROTEADOR.nextHop[buf.id];
	else
		return;

	for(j=0; j<_ROTEADOR.E; j++)
		if(_ROTEADOR.data[j].id == next)
			break;

	Port 	= _ROTEADOR.data[j].port;

	for(i=0; _ROTEADOR.data[j].ip[i] != '\0'; i++)
		IP[i] = _ROTEADOR.data[j].ip[i];
	IP[i] = '\0';

	if(buf.type == 0 && idLocal != buf.idSrc) {/* Verifica se e uma confirmacao ou se e pra ele mesmo */
		printf("Roteador %d encaminhando mensagem com sequencia %d para o destino %d\n", idLocal, buf.sequence, next);
	}

	buf.hop++;
	struct sockaddr_in si_other;
	int s;
	socklen_t slen = sizeof(si_other);

	if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		die("socket");

	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(Port);

	if(inet_aton(IP, &si_other.sin_addr) == 0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	if(sendto(s, &buf, sizeof(packet_t) , 0, (struct sockaddr *) &si_other, slen)==-1)
		die("sendto()");

	close(s);
}

void packetReceive(void) {

	struct sockaddr_in si_me, si_other;
	int s, recv_len, Port, ID, i;
	socklen_t slen = sizeof(si_other);
	packet_t buf;

	if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		die("socket");

	memset((char *) &si_me, 0, sizeof(si_me));

	for(i=1; i<=_ROTEADOR.E; i++)
		if(_ROTEADOR.data[i].id == _ID)
			break;

	Port 	= _ROTEADOR.data[i].port;
	ID 		= _ID;

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(Port);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(s, (struct sockaddr*)&si_me, sizeof(si_me)) == -1)
		die("bind");

	while(1) {
		fflush(stdout);

		memset(buf.message,'\0', BUFLEN);

		if((recv_len = recvfrom(s, &buf, sizeof(packet_t), 0, (struct sockaddr *) &si_other, &slen)) == -1)
			die("recvfrom()");

		if(buf.id == ID) {		/* Verifica se a mensagem recebida e pra ele */
			if(buf.type == 0) {		/* Verifica se e uma confirmacao ou nao */

				printf("|********** Receive **********|\n");
				printf("	IdFonte: %d\n	IdDestino: %d \n	Dado: %s\n", buf.idSrc, buf.id, buf.message);
				printf("|*****************************|\n\n");

				buf.id = buf.idSrc;
				buf.idSrc = ID;
				buf.type = 1;

				packetSend(buf);
			} else if(buf.type == 1) {				/* Recebeu uma confirmacao de um pacote de dados */
				if(buf.sequence == _ROTEADOR.sequence[buf.idSrc])
					_ACK = 1;
			} else if(buf.type == 2) {				/* Recebeu um pacote de configuracao */

				buf.id = buf.idSrc;
				buf.idSrc = ID;
				buf.type = 3;

				nextHop(buf);

			}

		} else {				/* Retransmite para outro roteador com o menos custo */
			packetSend(buf);
		}
	}
	close(s);
}

void refresh(void) {
	int i, j, k, aux;

	while(1) {
		
		sendControl();

		sleep(1);
		_ROTEADOR.validity[_ID] = 3;

		for(i=1; i<=_ROTEADOR.E; i++) {
			if(_ROTEADOR.validity[_ROTEADOR.data[i].id] > 0)	/* Verifica se o viinho está ativo e decrementa o tempo */
				_ROTEADOR.validity[_ROTEADOR.data[i].id]-=1;
			else if(_ROTEADOR.data[i].id != _ID) {				/* Se está inativo atualiza o vetor distância */
				_ROTEADOR.link[_ROTEADOR.data[i].id] = INFINITO;
				for(j=1; j<=_ROTEADOR.N; j++) {		/* Passa por todos os destinos verificando que algum deles dependia do enlace que caiu para ser alcançado */
					if(_ROTEADOR.nextHop[j] == _ROTEADOR.data[i].id) {
						aux = INFINITO;
						for(k=1; k<=_ROTEADOR.N; k++) {
							if(_ROTEADOR.nextTable[j][k] < aux && k != _ROTEADOR.data[i].id && k != _ID && _ROTEADOR.validity[k]) {
								_ROTEADOR.link[j] = _ROTEADOR.nextTable[j][k];
								aux = _ROTEADOR.nextTable[j][k];
								_ROTEADOR.nextHop[j] = k;
							}
						}
					}
				}
			}
		}
	}
}

void sendControl(void) {		/* Envia os pacotes de controle */
	int i;
	packet_t buf;

	buf.idSrc = _ID;

	struct sockaddr_in si_other;
	int s;
	socklen_t slen = sizeof(si_other);

	buf.type = 2;
	buf.hop = 0;
	buf.sequence = 0;

	for(i=1; i<=_ROTEADOR.N; i++)
		buf.message[i] = _ROTEADOR.link[i];
	buf.message[i] = '\0';

	buf.message[0] = 255;

	if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		die("socket");

	for(i=1; i<=_ROTEADOR.E; i++) {
		if(_ROTEADOR.data[i].id == buf.idSrc)
			continue;
		buf.id = _ROTEADOR.data[i].id;

		memset((char *) &si_other, 0, sizeof(si_other));
		si_other.sin_family = AF_INET;
		si_other.sin_port = htons(_ROTEADOR.data[i].port);

		if(inet_aton(_ROTEADOR.data[i].ip, &si_other.sin_addr) == 0) {
			fprintf(stderr, "inet_aton() failed\n");
			exit(1);
		}
			
		if(sendto(s, &buf, sizeof(packet_t) , 0, (struct sockaddr *) &si_other, slen)==-1)
			die("sendto()");

	}
	close(s);
}


void nextHop(packet_t buf) {
	int i, flag, j;

	_ROTEADOR.validity[buf.id] = 3;

	for(i=1; i<=_ROTEADOR.N; i++)
		_ROTEADOR.nextTable[i][buf.id] = buf.message[i];		/* Armazena o vetor distância recebido na tabela de vetores */

	flag=0;
	for(i=1; i<=_ROTEADOR.N; i++) {
		if((buf.message[i] + _ROTEADOR.nextTable[buf.id][_ID]) < _ROTEADOR.link[i])	{ /* Verifica se os caminhos por este destino são menos custosos */
				
			_ROTEADOR.link[i] = buf.message[i] + _ROTEADOR.nextTable[buf.id][_ID];
			_ROTEADOR.nextHop[i] = buf.id;
			flag=1;

		}
		if(_ROTEADOR.link[i] > INFINITY && _ROTEADOR.link[i] != INFINITO) {			/* Verifica se o enlace está em contagem ao infinito */
			_ROTEADOR.link[i] = INFINITO;
			_ROTEADOR.nextHop[i] = 254;
			flag=1;
		}
		/* Verifica se o vetor recebido é de algum próximo salto e recomputa a tabela */
		if((buf.id == _ROTEADOR.nextHop[i]) && (_ROTEADOR.link[i] != (buf.message[i] + _ROTEADOR.nextTable[buf.id][_ID]))) {
			_ROTEADOR.link[i] = INFINITO;
			for(j=1; j<=_ROTEADOR.N; j++) {
				if(((_ROTEADOR.nextTable[i][j] + _ROTEADOR.nextTable[j][_ID]) < _ROTEADOR.link[i]) && i != j && (_ROTEADOR.validity[j] > 0) && j != _ID) {
					_ROTEADOR.link[i] = _ROTEADOR.nextTable[i][j] + _ROTEADOR.nextTable[j][_ID];
					_ROTEADOR.nextHop[i] = j;
					flag = 1;
				}
			}
		} 
	}

	if(flag) {
		sendControl();
		showTable();
	}
}
