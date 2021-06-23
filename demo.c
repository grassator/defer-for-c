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

int main(void) {
  USE_DEFER();

  test_handle_t test_handle = test_acquire();
  DEFER(test_release, test_handle);

  FILE *f = fopen("demo.c", "wb");
  DEFER(fclose, f);

  return 0;
}
