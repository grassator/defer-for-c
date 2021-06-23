// TODO:
//  * Support C89/C99
//  * Try thread_local version
//  * Add DEFER_NO_ALLOCA flag
//  * Support GCC/Clang

#ifndef DEFER_H
#define DEFER_H

#define DEFER_PRAGMA_NO_WARNING_SHADOW(...)\
  _Pragma("warning (push)") _Pragma("warning (disable: 4459)")\
  __VA_ARGS__\
 _Pragma("warning (pop)")

#define DEFER_PRAGMA_VOID_SENTINEL( ...)\
  _Pragma("warning (push)") _Pragma("warning (error: 4189)")\
  __VA_ARGS__\
 _Pragma("warning (pop)")

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
static const void *DEFER_PRAGMA_NO_WARNING_SHADOW(_DEFER_ERROR_VOID_FN_) = 0;

#define USE_DEFER()\
   union defer_use_t \
     *DEFER_PRAGMA_NO_WARNING_SHADOW(DEFER_PRAGMA_VOID_SENTINEL(_DEFER_ERROR_VOID_FN_)),\
      DEFER_PRAGMA_NO_WARNING_SHADOW(defer_impl_holder) = {0}

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
#define return while((void)_DEFER_ERROR_VOID_FN_, defer_drain(defer_impl_holder.stack),1) return

#endif