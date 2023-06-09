#include "json.h"
#include <stdio.h>

int main()
{
  char* json_string = "{ \"key\": 10, \"key\": 20 }";
  struct json_t* json = json_parse_from_string(json_string);
  if (json != NULL)
  {
    fprintf(stderr, "Expected JSON to fail parsing but succeeded.\n");
    return -1;
  }

  json_free(&json);
  return 0;
}
