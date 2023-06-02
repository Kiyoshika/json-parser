#include "json.h"
#include <stdio.h>

int main()
{
  int status = -1;

  struct json_t* json = json_create();

  if (!json)
  {
    fprintf(stderr, "test couldn't finish due to allocation failure.\n");
    goto exittest;
  }

  if (json->n_items != 0)
  {
    fprintf(stderr, "n_items expected to be 0 but is %zu.\n", json->n_items);
    goto cleanup;
  }

  if (json->capacity != 10)
  {
    fprintf(stderr, "capacity expected to be 10 but is %zu.\n", json->capacity);
    goto cleanup;
  }

  status = 0;

cleanup:
  json_free(&json);
exittest:
  return status;
}
