/*
 * Copyright (c) 2016 Intel Corporation
 * Copyright (c) 2023 Nick Miller
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "test.h"
#include "slist.h"

static slist_t test_list;
static slist_t append_list;

struct container_node {
    snode_t node;
    int unused;
};

static struct container_node test_node_1;
static struct container_node test_node_2;
static struct container_node test_node_3;
static struct container_node test_node_4;

static inline bool verify_emptyness(slist_t *list)
{
    snode_t *node;
    snode_t *s_node;
    struct container_node *cnode;
    struct container_node *s_cnode;
    int count;

    if (!slist_is_empty(list)) {
        return false;
    }

    if (slist_peek_head(list)) {
        return false;
    }

    if (slist_peek_tail(list)) {
        return false;
    }

    count = 0;
    SLIST_FOR_EACH_NODE(list, node) {
        count++;
    }

    if (count) {
        return false;
    }

    SLIST_FOR_EACH_NODE_SAFE(list, node, s_node) {
        count++;
    }

    if (count) {
        return false;
    }

    count = 0;
    SLIST_FOR_EACH_CONTAINER(list, cnode, node) {
        count++;
    }

    if (count) {
        return false;
    }

    count = 0;
    SLIST_FOR_EACH_CONTAINER_SAFE(list, cnode, s_cnode, node) {
        count++;
    }

    if (count) {
        return false;
    }

    return true;
}

static inline bool verify_content_amount(slist_t *list, int amount)
{
    snode_t *node;
    snode_t *s_node;
    struct container_node *cnode;
    struct container_node *s_cnode;
    int count;

    if (slist_is_empty(list)) {
        return false;
    }

    if (!slist_peek_head(list)) {
        return false;
    }

    if (!slist_peek_tail(list)) {
        return false;
    }

    count = 0;
    SLIST_FOR_EACH_NODE(list, node) {
        count++;
    }

    if (count != amount) {
        return false;
    }

    count = 0;
    SLIST_FOR_EACH_NODE_SAFE(list, node, s_node) {
        count++;
    }

    if (count != amount) {
        return false;
    }

    count = 0;
    SLIST_FOR_EACH_CONTAINER(list, cnode, node) {
        count++;
    }

    if (count != amount) {
        return false;
    }

    count = 0;
    SLIST_FOR_EACH_CONTAINER_SAFE(list, cnode, s_cnode, node) {
        count++;
    }

    if (count != amount) {
        return false;
    }

    return true;
}

static inline bool verify_tail_head(slist_t *list,
        snode_t *head,
        snode_t *tail,
        bool same)
{
    if (slist_peek_head(list) != head) {
        return false;
    }

    if (slist_peek_tail(list) != tail) {
        return false;
    }

    if (same) {
        if (slist_peek_head(list) != slist_peek_tail(list)) {
            return false;
        }
    } else {
        if (slist_peek_head(list) == slist_peek_tail(list)) {
            return false;
        }
    }

    return true;
}
/**
 * @addtogroup kernel_common_tests
 * @{
 */

/**
 * @brief Test singly linked list functionalities
 *
 * @details Test list initialization, append item to the list,
 * find and remove item, prepend, append, remove list
 *
 * @see slist_init(), slist_append(),
 * slist_find_and_remove(), slist_prepend(),
 * slist_remove(), slist_get(), slist_get_not_empty(),
 * slist_append_list(), slist_merge_list()
 */
static int test_slist(void) {
    slist_init(&test_list);

    CHECK_TRUE((verify_emptyness(&test_list)),
            "test_list should be empty");

    /* Appending node 1 */
    slist_append(&test_list, &test_node_1.node);
    CHECK_TRUE((verify_content_amount(&test_list, 1)),
            "test_list has wrong content");

    CHECK_TRUE((verify_tail_head(&test_list, &test_node_1.node,
                    &test_node_1.node, true)),
            "test_list head/tail are wrong");

    /* Finding and removing node 1 */
    slist_find_and_remove(&test_list, &test_node_1.node);
    CHECK_TRUE((verify_emptyness(&test_list)),
            "test_list should be empty");

    /* Prepending node 1 */
    slist_prepend(&test_list, &test_node_1.node);
    CHECK_TRUE((verify_content_amount(&test_list, 1)),
            "test_list has wrong content");

    CHECK_TRUE((verify_tail_head(&test_list, &test_node_1.node,
                    &test_node_1.node, true)),
            "test_list head/tail are wrong");

    /* Removing node 1 */
    slist_remove(&test_list, NULL, &test_node_1.node);
    CHECK_TRUE((verify_emptyness(&test_list)),
            "test_list should be empty");

    /* Appending node 1 */
    slist_append(&test_list, &test_node_1.node);
    /* Prepending node 2 */
    slist_prepend(&test_list, &test_node_2.node);

    CHECK_TRUE((verify_content_amount(&test_list, 2)),
            "test_list has wrong content");

    CHECK_TRUE((verify_tail_head(&test_list, &test_node_2.node,
                    &test_node_1.node, false)),
            "test_list head/tail are wrong");

    /* Appending node 3 */
    slist_append(&test_list, &test_node_3.node);

    CHECK_TRUE((verify_content_amount(&test_list, 3)),
            "test_list has wrong content");

    CHECK_TRUE((verify_tail_head(&test_list, &test_node_2.node,
                    &test_node_3.node, false)),
            "test_list head/tail are wrong");

    CHECK_TRUE((slist_peek_next(&test_node_2.node) ==
                &test_node_1.node),
            "test_list node links are wrong");

    /* Inserting node 4 after node 2, peek with nocheck variant */
    slist_insert(&test_list, &test_node_2.node, &test_node_4.node);

    CHECK_TRUE((verify_tail_head(&test_list, &test_node_2.node,
                    &test_node_3.node, false)),
            "test_list head/tail are wrong");

    CHECK_TRUE((slist_peek_next_no_check(&test_node_2.node) ==
                &test_node_4.node),
            "test_list node links are wrong");

    /* Finding and removing node 1 */
    slist_find_and_remove(&test_list, &test_node_1.node);
    CHECK_TRUE((verify_content_amount(&test_list, 3)),
            "test_list has wrong content");

    CHECK_TRUE((verify_tail_head(&test_list, &test_node_2.node,
                    &test_node_3.node, false)),
            "test_list head/tail are wrong");

    /* Removing node 3 */
    slist_remove(&test_list, &test_node_4.node, &test_node_3.node);
    CHECK_TRUE((verify_content_amount(&test_list, 2)),
            "test_list has wrong content");

    CHECK_TRUE((verify_tail_head(&test_list, &test_node_2.node,
                    &test_node_4.node, false)),
            "test_list head/tail are wrong");

    /* Removing node 4 */
    slist_remove(&test_list, &test_node_2.node, &test_node_4.node);
    CHECK_TRUE((verify_content_amount(&test_list, 1)),
            "test_list has wrong content");

    CHECK_TRUE((verify_tail_head(&test_list, &test_node_2.node,
                    &test_node_2.node, true)),
            "test_list head/tail are wrong");

    /* Removing node 2 */
    slist_remove(&test_list, NULL, &test_node_2.node);
    CHECK_TRUE((verify_emptyness(&test_list)),
            "test_list should be empty");

    /* test iterator from a node */
    struct data_node {
        snode_t node;
        int data;
    } data_node[6] = {
        { .data = 0 },
        { .data = 1 },
        { .data = 2 },
        { .data = 3 },
        { .data = 4 },
        { .data = 5 },
    };
    snode_t *node = NULL;
    int ii;

    slist_init(&test_list);

    for (ii = 0; ii < 6; ii++) {
        slist_append(&test_list, &data_node[ii].node);
    }

    ii = 0;
    SLIST_ITERATE_FROM_NODE(&test_list, node) {
        ii++;
        if (((struct data_node *)node)->data == 2) {
            break;
        }
    }
    CHECK_EQUAL_INT(ii, 3, "");

    ii = 0;
    SLIST_ITERATE_FROM_NODE(&test_list, node) {
        ii++;
        if (((struct data_node *)node)->data == 3) {
            break;
        }
    }
    CHECK_EQUAL_INT(ii, 1, "");

    ii = 0;
    SLIST_ITERATE_FROM_NODE(&test_list, node) {
        ii++;
    }
    CHECK_EQUAL_INT(ii, 2, "");

    /* test slist_get_not_empty() and slist_get() APIs */
    for (ii = 0; ii < 6; ii++) {
        node = slist_get_not_empty(&test_list);
        CHECK_EQUAL_INT(((struct data_node *)node)->data, ii, "");
    }
    for (ii = 0; ii < 6; ii++) {
        /* regenerate test_list since we just emptied it */
        slist_append(&test_list, &data_node[ii].node);
    }
    for (ii = 0; ii < 6; ii++) {
        node = slist_get(&test_list);
        CHECK_EQUAL_INT(((struct data_node *)node)->data, ii, "");
    }
    node = slist_get(&test_list);
    CHECK_TRUE(node == NULL, "");

    /* test slist_append_list() */
    slist_init(&append_list);
    struct data_node data_node_append[6] = {
        { .data = 6 },
        { .data = 7 },
        { .data = 8 },
        { .data = 9 },
        { .data = 10 },
        { .data = 11 },
    };
    for (ii = 0; ii < 6; ii++) {
        /* regenerate test_list, which we just emptied */
        slist_append(&test_list, &data_node[ii].node);
        /* Build append_list so that the node pointers are correct */
        slist_append(&append_list, &data_node_append[ii].node);
    }
    slist_append_list(&test_list, &data_node_append[0].node,
            &data_node_append[5].node);
    for (ii = 0; ii < 12; ii++) {
        node = slist_get(&test_list);
        CHECK_EQUAL_INT(((struct data_node *)node)->data, ii, "");
    }

    /* test slist_append_list with emtpy list */
    slist_init(&test_list);
    slist_init(&append_list);
    for (ii = 0; ii < 6; ii++) {
        /* regenerate test_list only */
        slist_append(&test_list, &data_node[ii].node);
    }
    slist_append_list(&test_list, append_list.head, append_list.tail);
    node = slist_peek_tail(&test_list);
    CHECK_EQUAL_INT(((struct data_node *)node)->data, data_node[5].data, "");

    /* test slist_merge_slist */
    slist_init(&test_list);
    slist_init(&append_list);
    for (ii = 0; ii < 6; ii++) {
        /* regenerate both lists */
        slist_append(&test_list, &data_node[ii].node);
        slist_append(&append_list, &data_node_append[ii].node);
    }
    slist_merge_slist(&test_list, &append_list);
    for (ii = 0; ii < 12; ii++) {
        node = slist_get(&test_list);
        CHECK_EQUAL_INT(((struct data_node *)node)->data, ii, "");
    }
    CHECK_TRUE(slist_is_empty(&append_list),
            "merged list is not empty");

    /* test slist_merge_slist with emtpy list */
    slist_init(&test_list);
    slist_init(&append_list);
    for (ii = 0; ii < 6; ii++) {
        /* regenerate test_list only */
        slist_append(&test_list, &data_node[ii].node);
    }

    slist_merge_slist(&test_list, &append_list);
    node = slist_peek_tail(&test_list);
    CHECK_EQUAL_INT(((struct data_node *)node)->data, data_node[5].data, "");

    return 0;
}

/**
 * @}
 */

int main(void) {
    RETURN_IF_NONZERO(test_slist());
    return 0;
}
