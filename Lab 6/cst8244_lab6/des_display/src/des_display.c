/*
 * Filename:    des_display.c
 * Date:        November 18, 2021
 * Course:      CST8244 Real-Time Programming
 * Author(s):   Michael Turner and Zhicheng Zhu
 * Function(s): int main(void)
 *
 * Reads message sent from des_controller and prints the appropriate
 * message to the console.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <errno.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include "../../des_controller/src/des.h"

send_t msg_received;
response_t response;

int main(void) {
	name_attach_t *attach;
	int const flags = 0;
    int rcvid;

    if ((attach = name_attach(NULL, NAME_ATTACH_DISPLAY, flags)) == NULL) {
    	printf("ERROR: Could not attach to controller channel");
		exit(EXIT_FAILURE);
    }

    printf("Display's PID: %d\n", getpid());

    while (1) {
        if ((rcvid = MsgReceive(attach->chid, &msg_received, sizeof(send_t), NULL)) == -1) {
            printf("ERROR: Could not receive message from controller");
            return EXIT_FAILURE;
        }

        if (MsgReply(rcvid, EOK, &response, sizeof(response_t)) == -1) {
            printf("ERROR: Could not reply to message from controller");
            return EXIT_FAILURE;
        }

        // If message received was ID scan
        if (msg_received.input == LEFT_SCAN_I || msg_received.input == RIGHT_SCAN_I)
        	printf("%s %d\n", outMessage[SCAN_O], msg_received.person_id);
        // If message received was weight scan
        else if (msg_received.input == WEIGHT_SCAN_I)
        	printf("Person ID: %d has been weighed: %.2f.\n", msg_received.person_id, msg_received.weight);
        else if (msg_received.input == LEFT_UNLOCK_I) printf("%s\n", outMessage[LEFT_UNLOCK_O]);
        else if (msg_received.input == RIGHT_UNLOCK_I) printf("%s\n", outMessage[RIGHT_UNLOCK_O]);
        else if (msg_received.input == LEFT_OPEN_I) printf("%s\n", outMessage[LEFT_OPEN_O]);
        else if (msg_received.input == RIGHT_OPEN_I) printf("%s\n", outMessage[RIGHT_OPEN_O]);
        else if (msg_received.input == LEFT_CLOSE_I) printf("%s\n", outMessage[LEFT_CLOSE_O]);
        else if (msg_received.input == RIGHT_CLOSE_I) printf("%s\n", outMessage[RIGHT_CLOSE_O]);
        else if (msg_received.input == LEFT_LOCK_I) printf("%s\n", outMessage[LEFT_LOCK_O]);
        else if (msg_received.input == RIGHT_LOCK_I) printf("%s\n", outMessage[RIGHT_LOCK_O]);
        // Print message to the console
        else printf("%s\n", outMessage[msg_received.input]);

        // If message was received was exit, break from the loop
        if (msg_received.input == EXIT_I) break;
    }

    name_detach(attach, flags);
    return EXIT_SUCCESS;
}
