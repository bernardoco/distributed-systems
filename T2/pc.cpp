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
#include <semaphore.h>
#include <mutex>
#include <condition_variable>
#include <cmath>

#define max_value					 10000000
#define N 							 32

class Semaphore {
	public:
		Semaphore (int count_ = 0)
		: count(count_) {}

		inline void signal() {
			std::unique_lock<std::mutex> lock(mtx);
			count++;
			cv.notify_one();
		}

		inline void wait() {
			std::unique_lock<std::mutex> lock(mtx);
			while(count == 0) {
				cv.wait(lock);
			}
			count--;
		}

	private:
		std::mutex mtx;
		std::condition_variable cv;
		int count;
};


using namespace std;
mutex mtx;
Semaphore empty (N);
Semaphore full (0);

void produtor (vector<int> &numbers, int &counter) {
	while (1) {
		int n = rand() % (max_value+1);
		empty.wait();
		mtx.lock();
		if (counter > 100000) {
			mtx.unlock();
			empty.signal();
			full.signal();
			return void();
		}

		for (int i=0; i<N; i++) {
			if (numbers[i] == 0) {
				numbers[i] = n;
				break;
			}
		}
		mtx.unlock();
		full.signal();
	}
}
