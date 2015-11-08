#include "roteador.h"

void die(char *s) {
	perror(s);
	exit(1);
}

void interface(void) {
	packet_t buf;
	int ID, idMax, i;
	pthread_mutex_lock(&lock);
	ID = _ID;
	idMax = _ROTEADOR.N;
	pthread_mutex_unlock(&lock);

	while(1) {
		printf("Digite o id de destino e a mensagem: \n");
		scanf("%d", &buf.id);
		//system("clear");

		if(buf.id < 1 || buf.id > idMax) {
			printf("Desligando.....\n");
			return;
		}
		pthread_mutex_lock(&lock);
		_ROTEADOR.sequence[buf.id-1]++;
		buf.sequence = _ROTEADOR.sequence[buf.id-1];
		pthread_mutex_unlock(&lock);

		scanf("%s", buf.message);
		buf.idSrc = ID+1;
		buf.type = 0;
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
	int idLocal, Port, next;//, i;
	char IP[IPLEN];

	pthread_mutex_lock(&lock);

	idLocal = _ID;
	//next = G->adj[G->ID]->nextHop[buf.id-1];
	//Port = G->adj[G->adj[G->ID]->nextHop[buf.id-1]]->port;
	//for(i=0; G->adj[G->adj[G->ID]->nextHop[buf.id-1]]->ip[i] != '\0'; i++)
		//IP[i] = G->adj[G->adj[G->ID]->nextHop[buf.id-1]]->ip[i];
	//IP[i] = '\0';

	pthread_mutex_unlock(&lock);
	if(buf.type == 0 && idLocal != buf.idSrc-1) {/* Verifica se e uma confirmacao ou se e pra ele mesmo */
		//system("clear");
		printf("Roteador %d encaminhando mensagem com sequencia %d para o destino %d\n", idLocal+1, buf.sequence, next+1);
	}

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
	int s, recv_len, Port, ID;
	socklen_t slen = sizeof(si_other);
	packet_t buf;

	if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		die("socket");

	memset((char *) &si_me, 0, sizeof(si_me));

	pthread_mutex_lock(&lock);

	//Port = G->adj[G->ID]->port;
	//ID = G->ID;

	pthread_mutex_unlock(&lock);

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

		if(buf.id == _ID) {		/* Verifica se a mensagem recebida e pra ele */
			if(buf.type == 0) {		/* Verifica se e uma confirmacao ou nao */
				//system("clear");
				printf("|********** Receive **********|\n");
				printf("	IdFonte: %d\n	IdDestino: %d \n	Dado: %s\n", buf.idSrc, buf.id, buf.message);
				printf("|*****************************|\n\n");

				buf.id = buf.idSrc;
				buf.idSrc = ID;
				buf.type = 1;

				packetSend(buf);
			}else{				/* Recebeu uma confirmacao */

				//if(buf.sequence == G->sequence[buf.idSrc])
					_ACK = 1;

			}
		} else {				/* Retransmite para outro roteador com o menos custo */
			packetSend(buf);
		}
	}
	close(s);
}

void refresh(void) {
	int Port, next, i, j;
	char IP[IPLEN];

	for(j=1; j<_ROTEADOR.E; j++) {
		if(j == _ID)
			continue;

		pthread_mutex_lock(&lock);

			Port = _ROTEADOR.data[j].port;
			for(i=0; _ROTEADOR.data[j].id[i] != '\0'; i++)
				IP[i] = _ROTEADOR.data[j].id[i];
			IP[i] = '\0';

		pthread_mutex_unlock(&lock);

		struct sockaddr_in si_other;
		socklen_t slen = sizeof(si_other);
		int s;

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
}
