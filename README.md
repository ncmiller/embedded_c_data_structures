# embedded_c_data_structures

Minimal drop-in libraries for data structures which can be useful on
embedded systems.

All libraries have the following properties:

* No dependencies except a small subset of libc (e.g. memset)
* No dynamic memory allocations
* No I/O (e.g. stdio.h)
* No build system required. Simply copy the .h/.c to your project.

Most of these were copied from
[Zephyr RTOS](https://github.com/zephyrproject-rtos/zephyr)
and adapted to remove Zephyr-specific dependencies.

All memory required for these data structures is allocated by the user,
and lifetime is managed by the user.
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

| File | Description |
| --- | --- |
| slist.h | An intrusive single-linked list |
| dlist.h | An intrusive double-linked list |
| rbtree.h | An intrusive red-black tree |
| ringbuf.h | A circular FIFO queue, lock-free for single-producer, single-consumer usage |

## slist

An intrusive single-linked list (header-file only).

This is a flexible data structure that can also be used as a
queue (by using slist_append()) or stack (by using slist_prepend()).

This was [copied from zephyrrtos](https://github.com/zephyrproject-rtos/zephyr/blob/zephyr-v3.3.0/include/zephyr/sys/slist.h)
and modified to be a standalone module with no dependencies on zephyrrtos.

This is not thread-safe. If used across threads, be sure to protect with
synchronization primitives.

The struct `snode_t` contains only a single pointer. The struct `slist_t`
contains only two pointers (head and tail).

Simplified API:

```c
void slist_init(slist_t *list);
void slist_insert(slist_t *list, snode_t *prev, snode_t *node);
bool slist_find_and_remove(slist_t *list, snode_t *node);
void slist_prepend(slist_t *list, snode_t *node);
void slist_append(slist_t *list, snode_t *node);
snode_t *slist_peek_head(slist_t *list);
snode_t *slist_peek_tail(slist_t *list);
snode_t *slist_get(slist_t *list);
bool slist_is_empty(slist_t *list);
#define SLIST_FOR_EACH_CONTAINER(slist, container, node)
```

| Operation | Time Complexity |
| --- | --- |
| peek_head()/peek_tail() | O(1) |
| append()/prepend() | O(1) |
| search() | O(N) |
| insert() | O(N), requires prev pointer |
| remove() | O(N), requires prev pointer |
| get() | O(1) |

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
| peek_head()/peek_tail() | O(1) |
| append()/prepend() | O(1) |
| search() | O(N) |
| insert() | O(1) |
| remove() | O(1) |
| get() | O(1) |

Example code is omitted for brevity. It's exactly the same as slist,
just replace `slist` with `dlist` and `snode` with `dnode` throughout.

See the
[Zephyr RTOS double-linked list documentation](https://docs.zephyrproject.org/3.3.0/kernel/data_structures/dlist.html)
for more information.

## rbtree

An intrusive red-black tree (.h and .c file).

This is a self-balancing binary search tree, which can be used in a variety
of ways (e.g. dictionary, priority queue).

This was [copied from zephyrrtos](https://github.com/zephyrproject-rtos/zephyr/blob/zephyr-v3.3.0/include/zephyr/sys/rb.h)
and modified to be a standalone module with no dependencies on zephyrrtos.

This is not thread-safe. If used across threads, be sure to protect with
synchronization primitives.

Simplified API:

```c
void rb_init(struct rbtree *tree, rb_cmp_t compare_fn);
void rb_insert(rbtree_t *tree, rbnode_t *node);
void rb_remove(rbtree_t *tree, rbnode_t *node);
rbnode_t *rb_get_min(rbtree_t *tree);
rbnode_t *rb_get_max(rbtree_t *tree);
rbnode_t *rb_find(rbtree_t *tree, rbnode_t* *query);
bool rb_contains(rbtree_t *tree, rbnode_t *node);
#define RB_FOR_EACH_CONTAINER(tree, node, field)
```

| Operation | Time Complexity |
| --- | --- |
| insert() | O(lg n) |
| remove() | O(lg n) |
| find() | O(lg n) |

Example code, using an rbtree as a key-value dictionary:

```c
#include "rbtree.h"

// Add a rbnode_t to your struct to make it possible to use in a rbtree
struct my_container {
    int key;
    int value;
    rbnode_t node;
};

// Comparison function, should return:
//    <0 if a < b
//    0  if a == b
//    >0 if a > b
int my_container_cmp(rbnode_t *a, rbnode_t *b) {
    // Get pointer to the struct containing a and b (i.e. struct my_container)
    struct my_container *cont_a = CONTAINER_OF(a, struct my_container, node);
    struct my_container *cont_b = CONTAINER_OF(b, struct my_container, node);
    return cont_a->key - cont_b->key;
}

int rbtree(void) {
    struct my_container a = { .key = 75, .value = 76 };
    struct my_container b = { .key = 13, .value = 14 };
    struct my_container c = { .key = 1, .value = 99 };
    struct my_container d = { .key = 56, .value = 17 };

    // Create and initialize a rbtree
    rbtree_t my_rbtree;
    rb_init(&my_rbtree, my_container_cmp);

    // Add items to the rbtree
    rb_insert(&my_rbtree, &a.node);
    rb_insert(&my_rbtree, &b.node);
    rb_insert(&my_rbtree, &c.node);
    rb_insert(&my_rbtree, &d.node);

    // In-order traversal of rbtree, smallest to largest
    struct my_container* container;
    int count = 0;
    RB_FOR_EACH_CONTAINER(&my_rbtree, container, node) {
        printf("[%d], key %d value %d\n", count, container->key, container->value);
        count++;
    }

    // prints:
    //
    // [0], key 1 value 99
    // [1], key 13 value 14
    // [2], key 56 value 17
    // [3], key 75 value 76

    // Find a item with key == 13
    struct my_container query = { .key = 13 };
    rbnode_t* found = rb_find(&my_rbtree, &query.node);

    // Print and remove the item just found
    if (found) {
        struct my_container *found_item = CONTAINER_OF(found, struct my_container, node);
        printf("found key %d, value = %d\n", found_item->key, found_item->value);
        rb_remove(&my_rbtree, found);
    }

    // Iterate over the rbtree again
    count = 0;
    RB_FOR_EACH_CONTAINER(&my_rbtree, container, node) {
        printf("[%d], key %d value %d\n", count, container->key, container->value);
        count++;
    }

    // prints:
    //
    // [0], key 1 value 99
    // [1], key 56 value 17
    // [2], key 75 value 76
}
```

See the
[Zephyr RTOS rbtree documentation](https://docs.zephyrproject.org/3.0.0/reference/data_structures/rbtree.html)
for more information.

## ringbuf

A ring buffer (aka circular FIFO queue).

This was [copied from golioth-firmware-sdk](https://raw.githubusercontent.com/golioth/golioth-firmware-sdk/v0.5.0/src/priv_include/golioth_ringbuf.h)
and modified to be header-file only.

This is thread-safe and lock-free for single-producer, single-consumer usage.
Otherwise, it is not thread-safe.

Simplified API:

```c
#define RINGBUF_DEFINE_AND_INIT(name, item_sz, max_num_items)
bool ringbuf_put(ringbuf_t* ringbuf, const void* item);
bool ringbuf_get(ringbuf_t* ringbuf, void* item);
bool ringbuf_peek(ringbuf_t* ringbuf, void* item);
size_t ringbuf_capacity(const ringbuf_t* ringbuf);
size_t ringbuf_size(const ringbuf_t* ringbuf);
bool ringbuf_is_empty(const ringbuf_t* ringbuf);
bool ringbuf_is_full(const ringbuf_t* ringbuf);
void ringbuf_reset(ringbuf_t* ringbuf);
```

| Operation | Time Complexity |
| --- | --- |
| put() | O(1) |
| get() | O(1) |
| size() | O(1) |
| reset() | O(1) |

Example code:

```c
#include "ringbuf.h"
#include <stdio.h>

struct my_container {
    int my_data;
};

int main(void) {
    // Create ringbuf that can store up to 8 instances of struct my_container.
    //
    // This will define two variables in the current scope:
    //
    // uint8_t my_ringbuf_buffer[sizeof(struct my_container) * 9];
    // ringbuf_t my_ringbuf;
    //
    // Note that the array is sized to contain one more item (8 + 1) than the
    // user requests, for internal implementation reasons.
    RINGBUF_DEFINE_AND_INIT(my_ringbuf, sizeof(struct my_container), 8);

    struct my_container a = { 1 };
    struct my_container b = { 2 };
    struct my_container c = { 3 };

    // Add items to the ringbuf
    ringbuf_put(&my_ringbuf, &a);
    ringbuf_put(&my_ringbuf, &b);
    ringbuf_put(&my_ringbuf, &c);

    printf("size = %zd\n", ringbuf_size(&my_ringbuf));
    // prints:
    //   "size = 3"

    // Get oldest item
    struct my_container the_item;
    ringbuf_get(&my_ringbuf, &the_item);

    printf("my_data = %d\n", the_item.my_data);
    // prints:
    //   "my_data = 1"

    printf("size = %zd\n", ringbuf_size(&my_ringbuf));
    // prints:
    //   "size = 2"
}
```

For more examples, see [test/test_ringbuf.c](test/test_ringbuf.c).
