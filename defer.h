// TODO:
//  * Support C89/C99
//  * Add DEFER_NO_ALLOCA flag

#ifndef DEFER_H
#define DEFER_H

struct defer_stack_t {
  struct defer_stack_t *previous;
  void (*proc)(void *);
  void *payload;
};

enum {
  _DEFER_NOT_INITIALIZED_ = 1,
  _DEFER_INITIALIZED_ = 2,
};

union defer_use_t {
  struct defer_stack_t *stack;
  char sentinel[_DEFER_INITIALIZED_];
};

static inline void
defer_drain(
  const struct defer_stack_t *it
) {
  for (; it; it = it->previous) it->proc(it->payload);
}

#define _DEFER_ERROR_VOID_FN_ ERROR_this_void_function_must_use_an_explicit_return_at_the_end
static union defer_use_t const *_DEFER_ERROR_VOID_FN_ = 0;

#define USE_DEFER_IMPL\
    union defer_use_t *_DEFER_ERROR_VOID_FN_, defer_impl_holder = {0}

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
    _Static_assert(\
      sizeof(defer_impl_holder.sentinel) == _DEFER_INITIALIZED_,\
      "Please add USE_DEFER() call at the very start of the function"\
    );\
    struct defer_stack_t *defer_new_entry = DEFER_ALLOCA(sizeof(*defer_new_entry));\
    defer_new_entry->previous = defer_impl_holder.stack;\
    defer_new_entry->proc = (void (*)(void *))(_PROC_);\
    defer_new_entry->payload = (void *)(_PAYLOAD_);\
    defer_impl_holder.stack = defer_new_entry;\
    if (0) goto _DEFER_ERROR_VOID_FN_;\
  } while(0)


static const union {
  struct defer_stack_t *stack;
  char sentinel[_DEFER_NOT_INITIALIZED_];
} defer_impl_holder = {0};

#define DEFER_CONCAT(A,B) A##B
#define return \
  _DEFER_ERROR_VOID_FN_: while((void)_DEFER_ERROR_VOID_FN_, defer_drain(defer_impl_holder.stack), 1)\
    if (0) goto _DEFER_ERROR_VOID_FN_; else return

#endif