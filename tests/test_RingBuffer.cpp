// tests/test_RingBuffer.cpp

#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "RingBuffer.hpp"

using namespace SerialComm;

///////////////////////////////////////////////////////////////////////////////
// TEST CASE 1: Basic push/pop behavior
///////////////////////////////////////////////////////////////////////////////

TEST_CASE("RingBuffer push and pop basic functionality", "[RingBuffer]")
{
    // Create a ring buffer with capacity 4
    RingBuffer<4> rb;

    // Initially, buffer should be empty
    REQUIRE(rb.empty());         // empty() should return true
    REQUIRE(!rb.full());         // full() should return false
    REQUIRE(rb.size() == 0);     // size() should be 0
    REQUIRE(rb.capacity() == 4); // capacity() should be 4

    // Push 4 values into the buffer
    rb.push(0x10);
    rb.push(0x20);
    rb.push(0x30);
    rb.push(0x40);

    // After pushing 4 items, buffer should be full
    REQUIRE(!rb.empty());    // empty() should now return false
    REQUIRE(rb.full());      // full() should return true
    REQUIRE(rb.size() == 4); // size() should be 4

    // Pop items one by one and check the order (FIFO behavior)
    uint8_t v1 = rb.pop();
    REQUIRE(v1 == 0x10); // First popped value must be 0x10
    uint8_t v2 = rb.pop();
    REQUIRE(v2 == 0x20); // Next popped value must be 0x20

    // After popping 2 items, size should be 2, but still not empty or full
    REQUIRE(rb.size() == 2);
    REQUIRE(!rb.empty());
    REQUIRE(!rb.full());

    // Push two more values to test wrap-around behavior
    rb.push(0x50);
    rb.push(0x60);

    // Buffer should be full again (4 items total)
    REQUIRE(rb.full());
    REQUIRE(rb.size() == 4);

    // Now pop the remaining items in FIFO order
    uint8_t v3 = rb.pop();
    REQUIRE(v3 == 0x30); // Third popped value must be 0x30
    uint8_t v4 = rb.pop();
    REQUIRE(v4 == 0x40); // Fourth popped value must be 0x40
    uint8_t v5 = rb.pop();
    REQUIRE(v5 == 0x50); // Fifth popped value must be 0x50
    uint8_t v6 = rb.pop();
    REQUIRE(v6 == 0x60); // Sixth popped value must be 0x60

    // After popping all items, buffer should be empty again
    REQUIRE(rb.empty());
    REQUIRE(!rb.full());
    REQUIRE(rb.size() == 0);
}

///////////////////////////////////////////////////////////////////////////////
// TEST CASE 2: Underflow and overflow exceptions
///////////////////////////////////////////////////////////////////////////////

TEST_CASE("RingBuffer underflow and overflow exceptions", "[RingBuffer]")
{
    // Create a ring buffer with capacity 2
    RingBuffer<2> rb;

    // Attempt to pop from an empty buffer should throw underflow_error
    REQUIRE_THROWS_AS(rb.pop(), std::underflow_error);

    // Push up to capacity (2 items)
    rb.push(0xAA);
    rb.push(0xBB);

    // Buffer is now full; pushing another should throw overflow_error
    REQUIRE(rb.full());
    REQUIRE_THROWS_AS(rb.push(0xCC), std::overflow_error);

    // Pop one item to make space
    uint8_t first = rb.pop();
    REQUIRE(first == 0xAA);

    // Now buffer has one slot free; pushing should succeed
    rb.push(0xDD);
    REQUIRE(rb.size() == 2);
    REQUIRE(rb.full());

    // Pop remaining items in FIFO order
    uint8_t second = rb.pop();
    REQUIRE(second == 0xBB);
    uint8_t third = rb.pop();
    REQUIRE(third == 0xDD);

    // After popping all, buffer should be empty
    REQUIRE(rb.empty());
    REQUIRE(!rb.full());
    REQUIRE(rb.size() == 0);

    // Popping again should throw underflow_error
    REQUIRE_THROWS_AS(rb.pop(), std::underflow_error);
}