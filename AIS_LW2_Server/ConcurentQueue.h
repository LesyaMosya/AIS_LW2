
#ifndef CONCURENTQUEUE_H
#define CONCURENTQUEUE_H
#include <queue>
#include <mutex>

template <typename T>
class conc_queue {
public:
    void push(T value);
    T pop();
    bool empty();
private:
    std::queue<T> queue;
    std::mutex mutex;
};
template<typename T>
void conc_queue<T>::push(T value) {
    std::unique_lock<std::mutex> lock(mutex);
    queue.push(value);
}




template<typename T>
T conc_queue<T>::pop() {
    std::unique_lock<std::mutex> lock(mutex);
    T value = queue.front();
    queue.pop();
    return value;
}

template<typename T>
bool conc_queue<T>::empty() {
    std::unique_lock<std::mutex> lock(mutex);
    return queue.empty();
}
#endif //CONCURENTQUEUE_H
