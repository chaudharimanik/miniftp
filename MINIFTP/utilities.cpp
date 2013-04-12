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
    
    //char msg_buffer[PACKET_SIZE];
    packet pack;
    
    int packetNum=0;
    while(!fin.eof()){
        memset(pack.data,'\0',PACKET_DATA_SIZE); // null ensures strlen works
        
        //fin.read(temp_msg_buffer.data,PACKET_SIZE-2); // worst case, only 1 packet and it will
        fin.read(pack.data,PACKET_DATA_SIZE);
        
        // need both start and end DELIM
        int bytesRead=(int)fin.gcount(); // originally a long casted to int
        pack.dataSize = bytesRead;
        pack.last = 0;
        
        // returns the number of bytes read
        
        // if first packet of "message", needs start delim
//        if(packetNum==0){
//            memcpy(msg_buffer.data,&START_DELIM,sizeof(START_DELIM));
//            bytesRead++; // to ensure correct # bytes sent
//        }
        
        // lastly, if we are at end of the file, we are ending the message
//        if(fin.eof()){
//            memcpy(msg_buffer.data+strlen(msg_buffer.data),&END_DELIM,sizeof(END_DELIM));
//            bytesRead++; // to ensure correct # bytes sent
//            
//        }
        if(fin.eof())
            pack.last = 1;
            
        to_data_link(&pack, toDL, fromDL, signalFromDL);
        packetNum++;
    }
    return 1;
}

int receiveData(vector<string> arguments, int sock, int fromDL){
    ofstream fout;
    fout.open(arguments[0]);
    if(!fout.is_open()){
        // LOG
        cerr << "Failed to open file\n";
        exit(1);
    }
    
    //Now that file is open, receive and write
    //char packet_buffer[PACKET_SIZE];
    //packet packet_buffer;
    //int packetNum=0;
    
    string dataReceived=messageFromDL(fromDL);
    cout << "dataReceived=" << dataReceived << endl;
    fout.write(dataReceived.c_str(), dataReceived.length());
    fout.close();
    //int temp;
    //cin>>temp;
//    while(1){
//        memset(packet_buffer.data,'\0',PACKET_SIZE);
//        size_t bytesRec=recv(sock,packet_buffer.data,PACKET_SIZE,0);
//        if((int)bytesRec < 0){
//            perror("Failed to received file from client");
//            cout<<"strerrno=" <<errno << endl;
//            exit(1);
//        }
//        cout << packetNum+1 << ". \""<<packet_buffer.data << "\""<< endl;
//        
//        
//        if(packetNum==0 && packet_buffer.data[bytesRec-1]!=END_DELIM){
//            fout.write(packet_buffer.data+1, bytesRec-1);
//        }else if(packetNum!=0 && packet_buffer.data[bytesRec-1]!=END_DELIM){
//            fout.write(packet_buffer.data, bytesRec);
//        } else if(packetNum==0 && packet_buffer.data[bytesRec-1]==END_DELIM){
//            fout.write(packet_buffer.data+1, bytesRec-2);
//            fout.close();
//            return 1;
//        } else if(packetNum!=0 && packet_buffer.data[bytesRec-1]==END_DELIM){
//            fout.write(packet_buffer.data, bytesRec-1);
//            fout.close();
//            return 1;
//        }
//        packetNum++;
//    }
    
    return 0;
}

// Sends a string to server - separated by DLE byte
// This function assumes that the size of a message is never greater than 184
//bytes, which fits into one packet.
void sendMessage(int cmd, vector<string> parameters, int toDL, int fromDL, int signalFromDL){
    string msg_buffer= START_DELIM + to_string(cmd);
    for(int i=0;i<parameters.size();i++){
        msg_buffer+= DELIM + parameters[i] + DELIM;
    }
    msg_buffer += END_DELIM;
    cout << "msg_buffer=\"" <<msg_buffer << "\" with size " << msg_buffer.length() << "\n";

    packet pack;
    //strcpy(pack.data, data.c_str());
    memcpy(pack.data, msg_buffer.c_str(), PACKET_DATA_SIZE);
    pack.dataSize = (int)msg_buffer.length();
    pack.last = 1;
    to_data_link(&pack, toDL, fromDL, signalFromDL);
    
//    unsigned long i = 0;
//    
//    //Break the message into packets and send each packet individually to the
//    //data-link process.
//    while(i < msg_buffer.length()){
//        //unsigned long len = msg_buffer.length() - i > PACKET_SIZE ?
//        //msg_buffer.length() - i :
//        //    PACKET_SIZE;
//        unsigned long len=PACKET_SIZE;
//        string data = msg_buffer.substr(i, len);
//        packet pack;
//        //strcpy(pack.data, data.c_str());
//        memcpy(pack.data, data.c_str(),len);
//        to_data_link(&pack, toDL, fromDL, signalFromDL);
//        i+=len;
//        cout << "MSG LEN from NL=" << i << endl;
//    }
}

//Sends a packet to the data-link layer
int to_data_link(packet *p, int toDL, int fromDL, int sigFromDL){
    //char buff[3];
    
    //We assume that it waits until a message comes from data-link:
    char dlStatus;
    //while(dlStatus=='0'){
    cout << "CHECKING TOKEN-->\n";
    read(sigFromDL, &dlStatus, 1);
    cout << "TOKEN RECEIVED-->\n";

    //}
    //ssize_t bytesSent=
    write(toDL, p, sizeof(packet));
    //cout << "SENT " << bytesSent << " bytes TO DL which should equal " << PACKET_SIZE << "\n";
    //}
    //else{
       // cerr << "data link is out of sync";
    //}
    return 1;
}

string messageFromDL(int fromDL){
#ifdef DEBUG
    cout <<"Starting messsageFromDL" << endl;
#endif
    string message;
    char buffer;
    
    int packetNum=0;
    int bytesRec=0;
    int bytesTotal=0;
    //while(packetNum<1){
        while((bytesRec=read(fromDL,&buffer,1)>0)){
            bytesTotal+=bytesRec;
            if(packetNum==0 && buffer!=START_DELIM){
                //message+=buffer;
                cerr << "Message doesn't start with START_DELIM. Message ==" << message <<endl;
                exit(1);
            }
            else if(buffer==END_DELIM){
                //message+=buffer;
                break;
            }
            else if(buffer!=START_DELIM)
                message+=buffer;
            cout << "BYTE RECEIVED FROM DL\n";
            packetNum++;
        }
    //}
    
    if(errno!=0){
        cout << "errno set! -- " << errno << " with error:" << strerror(errno) << endl;
    }
    
#ifdef DEBUG
    cout <<"Message being returned from DL="<< message<< " -- Total rec bytes=" << bytesTotal << endl;
#endif
    return message;
}
























