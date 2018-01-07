#ifndef GRANDFATHER_H
#define GRANDFATHER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static void* find_fit(size_t size);
static void place(void *bp, size_t asize, size_t size);
static size_t extend_by(size_t block_size);
static void* extend_heap(size_t amount);
static void free_block(void* ptr);
static void* coalesce(void* ptr);
static int coalesce_case(void* ptr);
#endif
