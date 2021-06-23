/* TODO:
 * Add DEFER_NO_ALLOCA flag
 */

#ifndef DEFER_H
#define DEFER_H

struct defer_stack_t {
  struct defer_stack_t *previous;
  void (*proc)(void *);
  void *payload;
};
static struct defer_stack_t *defer_stack = {0};

#define _DEFER_ERROR_MISSING_USE_DEFER_\
  USE_DEFER_must_appear_at_the_start_of_the_functions_using_DEFER
#define _DEFER_ERROR_VOID_FN_\
  ERROR_void_functions_must_use_an_explicit_return_at_the_end

static void
defer_drain(
  const struct defer_stack_t *it
) {
  for (; it; it = it->previous) it->proc(it->payload);
}

#define USE_DEFER_IMPL\
  _DEFER_ERROR_MISSING_USE_DEFER_: {}\
  static struct defer_stack_t *defer_stack = {0}

#if defined(_MSC_VER)
  #define DEFER_ALLOCA _alloca

  #define USE_DEFER()\
    _Pragma("warning (push)")\
    _Pragma("warning (disable: 4459)")\
    USE_DEFER_IMPL\
   _Pragma("warning (pop)")

#else
  #include <alloca.h>
  #define DEFER_ALLOCA alloca

  #define DEFER_PRAGMA(X) _Pragma(#X)
  #define USE_DEFER() USE_DEFER_IMPL
#endif

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