#include "json.h"
#include "json_array.h"
#include <stdio.h>

int main()
{
  int status = -1;
  
  // previously there was a bug where whitespace after a string until a comma would be 
  // captured as the string value
  char* json_string = "{ \"key\": \"value\"     , \"other\":     \"otherval\",   \"arr\": [\"val1\"    ,   \"val2\"    ]}";

  struct json_t* json = json_parse_from_string(json_string);
  if (!json)
  {
    fprintf(stderr, "Couldn't parse JSON.");
    goto cleanup;
  }

  char* value1 = json_get(json, "key");
  if (strcmp(value1, "value") != 0)
  {
    fprintf(stderr, "Expected value 'value' but got '%s'.\n", value1);
    goto cleanup;
  }

  char* value2 = json_get(json, "other");
  if (strcmp(value2, "otherval") != 0)
  {
    fprintf(stderr, "Expected value 'otherval' but got '%s'.\n", value2);
    goto cleanup;
  }

  struct json_array_t* arr = json_get(json, "arr");
  char* value3 = *(char**)json_array_get_fixed(arr, 0, JSON_STRING);
  if (strcmp(value3, "val1") != 0)
  {
    fprintf(stderr, "Expected value 'val1' but got '%s'.\n", value3);
    goto cleanup;
  }

  char* value4 = *(char**)json_array_get_fixed(arr, 1, JSON_STRING);
  if (strcmp(value4, "val2") != 0)
  {
    fprintf(stderr, "Expected value 'val2' but got '%s'.\n", value4);
    goto cleanup;
  }

  status = 0;
cleanup:
  json_free(&json);
  return status;
}
