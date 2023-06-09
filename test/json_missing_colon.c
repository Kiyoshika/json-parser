#include "json.h"
#include <stdio.h>

int main()
{
  char* json_string = "{ \"key\" 10 }";
  struct json_t* json = json_parse_from_string(json_string);
  if (json != NULL)
  {
    fprintf(stderr, "Expected JSON to fail parsing but succeeded.\n");
    json_free(&json);
    return -1;
  }

  return 0;
}
