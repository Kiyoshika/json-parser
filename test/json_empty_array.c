#include "json.h"
#include "json_array.h"
#include <stdio.h>

int main()
{
  char* json_string = "{ \"key\": [] }";

  struct json_t* json = json_parse_from_string(json_string);
  if (!json)
  {
    fprintf(stderr, "JSON failed to parse.\n");
    return -1;
  }

  struct json_array_t* arr = json_get(json, "key");
  if (arr->n_items != 0)
  {
    fprintf(stderr, "Expected JSON array to have 0 items but has %zu.\n", arr->n_items);
    json_free(&json);
    return -1;
  }

  json_free(&json);
  return 0;
}
