#pragma once

#include <cstdint>

namespace NetherNet {
    struct View {
        const uint8_t* pData;
        size_t mLength;

    public:
        View(const uint8_t* data, size_t length) : pData(data), mLength(length) {}

        [[nodiscard]] size_t get_len() const { return mLength; }
        [[nodiscard]] const uint8_t* data() const { return pData; }
        [[nodiscard]] bool empty() const { return mLength == 0; }
    };
}