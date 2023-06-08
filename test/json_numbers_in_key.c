#include "json.h"
#include <stdio.h>

int main()
{
  char* json_string = "{ \"123key123\": 10 }";
  struct json_t* json = json_parse_from_string(json_string);
  if (!json)
  {
    fprintf(stderr, "Failed to parse JSON.\n");
    return -1;
  }

  int32_t value = *(int32_t*)json_get(json, "123key123");
  if (value != 10)
  {
    fprintf(stderr, "Value expected to be 10 but got %d.\n", value);
    json_free(&json);
    return -1;
  }

  json_free(&json);
  return 0;
}
