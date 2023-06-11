#include "json.h"
#include "json_array.h"
#include <stdio.h>
#include <math.h>

int main()
{
  int status = -1;

  char* json_string = "{ \"key\": [] }";
  struct json_t* json = json_parse_from_string(json_string);
  if (!json)
  {
    fprintf(stderr, "Failed to parse JSON.\n");
    return -1;
  }

  struct json_array_t* array = json_get(json, "key");
  json_array_append_int32(array, 10);
  json_array_append_decimal(array, 3.14159);
  json_array_append_bool(array, true);
  json_array_append_null(array);
  // TODO: finish
  
  status = 0;
cleanup:
  json_free(&json);
  return status;
}
