#pragma once

#include <cstdint>
#include <vector>
#include <stdexcept>

namespace SerialComm
{
    template <size_t N>
    class RingBuffer
    {
    public:
        RingBuffer();
        ~RingBuffer() = default;

        /**
         * @brief Push a byte into the buffer.
         * @throws std::overflow_error if buffer is full.
         */
        void push(uint8_t data);

        /**
         * @brief Pop a byte from the buffer.
         * @return The oldest byte in the buffer.
         * @throws std::underflow_error if buffer is empty.
         */
        uint8_t pop();

        /**
         * @brief Checks if the buffer is empty.
         * @return True if no elements are stored, false otherwise.
         * @note This function does not modify the object.
         */
        bool empty() const noexcept;

        /**
         * @brief Checks if the buffer is full.
         * @return True if the buffer has reached its maximum capacity, false otherwise.
         * @note This function does not modify the object.
         */
        bool full() const noexcept;

        /**
         * @brief Returns the number of elements currently stored in the buffer.
         * @return The current size of the buffer.
         * @note This function does not modify the object.
         */
        size_t size() const noexcept;

        /**
         * @brief Returns the maximum capacity of the buffer.
         * @return The compile-time constant capacity (`N`).
         * @note This function is marked constexpr for optimization and does not modify the object.
         */
        constexpr size_t capacity() const noexcept;

    private:
        std::vector<uint8_t> buffer_;
        size_t head_;
        size_t tail_;
        size_t count_;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // Implementation of template methods is provided inline in the header.
    //
    // Since templates are instantiated at compile-time, their full definition
    // must be available when the compiler generates code for specific template
    // arguments. Placing implementations in a separate .cpp file would prevent
    // proper instantiation, leading to linker errors.
    //
    // Keeping template methods inline allows:
    // - Efficient specialization for different parameter values (`N`)
    // - Compile-time optimizations without unnecessary function calls
    // - Avoiding redundant code generation across translation units
    ////////////////////////////////////////////////////////////////////////////////

    template <size_t N>
    SerialComm::RingBuffer<N>::RingBuffer()
        : buffer_(N), head_(0), tail_(0), count_(0)
    {
    }

    template <size_t N>
    void SerialComm::RingBuffer<N>::push(uint8_t data)
    {
        if (full())
        {
            throw std::overflow_error("Buffer is full");
        }

        buffer_[head_] = data;
        head_ = (head_ + 1) % N;
        count_++;
    }

    template <size_t N>
    uint8_t SerialComm::RingBuffer<N>::pop()
    {
        if (empty())
        {
            throw std::underflow_error("Buffer is empty");
        }

        uint8_t data = buffer_[tail_];
        tail_ = (tail_ + 1) % N;
        --count_;
        return data;
    }

    template <size_t N>
    bool SerialComm::RingBuffer<N>::empty() const noexcept
    {
        return count_ == 0;
    }

    template <size_t N>
    bool SerialComm::RingBuffer<N>::full() const noexcept
    {
        return count_ == N;
    }

    template <size_t N>
    size_t SerialComm::RingBuffer<N>::size() const noexcept
    {
        return count_;
    }

    template <size_t N>
    constexpr size_t SerialComm::RingBuffer<N>::capacity() const noexcept
    {
        return N;
    }

}; // namespace SerialComm
