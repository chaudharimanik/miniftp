//
//  utilities.cpp
//  MINIFTP
//
//  Created by Curtis Taylor on 3/19/13.
//  Copyright (c) 2013 WPI. All rights reserved.
//

#include "header.h"
#include "utilities.h"

//Parses the input message and breaks it down into its corresponding
//command and arguments. It returns the number of arguments.
void parseMessage(const char* msg, string & command,
                  vector<string> & arguments){
    char str[BUFFER_SIZE]; //work with a copy of the input message
    strcpy(str, msg);
    
//    if(msg[0]!=START_DELIM){
//        cerr << "Start of message is != to START_DELIM" <<endl;
//        int temp;
//        cin >>temp;
//    }else if(msg[0]!=START_DELIM){
//        cerr << "Start of message is != to START_DELIM" <<endl;
//        int temp;
//        cin >>temp;
//    }
    
    
    
    char *tmp=strtok (str,&DELIM); //get the first token, i.e. the command
    if(tmp)
        command=string(tmp);
    
    while (tmp != NULL){
        char* str = strtok (NULL, &DELIM); //fill in the next argument
        tmp = str;
        
        if(tmp == NULL) //end of the message
            break;
        
        //copy the value to the returning list of arguments:
        arguments.push_back(string(str));
    }
}

int sendData(int cmd, vector<string> parameters, int toDL, int fromDL, int signalFromDL){
    ifstream fin;
    fin.open(parameters[0],ios::in);
    if(!fin.is_open()){
        cerr << "File failed to open " <<  parameters[0]  << " in send data: This should never happen!\n";
        exit(1);
    }
    
    packet pack;
    
    int packetNum=0;
    while(!fin.eof()){
        memset(pack.data,'\0',PACKET_DATA_SIZE); // null ensures strlen works
        
        //fin.read(temp_msg_buffer.data,PACKET_SIZE-2); // worst case, only 1 packet and it will
        fin.read(pack.data,PACKET_DATA_SIZE-1);
        
        // need both start and end DELIM
        int bytesRead=(int)fin.gcount(); // originally a long casted to int
        cout << "bytesRead= " << bytesRead << endl;
        cout << "BYTES="<<endl;
        if(bytesRead>180){
            cout << "in if "<< endl;
            cout << pack.data[181];
            cout << pack.data[182];
        }
        cout << endl;

        pack.dataSize = bytesRead;
        
        cout << "PACKET SIZE WRITTEN TO FILE=" <<pack.dataSize << endl;
        pack.last = 0;
        
        if(fin.eof())
            pack.last = 1;
            
        to_data_link(&pack, toDL, fromDL, signalFromDL);
        packetNum++;
    }
    return 1;
}

int receiveData(vector<string> arguments, int fromDL){
    ofstream fout;
    fout.open(arguments[0]);
    if(!fout.is_open()){
        // LOG
        cerr << "Failed to open file\n";
        return 0;
    }
    packet pack;
    int packetNum=0;
    
    do{
        memset(pack.data,'\0',PACKET_DATA_SIZE); // null ensures strlen works
        
        int bytesRec=(int)read(fromDL,&pack,sizeof(packet));
        if(bytesRec<0){
            perror("Receive data from DL failed");
        }
        pack.data[pack.dataSize]='\0';
        
        fout.write(pack.data,pack.dataSize);
        cout << "BYTES="<<endl;
        if(bytesRec>180){
            cout << pack.data[181];
            cout << pack.data[182];
            cout << endl;
        }
        //fout << string(pack.data);
        packetNum++;
        cout << "PACKET SIZE WRITTEN TO FILE=" <<pack.dataSize << endl;

    } while(!pack.last);
    
    fout.close();
    return 1;
}

// Sends a string to server - separated by DLE byte
// This function assumes that the size of a message is never greater than 184
//bytes, which fits into one packet.
void sendMessage(int cmd, vector<string> parameters, int toDL, int fromDL, int signalFromDL){
    //string msg_buffer= START_DELIM + to_string(cmd);
    string msg_buffer=to_string(cmd);

    for(int i=0;i<parameters.size();i++){
        msg_buffer+= DELIM + parameters[i];
    }
    msg_buffer+= DELIM;
    //msg_buffer += END_DELIM;
    cout << "msg_buffer=\"" <<msg_buffer << "\" with size " << msg_buffer.length() << "\n";

    packet pack;
    //strcpy(pack.data, data.c_str());
    memcpy(pack.data, msg_buffer.c_str(), PACKET_DATA_SIZE);
    pack.dataSize = (int)msg_buffer.length();
    pack.last = 1;
    to_data_link(&pack, toDL, fromDL, signalFromDL);
}

//Sends a packet to the data-link layer
int to_data_link(packet *p, int toDL, int fromDL, int sigFromDL){
    //char buff[3];
    
    //We assume that it waits until a message comes from data-link:
    char dlStatus;
    cout << "CHECKING TOKEN-->\n";
    read(sigFromDL, &dlStatus, 1);
    cout << "TOKEN RECEIVED-->\n";

    write(toDL, p, sizeof(packet));
    //cout << "SENT " << bytesSent << " bytes TO DL which should equal " << PACKET_SIZE << "\n";
    //}
    //else{
       // cerr << "data link is out of sync";
    //}
    return 1;
}

string messageFromDL(int fromDL){
    cout <<"Starting messsageFromDL" << endl;
    //string message;
    char message[PACKET_DATA_SIZE+1];

    packet packetReceived;
    
    //int packetNum=0;
    //int bytesTotal=0;
    
    int bytesRec=(int)read(fromDL,&packetReceived,sizeof(packet));
    
    // check eof?
    // only copy number of "good" bytes
    memcpy(message,packetReceived.data,packetReceived.dataSize);
    message[packetReceived.dataSize]='\0';

    
    if(errno!=0){
        cout << "errno set! -- " << errno << " with error:" << strerror(errno) << endl;
    }
    
    cout <<"Message being returned from DL="<< message<< " --  Bytes Received=" << bytesRec<< endl;
    
    return string(message);
}
























