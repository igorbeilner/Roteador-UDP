#include "roteador.h"

void die(char *s) {
	perror(s);
	exit(1);
}

void frameSend(int *PORT) {
	struct sockaddr_in si_other;
	int s;
	socklen_t slen = sizeof(si_other);
	char buf[BUFLEN];
	char message[BUFLEN];

	if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		die("socket");

	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(*PORT);

	if(inet_aton(SERVER , &si_other.sin_addr) == 0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	while(1) {
		printf("Enter message : ");
		scanf("%s",message);

		if(sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen)==-1)
			die("sendto()");

		memset(buf,'\0', BUFLEN);

		if(recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
			die("recvfrom()");

		puts(buf);
	}

	close(s);
}

void frameReceive(int *PORT) {

	struct sockaddr_in si_me, si_other;
	int s, recv_len;
	socklen_t slen = sizeof(si_other);
	char buf[BUFLEN];

	if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		die("socket");

	memset((char *) &si_me, 0, sizeof(si_me));

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(*PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(s, (struct sockaddr*)&si_me, sizeof(si_me)) == -1)
		die("bind");

	while(1) {
		fflush(stdout);

		memset(buf,'\0', BUFLEN);

		if((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
			die("recvfrom()");

		printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		printf("Data: %s\n" , buf);

		if(sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
			die("sendto()");
	}

	close(s);
}

void initRouter(int IDRouter) {
	struct sockaddr_in si_other;
	int s;
	socklen_t slen = sizeof(si_other);
	char buf[MAXCARACTER];
	char message[MAXCARACTER];


	if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		die("socket");

	si_other.sin_family = AF_INET;
	//si_other.sin_port = htons(Port);
	// if (inet_aton(IP , &si_other.sin_addr) == 0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	//}
	bzero(&(si_other.sin_zero),8);

	while(1) {
		printf("Digite a mensagem: ");
		scanf("%s", message);
		if(sendto(s, message, strlen(message), 0, (struct sockaddr*) &si_other, slen) == -1)
			die("sendto()");

		memset(buf,'\0', MAXCARACTER);

		if(recvfrom(s, buf, MAXCARACTER, 0, (struct sockaddr*)&si_other, &slen) == -1)
			die("recvfrom()");

		puts(buf);
	}

	close(s);
}
