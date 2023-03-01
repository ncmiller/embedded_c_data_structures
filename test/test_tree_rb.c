/*      $OpenBSD: rb-test.c,v 1.4 2008/04/13 00:22:17 djm Exp $ */
/*
 * Copyright 2002 Niels Provos <provos@citi.umich.edu>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $FreeBSD$
 */
#include "test.h"
#define _RB_DIAGNOSTIC
#include "tree.h"
#include <stdlib.h>
#include <time.h>


struct node {
        RB_ENTRY(node) node;
        int key;
};

static RB_HEAD(tree, node) root;

static int
compare(struct node *a, struct node *b)
{
        if (a->key < b->key) return (-1);
        else if (a->key > b->key) return (1);
        return (0);
}

RB_PROTOTYPE(tree, node, node, compare);

RB_GENERATE(tree, node, node, compare);

#define ITER 150

int rb_test(void)
{
        struct node *tmp, *ins, store[ITER];
        int i, j, k, max, min;

        min = ITER;
        max = -1;

        RB_INIT(&root);

        /* Initialize keys */
        for (i = 0; i < ITER; i++)
                store[i].key = i;

        /* Randomly shuffle keys */
        for (i = 0; i < ITER; i++) {
                j = i + (rand() % (ITER - i));
                k = store[j].key;
                store[j].key = store[i].key;
                store[i].key = k;
        }

        for (i = 0; i < ITER; i++) {
                for (j = 0; j < i; ++j) {
                        tmp = &store[j];
                        CHECK_TRUE(tmp == RB_FIND(tree, &root, tmp), "");
                }
                tmp = &store[i];
                if (tmp->key > max)
                        max = tmp->key;
                if (tmp->key < min)
                        min = tmp->key;
                CHECK_TRUE(NULL == RB_INSERT(tree, &root, tmp), "");
                ins = RB_MIN(tree, &root);
                CHECK_TRUE(ins != NULL, "RB_MIN error");
                CHECK_EQUAL_INT(min, ins->key, "");
                ins = RB_MAX(tree, &root);
                CHECK_TRUE(ins != NULL, "RB_MAX error");
                CHECK_EQUAL_INT(max, ins->key, "");
        }
        tmp = RB_ROOT(&root);
        CHECK_TRUE(tree_RB_RANK(tmp) >= 0, "RB rank balance error");
        for (i = 0; i < ITER; i++) {
                tmp = RB_ROOT(&root);
                CHECK_TRUE(tmp != NULL, "RB_ROOT error");
                CHECK_TRUE(tmp == RB_REMOVE(tree, &root, tmp), "");
        }

        return 0;
}

int main(void)
{
        time_t t;
        srand((unsigned)time(&t));
        RETURN_IF_NONZERO(rb_test());
        return 0;
}
