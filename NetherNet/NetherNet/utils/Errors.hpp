#pragma once

#include <variant>

namespace NetherNet {
    template<class E>
    class result_error {
        template<typename T, typename R>
        friend class ErrorOr;
    public:
        constexpr explicit result_error(const E& t) : error(t) {}
        constexpr explicit result_error(E&& t) : error(std::move(t)) {}

        constexpr result_error(const result_error<E>&) = default;
        constexpr result_error(result_error<E>&&) noexcept = default;
    private:
        E error;
    };

    template <class T, class E>
    class ErrorOr {
    public:
        constexpr ErrorOr(const T& t) : impl(std::in_place_index<0>, t) {}
        constexpr ErrorOr(T&& t) : impl(std::in_place_index<0>, std::move(t)) {}

        constexpr ErrorOr(const result_error<E>& e) : impl(std::in_place_index<1>, e.error) {}
        constexpr ErrorOr(result_error<E>&& e) : impl(std::in_place_index<1>, std::move(e.error)) {}

        constexpr ErrorOr(const ErrorOr<T, E>&) = default;
        constexpr ErrorOr(ErrorOr<T, E>&&) noexcept = default;

        [[nodiscard]] constexpr bool has_value() const {
            return impl.index() == 0;
        }

        constexpr T& value() noexcept {
            return std::get<0>(impl);
        }

        constexpr const T& value() const noexcept {
            return std::get<0>(impl);
        }

        constexpr E& error() noexcept {
            return std::get<1>(impl);
        }

        constexpr const E& error() const noexcept {
            return std::get<1>(impl);
        }

    private:
        std::variant<T, E> impl;
    };

    template <class E>
    class ErrorOr<void, E> {
    public:
        constexpr ErrorOr() : impl(std::in_place_index<0>) {} 
        constexpr ErrorOr(const result_error<E>& e) : impl(std::in_place_index<1>, e.error) {}
        constexpr ErrorOr(result_error<E>&& e) : impl(std::in_place_index<1>, std::move(e.error)) {}

        constexpr ErrorOr(const ErrorOr<void, E>&) = default;
        constexpr ErrorOr(ErrorOr<void, E>&&) noexcept = default;

        [[nodiscard]] constexpr bool has_value() const {
            return impl.index() == 0;
        }

        constexpr E& error() noexcept {
            return std::get<1>(impl);
        }

        constexpr const E& error() const noexcept {
            return std::get<1>(impl);
        }

    private:
        std::variant<std::monostate, E> impl;  
    };
}