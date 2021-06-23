#include <stdio.h>

#include "defer.h"

typedef void * test_handle_t;

static test_handle_t
test_acquire(void) {
  puts("Test Handle Acquired");
  return 0;
}

static void
test_release(
  test_handle_t handle
) {
  (void)handle;
  puts("Test Handle Released");
}

void test(void) {
  USE_DEFER();

  test_handle_t test_handle = test_acquire();
  DEFER(test_release, test_handle);

  // explicit `return` is required even for void functions, will error out if missing
  return;
}

#define STRINGIFY(A) #A

int main(void) {
  USE_DEFER();

  test();

  FILE *f = fopen(STRINGIFY(__FILE__), "rb");
  DEFER(fclose, f);

  return 0;
}
