#include <iostream>
#include <vector>
#include <thread>
#include <ctime>

#define RANDOM_MIN 0
#define RANDOM_MAX 2
#define PRECISION 12

template <typename T>
double sumVectorPart(const std::vector<T> &v, unsigned start, unsigned end) {
    double sum = 0;
    for (unsigned i = start; i < end; i++)
        sum += v[i];
    return sum;
}

void generateRandomVector(std::vector<int> &v) {
    for (auto &x : v)
        x = RANDOM_MIN + rand() % (RANDOM_MAX - RANDOM_MIN + 1);
}

void generateRandomVector(std::vector<double> &v) {
    for (auto &x : v)
        x = RANDOM_MIN + (RANDOM_MAX - RANDOM_MIN) * (rand() / (double)RAND_MAX);
}

int main() {
    std::cout.precision(PRECISION);
    srand(time(0));

    unsigned int workers = std::thread::hardware_concurrency();

    std::cout << "Accessible threads: " << workers << std::endl;

    std::vector<double> v(1000000);
    generateRandomVector(v);

    unsigned int partSize = v.size() / workers;

    auto start1 = std::chrono::high_resolution_clock::now();
    double sum = 0;
    for (double &i : v)
        sum += i;

    auto end1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> oneThreadTotal = end1 - start1;

    std::cout << "Sum of elements (1 thread): " << sum << std::endl;
    std::cout <<"Total time (1 thread):"<<oneThreadTotal.count()<<std::endl;

    std::vector<std::thread> threads(workers);
    std::vector<double> partialSums(workers, 0);

    auto start2 = std::chrono::high_resolution_clock::now();

    for (unsigned int i = 0; i < workers; i++) {
        const unsigned int start = i * partSize;
        unsigned int end = start + partSize;
        if (i == workers - 1) end = v.size();

        threads[i] = std::thread([&v, &partialSums, start, end, i]() {
            partialSums[i] = sumVectorPart(v, start, end);
        });
    }

    for (unsigned int i = 0; i < workers; i++)
        threads[i].join();

    double totalSum = 0;
    for (unsigned int i = 0; i < workers; i++)
        totalSum += partialSums[i];

    auto end2 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> multiThreadTotal = end2 - start2;

    std::cout << "Sum of elements (multi-thread): " << totalSum << std::endl;
    std::cout <<"Total time (multi-thread):"<<multiThreadTotal.count()<<std::endl;

    return 0;
}
