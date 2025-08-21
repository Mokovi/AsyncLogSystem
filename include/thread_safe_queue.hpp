#pragma once

namespace log_system {

template<typename T>
void ThreadSafeQueue<T>::push(const T& item) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(item);
    condition_.notify_one();
}

template<typename T>
bool ThreadSafeQueue<T>::pop(T& item) {
    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(lock, [this] { return !queue_.empty(); });
    
    if (queue_.empty()) {
        return false;
    }
    
    item = std::move(queue_.front());
    queue_.pop();
    return true;
}

template<typename T>
bool ThreadSafeQueue<T>::tryPop(T& item) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (queue_.empty()) {
        return false;
    }
    
    item = std::move(queue_.front());
    queue_.pop();
    return true;
}

template<typename T>
bool ThreadSafeQueue<T>::empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
}

template<typename T>
size_t ThreadSafeQueue<T>::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}

template<typename T>
void ThreadSafeQueue<T>::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    while (!queue_.empty()) {
        queue_.pop();
    }
}

template<typename T>
bool ThreadSafeQueue<T>::waitForData(int timeout_ms) {
    std::unique_lock<std::mutex> lock(mutex_);
    
    if (timeout_ms <= 0) {
        condition_.wait(lock, [this] { return !queue_.empty(); });
        return true;
    } else {
        return condition_.wait_for(lock, 
                                 std::chrono::milliseconds(timeout_ms),
                                 [this] { return !queue_.empty(); });
    }
}

} // namespace log_system
