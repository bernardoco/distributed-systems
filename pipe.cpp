#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <cmath>

using namespace std;

bool prime (int n) {
  if (n <= 1) return false;
  for (int i=2; i<=pow(n, 0.5); i++) {
    if (n % i == 0) return true;
  }
  return false;
}

int main(int argc, char* argv[]) {
	int nn = atoi(argv[1]);
	srand(time(NULL));
	int p[2];
	pipe(p);

	pid_t pid = fork();
	if (pid == 0) {
		int n = 1;
		string output[2] = {"Primo", "Nao Primo"};
		while (n != 0) {
			read(p[0], &n, sizeof(int));
			if (n == 0) return 0;
			cout << n << ": " << output[prime(n)] << "\n";
		}
	}

	else  {
		int n = 1;
		for (int i=0; i<nn; i++) {
			int delta = rand() % 100 + 1;
			n += delta;
			write(p[1], &n, sizeof(int));
		}
		write(p[1], 0, sizeof(int));
	}

	return 0;
}
