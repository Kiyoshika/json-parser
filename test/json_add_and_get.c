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

  // first key will be an int32_t
  // second key will be a char*

  int32_t key1 = -525;
  char* key2 = "string literal";

  json_add_item(json, INT32, "key1", &key1);
  json_add_item(json, STRING, "key2", &key2);

  // fetch items back
  int32_t key1_get = *(int32_t*)json_get(json, "key1");
  
  if (key1_get != key1)
  {
    fprintf(stderr, "expected key1 to have value %d but got %d.\n", key1, key1_get);
    goto cleanup;
  }

  char* key2_get = *(char**)json_get(json, "key2");
  if (strcmp(key2_get, key2) != 0)
  {
    fprintf(stderr, "expected key2 to have value '%s' but got '%s'\n", key2, key2_get);
    goto cleanup;
  }

  // check key that doesn't exist (should be null)
  uintptr_t noexist = json_get(json, "dummy");
  if (noexist != 0)
  {
    fprintf(stderr, "expected noexist to be 0 (NULL) but is pointing to something...\n");
    goto cleanup;
  }

  status = 0;

cleanup:
  json_free(&json);
exittest:
  return status;
}
