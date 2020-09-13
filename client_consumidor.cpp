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

bool prime (int n) {
  if (n <= 1) return false;
  for (int i=2; i<=pow(n, 0.5); i++) {
    if (n % i == 0) return true;
  }
  return false;
}

int main(int argc, char* argv[]) {
	int sock = 0;
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
    cout << "Falha na Conexao\n";
    exit(1);
  }

  int n = 1;
  bool isPrime;
  while (n != 0) {
    read(sock, &n, sizeof(int));
    isPrime = prime(n);
    send(sock, &isPrime, sizeof(bool), 0);
  }
	return 0;
}
