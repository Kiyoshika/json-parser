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
  // NOTE that value strings have to be heap allocated (unlike keys)

  int32_t value1 = -525;
  char* value2 = strdup("string literal");

  json_add_item(json, JSON_INT32, "key1", &value1);
  json_add_item(json, JSON_STRING, "key2", value2);

  // fetch items back
  int32_t key1_get = *(int32_t*)json_get(json, "key1");
  
  if (key1_get != value1)
  {
    fprintf(stderr, "expected value1 to have value %d but got %d.\n", value1, key1_get);
    goto cleanup;
  }

  char* key2_get = json_get(json, "key2");
  if (strcmp(key2_get, value2) != 0)
  {
    fprintf(stderr, "expected value2 to have value '%s' but got '%s'\n", value2, key2_get);
    goto cleanup;
  }

  // check key that doesn't exist (should be null)
  void* noexist = json_get(json, "dummy");
  if (noexist != NULL)
  {
    fprintf(stderr, "expected noexist to be NULL but is pointing to something...\n");
    goto cleanup;
  }

  status = 0;

cleanup:
  json_free(&json);
exittest:
  return status;
}
