/*
 * Copyright (c) 2017 Intel Corporation
 * Copyright (c) 2023 Nick Miller
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "test.h"
#include "dlist.h"
#include <string.h>

static dlist_t test_list;

struct container_node {
	dnode_t node;
	int unused;
};

static struct container_node test_node_1;
static struct container_node test_node_2;
static struct container_node test_node_3;
static struct container_node test_node_4;

static inline bool verify_emptyness(dlist_t *list)
{
	dnode_t *node;
	dnode_t *s_node;
	struct container_node *cnode;
	struct container_node *s_cnode;
	int count;

	if (!dlist_is_empty(list)) {
		return false;
	}

	if (dlist_peek_head(list)) {
		return false;
	}

	if (dlist_peek_tail(list)) {
		return false;
	}

	count = 0;
	DLIST_FOR_EACH_NODE(list, node) {
		count++;
	}

	if (count) {
		return false;
	}

	DLIST_FOR_EACH_NODE_SAFE(list, node, s_node) {
		count++;
	}

	if (count) {
		return false;
	}

	count = 0;
	DLIST_FOR_EACH_CONTAINER(list, cnode, node) {
		count++;
	}

	if (count) {
		return false;
	}

	count = 0;
	DLIST_FOR_EACH_CONTAINER_SAFE(list, cnode, s_cnode, node) {
		count++;
	}

	if (count) {
		return false;
	}

	return true;
}

static inline bool verify_content_amount(dlist_t *list, int amount)
{
	dnode_t *node;
	dnode_t *s_node;
	struct container_node *cnode;
	struct container_node *s_cnode;
	int count;

	if (dlist_is_empty(list)) {
		return false;
	}

	if (!dlist_peek_head(list)) {
		return false;
	}

	if (!dlist_peek_tail(list)) {
		return false;
	}

	count = 0;
	DLIST_FOR_EACH_NODE(list, node) {
		count++;
	}

	if (count != amount) {
		return false;
	}

	count = 0;
	DLIST_FOR_EACH_NODE_SAFE(list, node, s_node) {
		count++;
	}

	if (count != amount) {
		return false;
	}

	count = 0;
	DLIST_FOR_EACH_CONTAINER(list, cnode, node) {
		count++;
	}

	if (count != amount) {
		return false;
	}

	count = 0;
	DLIST_FOR_EACH_CONTAINER_SAFE(list, cnode, s_cnode, node) {
		count++;
	}

	if (count != amount) {
		return false;
	}

	return true;
}

static inline bool verify_tail_head(dlist_t *list,
				    dnode_t *head,
				    dnode_t *tail,
				    bool same)
{
	if (dlist_peek_head(list) != head) {
		return false;
	}

	if (dlist_peek_tail(list) != tail) {
		return false;
	}

	if (same) {
		if (dlist_peek_head(list) != dlist_peek_tail(list)) {
			return false;
		}
	} else {
		if (dlist_peek_head(list) == dlist_peek_tail(list)) {
			return false;
		}
	}

	return true;
}
/**
 * @addtogroup unit_tests
 * @{
 */

/**
 * @brief Verify doubly linked list functionalities
 *
 * @see dlist_append(), dlist_remove(), dlist_prepend(),
 * dlist_remove(), dlist_insert(), dlist_peek_next()
 * DLIST_ITERATE_FROM_NODE()
 */
int test_dlist(void) {
	dlist_init(&test_list);

	CHECK_TRUE((verify_emptyness(&test_list)),
			"test_list should be empty");

	/* Appending node 1 */
	dlist_append(&test_list, &test_node_1.node);
	CHECK_TRUE((verify_content_amount(&test_list, 1)),
		     "test_list has wrong content");

	CHECK_TRUE((verify_tail_head(&test_list, &test_node_1.node,
				       &test_node_1.node, true)),
		     "test_list head/tail are wrong");

	/* Finding and removing node 1 */
	CHECK_TRUE(dnode_is_linked(&test_node_1.node),
		     "node1 is not linked");
	dlist_remove(&test_node_1.node);
	CHECK_TRUE((verify_emptyness(&test_list)),
		     "test_list should be empty");
	CHECK_FALSE(dnode_is_linked(&test_node_1.node),
		      "node1 is still linked");

	/* Prepending node 1 */
	dlist_prepend(&test_list, &test_node_1.node);
	CHECK_TRUE((verify_content_amount(&test_list, 1)),
		     "test_list has wrong content");

	CHECK_TRUE((verify_tail_head(&test_list, &test_node_1.node,
				       &test_node_1.node, true)),
		     "test_list head/tail are wrong");

	/* Removing node 1 */
	dlist_remove(&test_node_1.node);
	CHECK_TRUE((verify_emptyness(&test_list)),
		     "test_list should be empty");

	/* Appending node 1 */
	dlist_append(&test_list, &test_node_1.node);
	/* Prepending node 2 */
	dlist_prepend(&test_list, &test_node_2.node);

	CHECK_TRUE((verify_content_amount(&test_list, 2)),
		     "test_list has wrong content");

	CHECK_TRUE((verify_tail_head(&test_list, &test_node_2.node,
				       &test_node_1.node, false)),
		     "test_list head/tail are wrong");

	/* Appending node 3 */
	dlist_append(&test_list, &test_node_3.node);

	CHECK_TRUE((verify_content_amount(&test_list, 3)),
		     "test_list has wrong content");

	CHECK_TRUE((verify_tail_head(&test_list, &test_node_2.node,
				       &test_node_3.node, false)),
		     "test_list head/tail are wrong");

	CHECK_TRUE((dlist_peek_next(&test_list, &test_node_2.node) ==
		      &test_node_1.node),
		     "test_list node links are wrong");

	/* Inserting node 4 after node 2 */
	dlist_insert(test_node_2.node.next, &test_node_4.node);

	CHECK_TRUE((verify_tail_head(&test_list, &test_node_2.node,
				       &test_node_3.node, false)),
		     "test_list head/tail are wrong");

	CHECK_TRUE((dlist_peek_next(&test_list, &test_node_2.node) ==
		      &test_node_4.node),
		     "test_list node links are wrong");

	/* Finding and removing node 1 */
	dlist_remove(&test_node_1.node);
	CHECK_TRUE((verify_content_amount(&test_list, 3)),
		     "test_list has wrong content");

	CHECK_TRUE((verify_tail_head(&test_list, &test_node_2.node,
				       &test_node_3.node, false)),
		     "test_list head/tail are wrong");

	/* Removing node 3 */
	dlist_remove(&test_node_3.node);
	CHECK_TRUE((verify_content_amount(&test_list, 2)),
		     "test_list has wrong content");

	CHECK_TRUE((verify_tail_head(&test_list, &test_node_2.node,
				       &test_node_4.node, false)),
		     "test_list head/tail are wrong");

	/* Removing node 4 */
	dlist_remove(&test_node_4.node);
	CHECK_TRUE((verify_content_amount(&test_list, 1)),
		     "test_list has wrong content");

	CHECK_TRUE((verify_tail_head(&test_list, &test_node_2.node,
				       &test_node_2.node, true)),
		     "test_list head/tail are wrong");

	/* Removing node 2 */
	dlist_remove(&test_node_2.node);
	CHECK_TRUE((verify_emptyness(&test_list)),
		     "test_list should be empty");

	/* test iterator from a node */
	struct data_node {
		dnode_t node;
		int data;
	} data_node[6] = {
		{ .data = 0 },
		{ .data = 1 },
		{ .data = 2 },
		{ .data = 3 },
		{ .data = 4 },
		{ .data = 5 },
	};
	dnode_t *node = NULL;
	int ii;

	dlist_init(&test_list);

	for (ii = 0; ii < 6; ii++) {
		dlist_append(&test_list, &data_node[ii].node);
	}

	ii = 0;
	DLIST_ITERATE_FROM_NODE(&test_list, node) {
		ii++;
		if (((struct data_node *)node)->data == 2) {
			break;
		}
	}
	CHECK_EQUAL_INT(ii, 3, "");

	ii = 0;
	DLIST_ITERATE_FROM_NODE(&test_list, node) {
		ii++;
		if (((struct data_node *)node)->data == 3) {
			break;
		}
	}
	CHECK_EQUAL_INT(ii, 1, "");

	ii = 0;
	DLIST_ITERATE_FROM_NODE(&test_list, node) {
		ii++;
	}
	CHECK_EQUAL_INT(ii, 2, "");
        return 0;
}

int cond(dnode_t *node, void *data)
{
	return (node == data) ? 1 : 0;
}
/**
 * @brief Verify doubly linked list functionalities
 *
 * @see dlist_is_head(),dlist_is_tail(),
 * dlist_has_multiple_nodes(),dlist_get()
 * dlist_peek_head_not_empty(),dlist_insert_at(),
 * dlist_peek_prev(),
 */
int test_dlist2(void) {
	struct container_node test_node[6];
	struct container_node insert_node;
	struct container_node insert_node2;

	/* Initialize */
	memset(test_node, 0, sizeof(test_node));
	memset(&insert_node, 0, sizeof(insert_node));
	memset(&insert_node2, 0, sizeof(insert_node2));
	dlist_init(&test_list);

	/* Check if the dlist is empty */
	CHECK_TRUE(dlist_get(&test_list) == NULL,
			"Get a empty dilst, NULL will be returned");

	/* Check if a node can append as head if dlist is empty */
	dlist_insert_at(&test_list, &insert_node.node,
				cond, &test_node[2].node);
	CHECK_TRUE(test_list.head == &insert_node.node, "");
	CHECK_TRUE(test_list.tail == &insert_node.node, "");

	/* Re-initialize and insert nodes */
	dlist_init(&test_list);

	for (int i = 0; i < 5; i++) {
		dlist_append(&test_list, &test_node[i].node);
	}

	CHECK_TRUE(dlist_peek_head_not_empty(&test_list) != NULL,
				"dlist appended incorrectly");

	CHECK_TRUE(dlist_is_head(&test_list,
						&test_node[0].node),
				"dlist appended incorrectly");

	CHECK_TRUE(dlist_is_tail(&test_list,
					&test_node[4].node),
				"dlist appended incorrectly");

	CHECK_TRUE(dlist_has_multiple_nodes(&test_list),
				"dlist appended incorrectly");

	CHECK_TRUE(dlist_peek_prev(&test_list,
				&test_node[2].node) == &test_node[1].node,
				"dlist appended incorrectly");

	CHECK_TRUE(dlist_peek_prev(&test_list,
				&test_node[0].node) == NULL,
				"dlist appended incorrectly");

	CHECK_TRUE(dlist_peek_prev(&test_list,
				NULL) == NULL,
				"dlist appended incorrectly");

	CHECK_TRUE(dlist_get(&test_list) ==
				&test_node[0].node,
				"Get a dilst, head will be returned");

	/* Check if a node can insert in front of known nodes */
	dlist_insert_at(&test_list, &insert_node.node,
				cond, &test_node[2].node);
	CHECK_TRUE(dlist_peek_next(&test_list,
				&test_node[1].node) == &insert_node.node, " ");

	/* Check if a node can append if the node is unknown */
	dlist_insert_at(&test_list, &insert_node2.node,
				cond, &test_node[5].node);
	CHECK_TRUE(dlist_peek_next(&test_list,
				&test_node[4].node) == &insert_node2.node, " ");
        return 0;
}

/**
 * @}
 */

int main(void) {
    RETURN_IF_NONZERO(test_dlist());
    RETURN_IF_NONZERO(test_dlist2());
    return 0;
}
