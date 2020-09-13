#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <csignal>
#include <unistd.h>
#include <string.h>
using namespace std;

void signalHandler (int signum) {
	switch (signum) {
		case 1:
			cout << "Sinal Recebido: 1\n";
			break;
		case 2:
			cout << "Sinal Recebido: 2\n";
			exit(0);
		case 3:
			cout << "Sinal Recebido: 3\n";
			break;
	}
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		cout << "Uso: ./nomedoprograma (busy ou blocked)\n";
		return 1;
	}
	cout << getpid() << "\n";

	int sig;
	string wait_type = argv[1];
	sigset_t signal_set;

	if (wait_type == "busy") {
		signal(1, signalHandler);
		signal(2, signalHandler);
		signal(3, signalHandler);
		while (1) {
			sleep(1);
		}
		return 0;
	}

	sigemptyset(&signal_set);
	sigaddset(&signal_set, 1);
	sigaddset(&signal_set, 2);
	sigaddset(&signal_set, 3);

	sigprocmask(SIG_BLOCK, &signal_set, NULL);
	while (1) {
		sigwait(&signal_set, &sig);
		signalHandler(sig);
	}
	return 0;
}
