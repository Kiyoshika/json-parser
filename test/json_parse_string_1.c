#include "json.h"
#include <stdio.h>
#include <math.h>

int main()
{
  int status = -1;

  // simple key-value pairs for basic int and string types

  char* json_string = "{\"a\": 23.32, \"b\": -532, \"c\": \"myvalue\", \"d\": .552}";

  struct json_t* json = json_parse_from_string(json_string);
  if (!json)
  {
    fprintf(stderr, "JSON failed to parse.\n");
    return -1;
  }

  double value1 = *(double*)json_get(json, "a");
  if (fabs(value1 - 23.32) > 0.0001)
  {
    fprintf(stderr, "expected value1 to be 23 but got %f.\n", value1);
    goto cleanup;
  }

  int32_t value2 = *(int32_t*)json_get(json, "b");
  if (value2 != -532)
  {
    fprintf(stderr, "expected value2 to be -532 but got %d.\n", value2);
    goto cleanup;
  }

  char* value3 = json_get(json, "c");
  if (strcmp(value3, "myvalue") != 0)
  {
    fprintf(stderr, "expected value3 to be 'myvalue' but got '%s'.\n", value3);
    goto cleanup;
  }

  double value4 = *(double*)json_get(json, "d");
  if (fabs(value4 - 0.552) > 0.0001)
  {
    fprintf(stderr, "expected value4 to be 0.552 but got %f.\n", value4);
    goto cleanup;
  }

  status = 0;
cleanup:
  json_free(&json);
  return status;
}
