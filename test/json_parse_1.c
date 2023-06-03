#include "json.h"
#include <stdio.h>

int main()
{
  // a simple key-value pair with string value
  char* json_string = "{\"mykey\": 23.}";
  struct json_t* json = json_parse(json_string);
  if (!json)
    printf("JSON IS NULL\n");
  json_free(&json);

  return 0;
}
