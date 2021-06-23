#include <stdio.h>

#include "defer.h"

typedef const char * test_handle_t;

static test_handle_t test_acquire(const char *name) {
  printf("Handle '%s' (ACQUIRE)\n", name);
  return name;
}

static void test_release(test_handle_t handle) {
  printf("Handle '%s' (RELEASE)\n", handle);
}

void test(void) {
  USE_DEFER();

  /*
   * DEFER calls are done in reverse order (as expected).
   * Running this program will print:
   *   Handle 'one' (ACQUIRE)
   *   Handle 'two' (ACQUIRE)
   *   Handle 'two' (RELEASE)
   *   Handle 'one' (RELEASE)
   */
  {
    test_handle_t test_handle = test_acquire("one");
    DEFER(test_release, test_handle);
  }

  // You can add as many DEFER calls as you want so long as there is enough stack space
  {
    test_handle_t test_handle = test_acquire("two");
    DEFER(test_release, test_handle);
  }

  // explicit `return` is required even for void functions, will error out if missing
  return;
}

void libc(void) {
  USE_DEFER();

  const char *path = __FILE__;
  FILE *f = fopen(path, "rb");

  // You can use DEFER on any function that accepts a pointer
  // `fclose` is a common call that can benefit from DEFER
  if (f) DEFER(fclose, f);

  return;
}

int main(void) {
  test();
  libc();

  return 0;
}
