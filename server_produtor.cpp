#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cmath>
#include <errno.h>

using namespace std;

int main(int argc, char* argv[]) {
	int server, socketfd;
  sockaddr_in address;
  memset(&address, 0, sizeof(address));
  int addrlen = sizeof(address);
  srand(time(NULL));

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
  if (listen(server, 0) < 0) {
    cout << "Falha no Listen\n";
    exit(1);
  }
  if ((socketfd = accept(server, (struct sockaddr *) &address, (socklen_t*)&addrlen)) < 0) {
    cout << "Falha no Accept\n";
    exit(1);
  }

  int n = 1;
  int delta;
  bool isPrime;
  int nn = atoi(argv[1]);
	string output[2] = {"Primo", "Nao Primo"};
  for (int i=0; i<nn; i++) {
    delta = rand() % 100 + 1;
    n += delta;
    send(socketfd, &n, sizeof(int), 0);
    read(socketfd, &isPrime, sizeof(bool));
    cout << n << ": " << output[isPrime] << "\n";
  }
  send(socketfd, 0, sizeof(int), 0);
	return 0;
}
