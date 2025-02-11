
/*
* Source code from:
* https://github.com/LiteLDev/LeviLamina/blob/main/src/mc/deps/core/utility/NonOwnerPointer.h
*/

#pragma once
#include <compare>

#include "EnableNonOwnerReferences.hpp"

namespace Bedrock {

    template <typename T>
    class NonOwnerPointer {
    public:
        std::shared_ptr<Bedrock::EnableNonOwnerReferences::ControlBlock> mControlBlock;
        T* mPointer{};
        NonOwnerPointer(std::shared_ptr<Bedrock::EnableNonOwnerReferences::ControlBlock> cb, T* p)
            : mControlBlock(std::move(cb)),
            mPointer(p) {
        }
        NonOwnerPointer() noexcept {}
        NonOwnerPointer(std::nullptr_t) noexcept {}
        T* get() const {
            if (mControlBlock && mControlBlock->mIsValid) {
                return mPointer;
            }
            return nullptr;
        }
        explicit operator bool() const noexcept { return get() != nullptr; }

        [[nodiscard]] constexpr    operator T* () const { return get(); }
        [[nodiscard]] constexpr T* operator->() const { return get(); }
        [[nodiscard]] constexpr T& operator*() const { return *get(); }
    };

    template <class T>
    [[nodiscard]] bool operator==(NonOwnerPointer<T> const& self, nullptr_t) noexcept {
        return self.get() == nullptr;
    }
    template <class T>
    [[nodiscard]] std::strong_ordering operator<=>(NonOwnerPointer<T> const& self, nullptr_t) noexcept {
        return self.get() <=> static_cast<T*>(nullptr);
    }

    template <class T1, class T2>
    [[nodiscard]] bool operator==(NonOwnerPointer<T1> const& l, NonOwnerPointer<T2> const& r) noexcept {
        return l.get() == r.get();
    }
    template <class T1, class T2>
    [[nodiscard]] std::strong_ordering operator<=>(NonOwnerPointer<T1> const& l, NonOwnerPointer<T2> const& r) noexcept {
        return l.get() <=> r.get();
    }

}; // namespace Bedrock