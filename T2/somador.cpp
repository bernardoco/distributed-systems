#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <random>
#include <numeric>
#include <iostream>
#include <atomic>
#include <thread>
#include <time.h>
#include <chrono>
#include <ctime>

#define N						10000000

using namespace std;

atomic_flag lock = ATOMIC_FLAG_INIT;

void acquire() {
	while(lock.test_and_set()) {}
}

void release() {
	lock.clear();
}

void sum(vector<signed char>::const_iterator begin, vector<signed char>::const_iterator end, long int *soma) {
	long int s = accumulate(begin, end, 0);
	acquire();
	*soma += s;
	release();
}

int main (int argc, char * argv[]) {
	if (argc != 2) {
		cout << "Uso: ./somador K\n";
		return 1;
	}

	srand(time(NULL));
	vector<signed char> numbers;
	for (int i=0; i<N; i++) {
		numbers.push_back((rand() % 201) - 100);
	}
	int k = atoi(argv[1]);
	vector<thread> v;
	long int soma = 0;
	int piece = (int) N/k;
	double tempos[9];
	auto start = chrono::system_clock::now();
	for (int i=0; i<k; i++) {
		thread thread_obj(sum,
								numbers.begin() + i*piece,
								numbers.end() - (k-i-1)*piece, 
							 	&soma);
		v.push_back((thread&&) (thread_obj));
	}

	for (int i=0; i<k; i++) {
		v[i].join();
	}
	
	auto end = chrono::system_clock::now();
	chrono::duration<double> elapsed = end-start;
	cout << "time: " << elapsed.count() << "\n";
	cout << "Soma: " << soma << "\n";

	return 0;
}
