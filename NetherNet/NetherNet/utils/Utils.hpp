#pragma once

#include <chrono>
#include <mutex>

namespace NetherNet::Utils {
	std::chrono::steady_clock::time_point GetTimeNow();

    template <typename T>
    class ThreadSafe {
    public:
        class View;

        ThreadSafe(T value) : data(std::move(value)) {}

        View get() {
            return View(*this);  // Returns a View object, locking the mutex during access
        }

        const T& get() const {
            std::lock_guard<std::mutex> lock(mtx); // Lock the mutex
            return data;
        }

        class View {
        public:
            View(ThreadSafe<T>& ts) : lock(ts.mtx), data(ts.data) {}

            T& get() const {
                return data;
            }

        private:
            std::lock_guard<std::mutex> lock;
            T& data;
        };

    private:
        T data; 
        mutable std::mutex mtx;
    };

    using TimePoint = std::chrono::steady_clock::time_point;
}