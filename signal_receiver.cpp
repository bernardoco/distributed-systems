#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
	int pid = atoi(argv[1]);
	int signal = atoi(argv[2]);
	int ret = kill(pid, 0);
	if (ret == 0) {
		kill(pid, signal);
	} else {
		cout << "O PID nao existe\n";
		return -1;
	}
	return 0;
}
