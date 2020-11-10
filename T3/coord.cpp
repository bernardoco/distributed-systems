#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <mutex>
#include <string>
#include <queue>
#include <vector>
#include <thread>

#define MAX_PROCESS			129

using namespace std;

int socketfd[MAX_PROCESS];
int grantCounter[MAX_PROCESS];
int socketIndex = 0;

struct message {
	int msgId;
	int socketId;
};

mutex mtx;
queue<int> q;


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
	while (j < F) {
		buffer[j] = '0';
		j++;
	}
}
/*
int findIndex (int *socketfd, int id) {
	int i = 0;
	for (i=0; i<=MAX_PROCESS; i++) {
		if (socketfd[i] == id) {
			return i;
		}
	}
	return -1;
}
*/
void printQueue (queue<int> q) {
	cout << "[";
	while (!q.empty()) {
		cout <<q.front();
		q.pop();
		if (!q.empty()) cout << ", ";
	}
	cout << "]\n";
}


void server (void) {
	int server;
	sockaddr_in address;
	memset(&address, 0, sizeof(address));
	int addrlen = sizeof(address);

	if ((server = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		cout << "Falha no Socket\n";
		exit(1);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(12345);

	if (bind(server, (const sockaddr *) &address, sizeof(address)) < 0) {
		cout << "Falha no Bind\n";
		exit(1);
	}

	if (listen(server, MAX_PROCESS) < 0) {
		cout << "Falha no Listen\n";
		exit(1);
	}
	while (1) {
		if ((socketfd[socketIndex] = accept(server,
														(struct sockaddr *) &address,
														(socklen_t*)&addrlen)) < 0) {
			cout << "Falha no Accept\n";
			exit(1);
		}
		send(socketfd[socketIndex], &socketIndex, sizeof(int), 0);
		socketIndex++;
	}
}

void coordenador (void) {
	int F = 10;
	char buffer[F];
	struct timeval tv;
	int rv;
	fd_set readfds;

	while (1) {
		//memset(buffer, 0, sizeof(buffer));
		FD_ZERO(&readfds);
		for (int i=0; i<socketIndex; i++) {
			FD_SET(socketfd[i], &readfds);
		}

		tv.tv_sec = 1;
		int returnValue = 0;

		int maxSocket = 0;
		for (int i=0; i<socketIndex; i++) {
			if (socketfd[i] > maxSocket) maxSocket = socketfd[i];
		}

		rv = select(maxSocket+1, &readfds, NULL, NULL, &tv);
		int i;
		if (rv == -1) {
			cout << "Select Error\n";
			continue;
		}
		else if (rv == 0) {
			continue;
		}
		else {
			for (i=0; i<socketIndex; i++) {
				if (socketfd[i] < 0) continue;
				if (FD_ISSET(socketfd[i], &readfds)) {
					returnValue = recv(socketfd[i], buffer, sizeof(buffer), 0);
					break;
				}
			}
			if (returnValue < 1) {
				socketfd[i] = -2;
				continue;
			}
			struct message msg = decode(buffer);
			if (msg.msgId == 1) {
				mtx.lock();
				if (q.empty()) {
					encode(buffer, 2, msg.socketId, F);
					while (send(socketfd[msg.socketId], &buffer, sizeof(buffer), 0) == -1);
					grantCounter[i]++;
				}
				q.push(msg.socketId);
				mtx.unlock();
			}

			else if (msg.msgId == 3) {
				mtx.lock();
				q.pop();
				if (!q.empty()) {
					int process = q.front();
					encode(buffer, 2, process, F);
					while (send(socketfd[process], &buffer, sizeof(buffer), 0) == -1);
					grantCounter[process]++;
				}
				mtx.unlock();
			}
		}
	}
}

void interface (void) {
	int code;
	cout << "Bem vindo a Interface do Trabalho 3!\n";
	cout << "1 - Imprime Fila\n";
	cout << "2 - Numero de Processos Atendidos\n";
	cout << "3 - Encerrar\n";

	while (1) {
		cout << "Opção: ";
		cin >> code;
		switch(code) {
			case (1):
				{
				mtx.lock();
				printQueue(q);
				mtx.unlock();
				break;
				}
			
			case (2):
				{
				int i = 0;
				for (i=0; i<socketIndex; i++) {
					if (socketfd[i] != -1) {
						cout << i << ": " << grantCounter[i] << "\n";
					}
				}
				break;
				}
			
			case (3):
				{
				exit(0);
				break;
				}
		
			default:
				{
				cout << "Opcao Invalida\n"	;
				break;
				}
		}
	}
}


int main (int agrc, char *argv[]) {
	memset(socketfd, -1, sizeof(socketfd));

	vector<thread> v;
	thread thread_server(server);
	v.push_back((thread&&) (thread_server));
	thread thread_coord(coordenador);
	v.push_back((thread&&) (thread_coord));
	thread thread_interface(interface);
	v.push_back((thread&&) (thread_interface));

	for (int i=0; i<=2; i++) v[i].join();
	return 0;
}
