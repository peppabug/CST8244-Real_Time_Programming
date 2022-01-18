#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include "calc_message.h"

int main(void) {
	client_send_t client_message;
	server_response_t response;
	char message[50];
	int rcvid;
	int const flags = 0;

	int chid = ChannelCreate(flags);
	if (chid == -1) {
		perror("ERROR: ChannelCreate\n");
		exit(EXIT_FAILURE);
	}

	printf("Server PID = %d\n", getpid());

	while (1) {
		rcvid = MsgReceive(chid, &client_message, sizeof(client_message), NULL);
		if (rcvid == -1)
			perror("ERROR: MsgReceive\n");

		response.statusCode = SRVR_OK;

		int left_temp = client_message.left_hand;
		int right_temp = client_message.right_hand;
		switch (client_message.operator) {
		case '+':
			response.answer = left_temp + right_temp;
			break;
		case '-':
			response.answer = left_temp - right_temp;
			break;
		case 'x':
			response.answer = left_temp * right_temp;
			break;
		case '/':
			if (right_temp == 0) {
				perror("ERROR: Cannot divide by zero\n");
				exit(-1);
			} else {
				response.answer = left_temp / right_temp;
				break;
			}
		}

		snprintf(message, 50, "%.2f", response.answer);

		if (MsgReply(rcvid, EOK, message, sizeof(server_response_t)) == -1) {
			fprintf(stderr, "ERROR: MsgReply\n");
			perror(NULL);
			exit(EXIT_FAILURE);
		}
	}

	ChannelDestroy(chid);
	return EXIT_SUCCESS;
}
