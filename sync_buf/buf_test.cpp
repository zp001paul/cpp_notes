#include "unsync_buf.hpp"
#include <boost/format.hpp>
#include <chrono>
#include <iostream>
#include <random>
#include <thread>

int main(int argc, char *argv[]) {
    UnsyncBuf buf;
    auto produce{[&buf]() {
        std::random_device ran_dev;
        std::default_random_engine ran_eng{ran_dev()};
        std::uniform_int_distribution<> ints{0, 3000};

        int sum{0};

        for (int count{1}; count <= 10; ++count) {
            std::chrono::milliseconds sleep_time{ints(ran_eng)};
            std::this_thread::sleep_for(sleep_time);

            buf.put(count);
            sum += count;
            std::cout << boost::format("producer put:\t\t%1%\t%2%\n") % count %
                             sum;
        }
        std::cout << "producer done" << std::endl;
    }};

    auto consume{[&buf]() {
        std::random_device ran_dev;
        std::default_random_engine ran_eng{ran_dev()};
        std::uniform_int_distribution<> ints{0, 3000};

        int sum{0};
        int value;

        for (int count{1}; count <= 10; ++count) {
            std::chrono::milliseconds sleep_time{ints(ran_eng)};
            std::this_thread::sleep_for(sleep_time);

            value = buf.get();
            sum += value;
            std::cout << boost::format("consumer get:\t\t%1%\t\t%2%\n") %
                             value % sum;
        }
        std::cout << "consumer done" << std::endl;
    }};

    std::cout << "Action\t\t\tValue\tSumOfProduced\tSumOfConsumed\n";
    std::cout << "------\t\t\t-----\t-------------\t-------------\n";

    std::jthread producer{produce};
    std::jthread consumer{consume};

    return 0;
}
