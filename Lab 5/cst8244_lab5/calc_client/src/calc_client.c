#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include "..\..\calc_server\src\calc_message.h"

int main(int argc, char *argv[]) {
	client_send_t msg_send;
	//server_response_t msg_receive;
	char msg_receive[200];
	pid_t server_pid;
	int const flags = 0;

	if (argc != 5) {
		perror("ERROR: Invalid number of command line arguments\n");
		return EXIT_FAILURE;
	}

	server_pid = atoi(argv[1]);
	msg_send.left_hand = atoi(argv[2]);
	char *operation;
	operation = argv[3];
	switch (*operation) {
	case '+':
		msg_send.operator = '+';
		break;
	case '-':
		msg_send.operator = '-';
		break;
	case 'x':
		msg_send.operator = 'x';
		break;
	case '/':
		msg_send.operator = '/';
		break;
	}
	msg_send.right_hand = atoi(argv[4]);

	int coid = ConnectAttach(ND_LOCAL_NODE, server_pid, 1, _NTO_SIDE_CHANNEL,
			flags);
	if (coid == -1) {
		fprintf(stderr, "ERROR: ConnectAttach\n");
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	if (MsgSend(coid, &msg_send, sizeof(msg_send) + 1, msg_receive,
			sizeof(msg_receive)) == -1L) {
		fprintf(stderr, "ERROR: MsgSend\n");
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	if (strlen(msg_receive) > 0) {
		printf("The server has calculated the result of %d %c %d as ",
				msg_send.left_hand, msg_send.operator, msg_send.right_hand);
		printf("%s\n", msg_receive);
	} else {
		fprintf(stderr, "ERROR: Server not OK\n");
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	ConnectDetach(coid);
	return EXIT_SUCCESS;
}
