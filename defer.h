// TODO:
//  * Make sure that `return` is forced in `void` functions
//  * Support C89/C99
//  * Try thread_local version
//  * Add DEFER_NO_ALLOCA flag

#ifndef DEFER_H
#define DEFER_H

#define DEFER_NO_WARN(WARNING, ...)\
  _Pragma("warning (push)") _Pragma(WARNING)\
  __VA_ARGS__\
 _Pragma("warning (pop)")

#define DEFER_NO_WARN_SHADOW(...) DEFER_NO_WARN("warning (disable: 4459)", ##__VA_ARGS__)

struct defer_stack_t {
  struct defer_stack_t *previous;
  void (*proc)(void *);
  void *payload;
};

static inline void
defer_drain(
  const struct defer_stack_t *it
) {
  for (; it; it = it->previous) it->proc(it->payload);
}

enum {
  _DEFER_NOT_INITIALIZED_ = 1,
  _DEFER_INITIALIZED_ = 2,
};

#define USE_DEFER()\
  DEFER_NO_WARN_SHADOW(\
    union {\
      struct defer_stack_t *stack;\
      char sentinel[_DEFER_INITIALIZED_];\
    } defer_impl_holder = {0}\
  )

#define DEFER(_PROC_, _PAYLOAD_)\
  do {\
    _Static_assert(\
      sizeof(defer_impl_holder.sentinel) == _DEFER_INITIALIZED_,\
      "Please add USE_DEFER() call at the very start of the function"\
    );\
    struct defer_stack_t *defer_new_entry = _alloca(sizeof(*defer_new_entry));\
    defer_new_entry->previous = defer_impl_holder.stack;\
    defer_new_entry->proc = (_PROC_);\
    defer_new_entry->payload = (_PAYLOAD_);\
    defer_impl_holder.stack = defer_new_entry;\
  } while(0)

static const union {
  struct defer_stack_t *stack;
  char sentinel[_DEFER_NOT_INITIALIZED_]; } defer_impl_holder = {0};

#define DEFER_CONCAT(A,B) A##B
#define return while(defer_drain(defer_impl_holder.stack),1) return

#endif