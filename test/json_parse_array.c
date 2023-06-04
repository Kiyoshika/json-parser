#include "json.h"
#include <stdio.h>
#include <math.h>

int main()
{
  int status = -1;

  // simple key-value pairs for basic int and string types

  char* json_string = "{ \"array_int\": [1, 2, 3], \"array_double\": [1.1, 2.2, 3.3, 4.4, 5.5] }";

  struct json_t* json = json_parse_from_string(json_string);
  if (!json)
  {
    fprintf(stderr, "JSON failed to parse.\n");
    return -1;
  }

  struct json_array_t* array = json_get(json, "array_int");
  printf("\n");
  for (size_t i = 0; i < array->length; ++i)
    printf("%d ", array->contents.int32[i]);

  struct json_array_t* array_double = json_get(json, "array_double");
  printf("\n");
  for (size_t i = 0; i < array_double->length; ++i)
    printf("%f ", array_double->contents.decimal[i]);

  status = 0;
cleanup:
  json_free(&json);
  return status;
}
