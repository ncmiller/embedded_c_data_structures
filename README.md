# embedded_c_data_structures

Minimal header-file-only data structures which can be useful on
embedded systems.

All memory required for these data structures is allocated by the user.
There are no restrictions on how the user allocates the data structure.
This can be useful in embedded environments where dynamic memory allocation
is forbidden or discouraged.

Feel free to copy these files to your project, but retain the
license header at the top of the file. If you make modifications,
add a line to the copyright section, for example:

```diff
 /*
  * Copyright (c) 2023 Nick Miller
+ * Copyright (c) 2023 Acme Corp.
  *
  * SPDX-License-Identifier: Apache-2.0
  */
```

## slist

An intrusive single-linked list (header-file only).

This was [copied from zephyrrtos](https://github.com/zephyrproject-rtos/zephyr/blob/zephyr-v3.3.0/include/zephyr/sys/slist.h)
and modified to be a standalone module with no dependencies on zephyrrtos.

This is not thread-safe. If used across threads, be sure to protect with
synchronization primitives.

The struct `snode_t` contains only a single pointer. The struct `slist_t`
contains only two pointers (head and tail).

| Operation | Time Complexity |
| --- | --- |
| Access head/tail | O(1) |
| Insert before head, after tail | O(1) |
| Insert in middle | O(N), requires prev pointer |
| Remove head | O(1) |
| Remove non-head nodes | O(N), requires prev pointer |

Example code:

```c
#include "slist.h"

// Add an snode_t to your struct to make it "link-listable"
struct my_container {
    int my_data;
    snode_t node;
};

int main(void) {
    struct my_container a = { .my_data = 14 };
    struct my_container b = { .my_data = 27 };
    struct my_container c = { .my_data = 35 };

    // Create a list
    slist_t my_list;
    slist_init(&my_list);

    // Add some items to the list
    slist_append(&my_list, &a.node);
    slist_append(&my_list, &b.node);
    slist_append(&my_list, &c.node);

    // Iterate over the list
    struct my_container* container;
    int count = 0;
    SLIST_FOR_EACH_CONTAINER(&my_list, container, node) {
        printf("item %d = %d\n", count, container->my_data);
        count++;
    }

    // prints:
    //
    // item 0 = 14
    // item 1 = 27
    // item 2 = 35

    // Remove the middle item
    slist_find_and_remove(&my_list, &b.node);

    // Iterate over the list again
    count = 0;
    SLIST_FOR_EACH_CONTAINER(&my_list, container, node) {
        printf("item %d = %d\n", count, container->my_data);
        count++;
    }

    // prints:
    //
    // item 0 = 14
    // item 1 = 35
}
```

See the
[Zephyr RTOS single-linked list documentation](https://docs.zephyrproject.org/3.3.0/kernel/data_structures/slist.html)
for more information.

## dlist

An intrusive double-linked list (header-file only).

Similar to slist, but allows for constant time remove and insert.

The struct `dnode_t` contains only two pointers (prev and next).
The struct `dlist_t` contains only two pointers (head and tail).

This was [copied from zephyrrtos](https://github.com/zephyrproject-rtos/zephyr/blob/zephyr-v3.3.0/include/zephyr/sys/dlist.h)
and modified to be a standalone module with no dependencies on zephyrrtos.

This is not thread-safe. If used across threads, be sure to protect with
synchronization primitives.

| Operation | Time Complexity |
| --- | --- |
| Access head/tail | O(1) |
| Insert before head, after tail | O(1) |
| Insert in middle | O(1) |
| Remove head | O(1) |
| Remove non-head nodes | O(1) |

Example code is omitted for brevity. It's exactly the same as slist,
just replace `slist` with `dlist` and `snode` with `dnode` throughout.

See the
[Zephyr RTOS double-linked list documentation](https://docs.zephyrproject.org/3.3.0/kernel/data_structures/dlist.html)
for more information.
