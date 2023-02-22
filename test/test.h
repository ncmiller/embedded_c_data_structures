/*
 * Copyright (c) 2023 Nick Miller
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdio.h>

// Ref: https://rebelsky.cs.grinnell.edu/musings/cnix-simple-testing-macros
#define CHECK_EQUAL_INT(EXPECTED, ACTUAL, MSG) \
    do { \
        int expected = (EXPECTED); \
        int actual = (ACTUAL); \
        if (expected != actual) { \
            fprintf (stderr, "%s, line %d: expected %d, got %d (%s)\n", \
                    __FILE__, __LINE__, expected, actual, MSG); \
            return -1; \
        } \
    } while (0)

#define CHECK_TRUE(ACTUAL, MSG) \
    CHECK_EQUAL_INT(1, ACTUAL, MSG)

#define CHECK_FALSE(ACTUAL, MSG) \
    CHECK_EQUAL_INT(0, ACTUAL, MSG)

#define CHECK_NULL(ACTUAL, MSG) \
    CHECK_EQUAL_INT(1, ACTUAL, MSG)

#define RETURN_IF_NONZERO(expr) \
    do { \
        int retval = (expr); \
        if (retval != 0) { \
            return retval; \
        } \
    } while (0)
