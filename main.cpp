#include <iostream>
#include <vector>
#include <thread>
#include <ctime>

#define RANDOM_MIN 0
#define RANDOM_MAX 1

template <typename T>
double sumVectorPart(const std::vector<T> &v, unsigned start, unsigned end) {
    double sum = 0;
    for (unsigned i = start; i < end; i++)
        sum += v[i];
    return sum;
}

template <typename T>
void generateRandomVector(std::vector<T> &v, T minVal, T maxVal) {
    for (unsigned i = 0; i < v.size(); i++)
        v[i] = minVal + rand() % (maxVal - minVal + 1);
}

int main() {
    srand(time(0));

    unsigned int workers = std::thread::hardware_concurrency();

    std::cout << "Accessible threads: " << workers << std::endl;

    std::vector<int> v(1000000);
    generateRandomVector(v, RANDOM_MIN, RANDOM_MAX);

    unsigned int partSize = v.size() / workers;

    std::vector<std::thread> threads(workers);
    std::vector<double> partialSums(workers, 0);

    for (unsigned int i = 0; i < workers; i++) {
        const unsigned int start = i * partSize;
        unsigned int end = start + partSize;
        if (i == workers - 1) end = v.size();

        threads[i] = std::thread([=, &partialSums]() {
            partialSums[i] = sumVectorPart(v, start, end);
        });
    }

    for (unsigned int i = 0; i < workers; i++) {
        threads[i].join();
    }

    double totalSum = 0;
    for (unsigned int i = 0; i < workers; i++) {
        totalSum += partialSums[i];
    }

    std::cout << "Sum of elements: " << totalSum << std::endl;

    return 0;
}
