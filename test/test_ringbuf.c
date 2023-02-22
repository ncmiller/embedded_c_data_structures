/*
 * Copyright (c) 2022 Golioth, Inc.
 * Copyright (c) 2023 Nick Miller
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "test.h"
#include "ringbuf.h"

int empty_ringbuf_has_zero_size(void) {
    RINGBUF_DEFINE_AND_INIT(rb, 1, 8);
    CHECK_EQUAL_INT(0, ringbuf_size(&rb), "");
    CHECK_TRUE(ringbuf_is_empty(&rb), "");
    return 0;
}

int capacity_is_what_i_asked_for(void) {
    RINGBUF_DEFINE_AND_INIT(rb, 1, 8);
    CHECK_EQUAL_INT(8, ringbuf_capacity(&rb), "");
    return 0;
}

int get_returns_the_oldest_item(void) {
    RINGBUF_DEFINE_AND_INIT(rb, 1, 8);

    uint8_t item1 = 4;
    uint8_t item2 = 5;

    uint8_t item;
    CHECK_TRUE(ringbuf_put(&rb, &item1), "");
    CHECK_TRUE(ringbuf_put(&rb, &item2), "");
    CHECK_TRUE(ringbuf_get(&rb, &item), "");
    CHECK_EQUAL_INT(4, item, "");
    return 0;
}

int get_when_empty_fails(void) {
    RINGBUF_DEFINE_AND_INIT(rb, 1, 8);
    uint8_t item;
    CHECK_FALSE(ringbuf_get(&rb, &item), "");
    return 0;
}

int put_when_full_fails(void) {
    const size_t maxitems = 8;
    RINGBUF_DEFINE_AND_INIT(rb, 1, maxitems);

    uint8_t item = 0;
    for (size_t i = 0; i < maxitems; i++) {
        CHECK_TRUE(ringbuf_put(&rb, &item), "");
    }
    CHECK_TRUE(ringbuf_is_full(&rb), "");
    CHECK_FALSE(ringbuf_put(&rb, &item), "");
    return 0;
}

int can_peek(void) {
    RINGBUF_DEFINE_AND_INIT(rb, 1, 8);

    uint8_t item1 = 4;
    uint8_t item2 = 5;

    uint8_t item;
    CHECK_TRUE(ringbuf_put(&rb, &item1), "");
    CHECK_TRUE(ringbuf_put(&rb, &item2), "");
    CHECK_TRUE(ringbuf_peek(&rb, &item), "");
    CHECK_EQUAL_INT(4, item, "");
    CHECK_EQUAL_INT(2, ringbuf_size(&rb), "");
    return 0;
}

int can_reset(void) {
    RINGBUF_DEFINE_AND_INIT(rb, 1, 8);
    uint8_t item = 0;
    CHECK_TRUE(ringbuf_put(&rb, &item), "");
    CHECK_EQUAL_INT(1, ringbuf_size(&rb), "");
    ringbuf_reset(&rb);
    CHECK_EQUAL_INT(0, ringbuf_size(&rb), "");
    return 0;
}

int array_wraparound(void) {
    // Verify the size is reported correctly when the write pointer wraps
    // around in the internal array.
    //
    // To force wraparound, fill the ringbuf, read an item, then write an item
    RINGBUF_DEFINE_AND_INIT(rb, 1, 2);
    uint8_t item1 = 1;
    uint8_t item2 = 2;
    uint8_t item3 = 3;
    uint8_t rd_item;
    CHECK_TRUE(ringbuf_put(&rb, &item1), "");
    CHECK_TRUE(ringbuf_put(&rb, &item2), "");
    CHECK_TRUE(ringbuf_is_full(&rb), "");
    CHECK_TRUE(ringbuf_get(&rb, &rd_item), "");
    CHECK_EQUAL_INT(1, rd_item, "");
    CHECK_TRUE(ringbuf_put(&rb, &item3), "");
    CHECK_TRUE(ringbuf_get(&rb, &rd_item), "");
    CHECK_EQUAL_INT(2, rd_item, "");
    CHECK_TRUE(ringbuf_get(&rb, &rd_item), "");
    CHECK_EQUAL_INT(3, rd_item, "");
    CHECK_TRUE(ringbuf_is_empty(&rb), "");
    return 0;
}

int put_when_null_item_fails(void) {
    RINGBUF_DEFINE_AND_INIT(rb, 1, 1);
    CHECK_FALSE(ringbuf_put(&rb, NULL), "");
    return 0;
}

int main(void) {
    RETURN_IF_NONZERO(empty_ringbuf_has_zero_size());
    RETURN_IF_NONZERO(capacity_is_what_i_asked_for());
    RETURN_IF_NONZERO(get_returns_the_oldest_item());
    RETURN_IF_NONZERO(get_when_empty_fails());
    RETURN_IF_NONZERO(put_when_full_fails());
    RETURN_IF_NONZERO(put_when_null_item_fails());
    RETURN_IF_NONZERO(array_wraparound());
    RETURN_IF_NONZERO(can_peek());
    RETURN_IF_NONZERO(can_reset());
    return 0;
}
