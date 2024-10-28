//============================================================================
// Name        : Programming.cpp
// Author      : Julio Chavez
// Version     : 1.0
// Copyright   : 
// Description :  * This project shows the use of multi-threading in C++ to perform
// counting operation in two threads. One thread counts up to a max value, in this case 20,
// while the other thread waits for the first thread to complete before counting down.
//============================================================================

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <chrono>

class ThreadCounter {
private:
    // Mutex to lock shared resourced between threads
    std::mutex mtx;
    // Condition variable to signal when the first thread is done
    std::condition_variable cv;
    // Volatile counter to avoid optimization issues
    volatile int counter = 0;
    // Flag to track if the first thread (count up) is done
    bool firstThreadDone = false;
    // Constant for maximum count
    const int MAX_COUNT = 20;

    // Fixed-size buffer for formatted messages to avoid buffer overflow
    char messageBuffer[100];

public:
    //Thread function to count up from 0 to MAX_COUNT
    void countUp() {
        std::unique_lock<std::mutex> lock(mtx);

        while (counter < MAX_COUNT) {
            // Format the message securely to print the current count
            snprintf(messageBuffer, sizeof(messageBuffer),
                    "Thread 1 counting up: %d", counter);
            std::cout << messageBuffer << std::endl;

            counter++;

            // Simulate some work with a delay of 100 milliseconds
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        firstThreadDone = true;
        cv.notify_one();
    }

    void countDown() {
        std::unique_lock<std::mutex> lock(mtx);

        // Wait until first thread is done
        cv.wait(lock, [this]{ return firstThreadDone; });

        while (counter > 0) {
            // Format the message securely to print the current count
            snprintf(messageBuffer, sizeof(messageBuffer),
                    "Thread 2 counting down: %d", counter);
            std::cout << messageBuffer << std::endl;

            counter--;

            // Simulate some work with a delay of 100 milliseconds
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
};

int main() {
    ThreadCounter counter;

    // Create threads
    std::thread t1(&ThreadCounter::countUp, &counter);
    std::thread t2(&ThreadCounter::countDown, &counter);

    // Wait for threads to complete
    t1.join();
    t2.join();

    return 0;
}
