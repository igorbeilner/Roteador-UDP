#include "roteador.h"

void die(char *s) {
	perror(s);
	exit(1);
}

void interface(void) {
	packet_t buf;

	while(1) {
		printf("Digite o id de destino e a mensagem: \n");
		scanf("%d %s", &buf.id, buf.message);
		buf.id -= 1;

		packetSend(buf);
	}
}

void packetSend(packet_t buf) {
	int Port, i;
	char IP[IPLEN];

	pthread_mutex_lock(&lock);

	Port = G->adj[G->adj[G->ID]->nextHop[buf.id]]->port;
	for(i=0; G->adj[G->adj[G->ID]->nextHop[buf.id]]->ip[i] != '\0'; i++)
		IP[i] = G->adj[G->adj[G->ID]->nextHop[buf.id]]->ip[i];
	IP[i] = '\0';
	printf("************************ %d\n", Port);
	printf("************************ %s\n", IP);

	pthread_mutex_unlock(&lock);

	struct sockaddr_in si_other;
	int s;
	socklen_t slen = sizeof(si_other);
	packet_t buff;

	if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		die("socket");

	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(Port);

	if(inet_aton(IP, &si_other.sin_addr) == 0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	if(sendto(s, &buf, strlen(buf.message) , 0, (struct sockaddr *) &si_other, slen)==-1)
		die("sendto()");

	memset(buff.message,'\0', BUFLEN);

	if(recvfrom(s, &buff, sizeof(packet_t), 0, (struct sockaddr *) &si_other, &slen) == -1)
		die("recvfrom()");

	close(s);
}

void packetReceive(void) {

	struct sockaddr_in si_me, si_other;
	int s, recv_len, Port;
	socklen_t slen = sizeof(si_other);
	packet_t buf;

	if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		die("socket");

	memset((char *) &si_me, 0, sizeof(si_me));

	pthread_mutex_lock(&lock);
	Port = G->adj[G->ID]->port;
	printf("--------------------- %d\n", Port);
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

		if(buf.id == G->ID) {
			printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
			printf("ID: %d Data: %s\n", buf.id, buf.message);

			if(sendto(s, &buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
				die("sendto()");
		} else {
			feedForward(buf);
		}
	}
	close(s);
}

void feedForward(packet_t buf) {
	int idLocal, Port, i;
	char IP[IPLEN];

	pthread_mutex_lock(&lock);

	idLocal = G->ID;
	Port = G->adj[G->adj[G->ID]->nextHop[buf.id]]->port;
	for(i=0; G->adj[G->adj[G->ID]->nextHop[buf.id]]->ip[i] != '\0'; i++)
		IP[i] = G->adj[G->adj[G->ID]->nextHop[buf.id]]->ip[i];
	IP[i] = '\0';

	pthread_mutex_unlock(&lock);
	printf("Roteador %d encaminhando mensagem com # sequência N para o destino %d...\n", idLocal, buf.id);

	struct sockaddr_in si_other;
	int s;
	socklen_t slen = sizeof(si_other);
	packet_t buff;

	if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		die("socket");

	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(Port);

	if(inet_aton(IP, &si_other.sin_addr) == 0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	if(sendto(s, &buf, strlen(buf.message) , 0, (struct sockaddr *) &si_other, slen)==-1)
		die("sendto()");

	memset(buff.message,'\0', BUFLEN);

	if(recvfrom(s, &buff, sizeof(packet_t), 0, (struct sockaddr *) &si_other, &slen) == -1)
		die("recvfrom()");

	close(s);
}
