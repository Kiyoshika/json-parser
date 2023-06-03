#include "json.h"
#include <stdio.h>

// testing keys & values with "special" characters and spaces

int main()
{
  int status = -1;

  char* json_string = "{ \"some, interesting, key!@#$\": \"some, interesting, value!@#$\"}";

  struct json_t* json = json_parse_from_string(json_string);
  if (!json)
  {
    fprintf(stderr, "JSON failed to parse.\n");
    return -1;
  }

  char* value = json_get(json, "some, interesting, key!@#$");
  if (!value)
  {
    fprintf(stderr, "Couldn't find key.\n");
    goto cleanup;
  }

  if (strcmp(value, "some, interesting, value!@#$") != 0)
  {
    fprintf(stderr, "Value is incorrect.\n");
    goto cleanup;
  }

  status = 0;
cleanup:
  json_free(&json);
  return status;
}
