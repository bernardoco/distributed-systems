#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <chrono>
#include <ctime>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>


template <typename Duration>
void print_time(tm t, Duration fraction) {
    using namespace std::chrono;
    std::printf("[%04u-%02u-%02u %02u:%02u:%02u.%03u]\n", t.tm_year + 1900,
                t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec,
                static_cast<unsigned>(fraction / milliseconds(1)));
}

using namespace std;

struct message {
	int msgId;
	int socketId;
};


message decode (char *buffer) {
   struct message msg;
   int i=0;
   string msgId = "";
   string sockId = "";
   while (buffer[i] != '|') {
      msgId += buffer[i];
      i++;
   }
   i++;
   while (buffer[i] != '|') {
      sockId += buffer[i];
      i++;
   }
   msg.msgId = stoi(msgId);
   msg.socketId = stoi(sockId);
   return msg;
}

void encode (char *buffer, int code, int socketId, int F) {
   string strCode = to_string(code);
   string strSocketId = to_string(socketId);
   int i;
   for (i=0; i<(int) strCode.length(); i++)
      buffer[i] = strCode[i];
   buffer[i] = '|';
   i++;
   int j;
   int k = 0;
   for (j=i; j<i+(int) strSocketId.length(); j++) {
      buffer[j] = strSocketId[k];
      k++;
   }
   buffer[j] = '|';
   j++;
   while (j < F-1) {
      buffer[j] = '0';
      j++;
   }
}

int main(int argc, char* argv[]) {
	int sock = 0;
	int F = 10;
	if (argc != 3) {
		cout << "Uso: ./proc r k\n";
		exit(1);
	}

	int r = atoi(argv[1]);
	int k = atoi(argv[2]);

	sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		cout << "Falha no Socket Client\n";
		exit(1);
	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(12345);

	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		cout << "Falha na Conversao de IP\n";
		exit(1);
	}
	if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		cout << "Falha na Conexao " << errno << "\n";
		exit(1);
	}
	int myId;
	recv(sock, &myId, sizeof(int), 0);

	char buffer[F] = "";

	for (int steps=0; steps<r; steps++) {
		encode(buffer, 1, myId, F);
		send(sock, &buffer, sizeof(buffer), 0);
		recv(sock, buffer, sizeof(buffer), 0);
		struct message msgResponse = decode(buffer);
		if (msgResponse.msgId == 2) {
			ofstream myfile;
			myfile.open("resultado.txt", ios::app);
			chrono::system_clock::time_point now = chrono::system_clock::now();
			chrono::system_clock::duration tp = now.time_since_epoch();
			tp -= chrono::duration_cast<chrono::seconds>(tp);
			time_t tt = chrono::system_clock::to_time_t(now);
			char current_time[50];
			tm t = *localtime(&tt);
			sprintf(current_time, "[%04u-%02u-%02u %02u:%02u:%02u.%03u]", t.tm_year + 1900,
                t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec,
                static_cast<unsigned>(tp / chrono::milliseconds(1)));

			myfile << myId << " " << current_time << "\n";
			sleep(k);
			myfile.close();
			encode(buffer, 3, myId, F);
			send(sock, &buffer, sizeof(buffer), 0);
		}
	}
	return 0;
}
