#include <gtest/gtest.h>

#include "Circular-Buffer/ringbuf.hpp"

TEST(RingBuf, IsWorking) {
    static const size_t CAPACITY = 32;
    uint8_t buf[CAPACITY];
    ringbuf_t ringbuf;

    /// To create a ringbuf instance, you must provide a pointer to a ringbuf_t,
    /// a pointer to an allocated array and it's size, which will be the capacity
    /// (max size) of the circular buffer.
    {
        ringbuf_Error_t err = ringbuf_new(&ringbuf, buf, CAPACITY);
        EXPECT_EQ(ringbuf_Error_Ok, err);
    }

    /// Use ringbuf_get_capacity to get it's capacity.
    {
        size_t capacity = ringbuf_get_capacity(&ringbuf);
        EXPECT_EQ(CAPACITY, capacity);
    }

    /// Use ringbuf_get_size to get it's size.
    {
        size_t size = ringbuf_get_size(&ringbuf);
        EXPECT_EQ(0, size);
    }

    /// Use ringbuf_is_empty to know if it is empty.
    {
        bool is_empty = ringbuf_is_empty(&ringbuf);
        EXPECT_EQ(true, is_empty);
    }

    /// Use ringbuf_push to add an item.
    {
        ringbuf_Error_t err = ringbuf_push(&ringbuf, 42);
        EXPECT_EQ(ringbuf_Error_Ok, err);
    }

    /// The size increases after adding an item.
    {
        size_t size = ringbuf_get_size(&ringbuf);
        EXPECT_EQ(1, size);
    }

    /// The number of available slots decrease after adding an item.
    {
        size_t size = ringbuf_get_available(&ringbuf);
        EXPECT_EQ(CAPACITY - 1, size);
    }

    /// Use ringbuf_is_full to know if it is full.
    /// We didn't add many items, so it is still not full.
    {
        bool is_full = ringbuf_is_full(&ringbuf);
        EXPECT_EQ(false, is_full);
    }

    /// Use ringbuf_pop to get the first inserted item.
    {
        uint8_t value = 0;
        ringbuf_Error_t err = ringbuf_pop(&ringbuf, &value);
        EXPECT_EQ(ringbuf_Error_Ok, err);
        EXPECT_EQ(42, value);
    }

    /// The size decreases after removing an item.
    {
        size_t size = ringbuf_get_size(&ringbuf);
        EXPECT_EQ(0, size);
    }

    /// You can add items until the size reaches the capacity.
    {
        for (int i = 0; i < CAPACITY; ++i) {
            ringbuf_Error_t err = ringbuf_push(&ringbuf, (uint8_t)i);
            EXPECT_EQ(ringbuf_Error_Ok, err);
        }
    }

    /// After that, it is full.
    {
        bool is_full = ringbuf_is_full(&ringbuf);
        EXPECT_EQ(true, is_full);
    }

    /// The number of available slots is zero when it is full.
    {
        size_t size = ringbuf_get_available(&ringbuf);
        EXPECT_EQ(0, size);
    }

    /// You cannot add more items if it is full.
    {
        ringbuf_Error_t err = ringbuf_push(&ringbuf, 1);
        EXPECT_EQ(ringbuf_Error_Full, err);
    }

    /// Obviously, it is not empty when it is full
    /// We have this test, because head==tail when the buffer is full
    /// and also when the buffer is empty.
    {
        bool is_empty = ringbuf_is_empty(&ringbuf);
        EXPECT_EQ(false, is_empty);
    }

    /// When full, size is equal to capacity.
    {
        size_t size = ringbuf_get_size(&ringbuf);
        EXPECT_EQ(CAPACITY, size);
    }

    /// You can pop as many items it contains.
    {
        size_t size = ringbuf_get_size(&ringbuf);

        for (int i = 0; i < size; ++i) {
            uint8_t value = 0;
            ringbuf_Error_t err = ringbuf_pop(&ringbuf, &value);
            EXPECT_EQ(ringbuf_Error_Ok, err);
            EXPECT_EQ(i, value);
        }
    }

    /// And after that, it will be empty again. When it is empty you cannot pop
    /// other items from it.
    {
        bool is_empty = ringbuf_is_empty(&ringbuf);
        EXPECT_EQ(true, is_empty);

        uint8_t value = 0;
        ringbuf_Error_t err = ringbuf_pop(&ringbuf, &value);
        EXPECT_EQ(ringbuf_Error_Empty, err);
    }

    /// If you push and pop immediately, the size is 0. You can push/pop
    /// indefinitely.
    {
        size_t repeat = CAPACITY * 2;

        for (size_t i = 0; i < repeat; ++i) {
            uint8_t value = 0;
            EXPECT_EQ(ringbuf_Error_Ok, ringbuf_push(&ringbuf, (uint8_t)i));
            EXPECT_EQ(1, ringbuf_get_size(&ringbuf));
            EXPECT_EQ(ringbuf_Error_Ok, ringbuf_pop(&ringbuf, &value));
            EXPECT_EQ(0, ringbuf_get_size(&ringbuf));
            EXPECT_EQ(i, value);
        }
    }
}

TEST(RingBuf, InvalidArguments) {
    static const size_t CAPACITY = 32;
    uint8_t buf[CAPACITY];
    ringbuf_t ringbuf;

    EXPECT_EQ(ringbuf_Error_InvalidArgument, ringbuf_new(NULL, buf, CAPACITY));
    EXPECT_EQ(ringbuf_Error_InvalidArgument, ringbuf_new(&ringbuf, NULL, CAPACITY));
    EXPECT_EQ(ringbuf_Error_InvalidArgument, ringbuf_new(&ringbuf, buf, 0));
}
