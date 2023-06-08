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
  struct json_t* json_2 = NULL;
  struct json_t* json_3 = NULL;
  struct json_t* json_4 = NULL;
  struct json_t* json_5 = NULL;
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

  char* json_string_2 = "{ \"key\": 10 }";
  json_2 = json_parse_from_string(json_string_2);
  if (!json_2)
  {
    fprintf(stderr, "Couldn't parse JSON.\n");
    goto cleanup;
  }
  int32_t intval = *(int32_t*)json_get(json_2, "key");
  if (intval != 10)
  {
    fprintf(stderr, "Expected value to be 10 but got %d.\n", intval);
    goto cleanup;
  }

  char* json_string_3 = "{ \"key\":   11   }";
  json_3 = json_parse_from_string(json_string_3);
  if (!json_3)
  {
   fprintf(stderr, "Couldn't parse JSON.\n");
    goto cleanup;
  }
  intval = *(int32_t*)json_get(json_3, "key");
  if (intval != 11)
  {
    fprintf(stderr, "Expected value to be 11 but got %d.\n", intval);
    goto cleanup;
  }

  char* json_string_4 = "{ \"key\": 1 2 }";
  json_4 = json_parse_from_string(json_string_4);
  if (json_4)
  {
    fprintf(stderr, "Expected json_4 to fail but it succeeded.\n");
    goto cleanup;
  }

  char* json_string_5 = "{ \"key\":13}";
  json_5 = json_parse_from_string(json_string_5);
  if (!json_5)
  {
    fprintf(stderr, "Couldn't parse JSON.\n");
    goto cleanup;
  }
  intval = *(int32_t*)json_get(json_5, "key");
  if (intval != 13)
  {
    fprintf(stderr, "Expected value to be 13 but got %d.\n", intval);
    goto cleanup;
  }

  status = 0;
cleanup:
  json_free(&json);
  json_free(&json_2);
  json_free(&json_3);
  json_free(&json_4);
  json_free(&json_5);
  return status;
}
