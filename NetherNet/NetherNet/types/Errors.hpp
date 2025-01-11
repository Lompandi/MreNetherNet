#pragma once

namespace NetherNet {
    template <typename T, typename E>
    struct ErrorOr {
        union {
            T val;
            E err;
        };
        bool is_err;

        ErrorOr() : is_err(false), val() {}
        ErrorOr(const T& value) : is_err(false), val(value) {}
        ErrorOr(const E& error) : is_err(true), err(error) {}

        ~ErrorOr() {
            if (is_err) {
                err.~E();
            }
            else {
                val.~T();
            }
        }

        bool is_error() const { return is_err; }

        T unwrap() {
            if (is_err)
                throw std::runtime_error("Attempted to unwrap an error");
            return val;
        }

        E unwrap_error() {
            if (!is_err)
                throw std::runtime_error("Attempted to unwrap a value");
            return err;
        }

        template <typename F>
        auto map(F&& func) -> ErrorOr<decltype(func(val)), E> {
            if (is_err) 
                return ErrorOr<decltype(func(val)), E>(err);  
            return ErrorOr<decltype(func(val)), E>(func(val));  
        }

        template <typename F>
        auto map_error(F&& func) -> ErrorOr<T, decltype(func(err))> {
            if (!is_err)
                return *this;  
            return ErrorOr<T, decltype(func(err))>(func(err));  
        }

        static ErrorOr<T, E> success(const T& value) {
            return ErrorOr<T, E>(value);
        }

        E& error() {
            return err;
        }

        T& value() {
            return val;
        }

        static ErrorOr<T, E> error(const E& error) {
            return ErrorOr<T, E>(error);
        }
    };
}