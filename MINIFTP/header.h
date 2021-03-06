//
// header.h
// MINIFTP
// CS513-TEAM2


#ifndef header_h
#define header_h

//#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <vector>
#include <math.h>
#include <sys/wait.h>
using namespace std;

const string PORT="7766"; // unused port
const char DL_DELIM=0x10;
const char APP_DELIM=' ';

//const char START_DELIM='S';
//const char END_DELIM='E';

typedef unsigned int seq_nr;
typedef struct timeval timestruct;

#define BUFFER_SIZE 100
#define PAYLOAD_SIZE 100
#define PACKET_SIZE 192
#define PACKET_DATA_SIZE 184
#define MAX_MESSAGE_LEN 190 // need to put in design report
#define FRAME_TIMEOUT 1

// Messages
#define MSG_LOGIN 0
#define MSG_LOGOUT 1
#define MSG_LIST 2
#define MSG_PUT 3
#define MSG_GET 4
#define MSG_REMOVE 5
#define MSG_GRANT 6
#define MSG_REVOKE 7
#define MSG_LOGOUT_EXIT 8
#define MSG_CONFIRM_REQ 9
#define MSG_CONFIRM_RES 10
#define MSG_DATA 99
#define MSG_OK 21
#define MSG_ERROR 22
#define MSG_NO_EXIST 23
#define MSG_IN_USE 24
#define MSG_OVERWRITE 25
#define MSG_USER_NO_EXIST 26

#define MSG_NO 30
#define MSG_YES 31

struct packet{
    int dataSize;
    int last; //1 if the packet is the last packet of a message or a file
    char data[PACKET_DATA_SIZE]; //tan has this as unsigned char[]
};

//Primary Author: Salman
void parseMessage(const char* msg, string & command, vector<string> & arguments);
//Primary Author: Curtis
int checkNumArguments(int cmd, vector<string> arguments);
//Primary Author: Curtis
int sendData(int cmd, vector<string> parameters, int sock);
//Primary Author: Curtis
int receiveData(vector<string> arguments, int sock);
//Primary Author: Curtis
void sendMessage(int cmd, vector<string> parameters, int sock);

#endif
