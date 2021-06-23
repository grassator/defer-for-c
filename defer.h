/* Copyright (c) 2021 Dmitriy Kubyshkin
 * SPDX-License-Identifier: MIT */

#ifndef DEFER_H
#define DEFER_H

#define _DEFER_ERROR_MISSING_USE_DEFER_\
  USE_DEFER_must_appear_at_the_start_of_the_functions_using_DEFER
#define _DEFER_ERROR_VOID_FN_\
  ERROR_void_functions_must_use_an_explicit_return_at_the_end

#if defined(_MSC_VER)
  #define _DEFER_MSVC_NO_SHADOW_WARNING_(X)\
    _Pragma("warning (push)")\
    _Pragma("warning (disable: 4459)")\
    X\
   _Pragma("warning (pop)")
#else
  #define _DEFER_MSVC_NO_SHADOW_WARNING_(X) X
#endif

typedef void(*defer_callback_t)(void *payload);

#if defined(DEFER_NO_ALLOCA)

  #include <assert.h>

  struct defer_stack_item_t {
    defer_callback_t proc;
    void *payload;
  };

  static struct {
    struct defer_stack_item_t *items;
    struct defer_stack_item_t *next;
  } defer_stack = {0};

  static void
  defer_drain(
    const struct defer_stack_item_t *it,
    const struct defer_stack_item_t *end
  ) {
    for (; it != end; it++) it->proc(it->payload);
  }

  #define _DEFER_COUNTOF_(A)\
    ((sizeof(A)) / (sizeof((A)[0])))

  #define USE_DEFER(_SIZE_)\
    _DEFER_ERROR_MISSING_USE_DEFER_: if (0) goto _DEFER_ERROR_MISSING_USE_DEFER_;\
    struct {\
      struct defer_stack_item_t *next;\
      struct defer_stack_item_t items[_SIZE_];\
    } _DEFER_MSVC_NO_SHADOW_WARNING_(defer_stack)\
      = {&defer_stack.items[0]}

  #define DEFER(_PROC_, _PAYLOAD_)\
    do {\
      if (0) goto _DEFER_ERROR_MISSING_USE_DEFER_;\
      if (0) goto _DEFER_ERROR_VOID_FN_;\
      if (defer_stack.next >= &defer_stack.items[_DEFER_COUNTOF_(defer_stack.items)]) {\
        assert(!"Not enough stack space allocated with USE_DEFER() for all DEFER calls");\
      }\
      struct defer_stack_item_t *defer_new_entry = defer_stack.next++;\
      defer_new_entry->proc = (void (*)(void *))(_PROC_);\
      defer_new_entry->payload = (void *)(_PAYLOAD_);\
    } while(0)

  #define return \
    _DEFER_ERROR_VOID_FN_: while(defer_drain(defer_stack.items, defer_stack.next), 1)\
      if (0) {goto _DEFER_ERROR_VOID_FN_; } else return
#else
  struct defer_stack_t {
    struct defer_stack_t *previous;
    defer_callback_t proc;
    void *payload;
  };
  static struct defer_stack_t *defer_stack = {0};

  static void
  defer_drain(
    const struct defer_stack_t *it
  ) {
    for (; it; it = it->previous) it->proc(it->payload);
  }

  #define USE_DEFER_IMPL\

  #if defined(_MSC_VER)
    #define DEFER_ALLOCA _alloca
  #else
    #include <alloca.h>
    #define DEFER_ALLOCA alloca
  #endif

  #define USE_DEFER(...)\
    _DEFER_ERROR_MISSING_USE_DEFER_: if (0) goto _DEFER_ERROR_MISSING_USE_DEFER_;\
    struct defer_stack_t *_DEFER_MSVC_NO_SHADOW_WARNING_(defer_stack) = {0}

  #define DEFER(_PROC_, _PAYLOAD_)\
    do {\
      if (0) goto _DEFER_ERROR_MISSING_USE_DEFER_;\
      if (0) goto _DEFER_ERROR_VOID_FN_;\
      struct defer_stack_t *defer_new_entry = DEFER_ALLOCA(sizeof(*defer_new_entry));\
      defer_new_entry->previous = defer_stack;\
      defer_new_entry->proc = (void (*)(void *))(_PROC_);\
      defer_new_entry->payload = (void *)(_PAYLOAD_);\
      defer_stack = defer_new_entry;\
    } while(0)

  #define return \
    _DEFER_ERROR_VOID_FN_: while(defer_drain(defer_stack), 1)\
      if (0) {goto _DEFER_ERROR_VOID_FN_; } else return
#endif

#endif /* DEFER_H */
