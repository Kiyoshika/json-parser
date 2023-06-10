#include "json.h"
#include <stdio.h>
#include <math.h>

int main()
{
  char* json_string = "{ \"key\": \"value\" }";
  struct json_t* json = json_parse_from_string(json_string);
  if (!json)
  {
    fprintf(stderr, "JSON failed to parse.\n");
    return -1;
  }

  int status = -1;

  // setting key to different size data types
  json_set_int32(json, "key", 25);
  if (json->items[0].type != JSON_INT32)
  {
    fprintf(stderr, "Expected type to be JSON_INT32.\n");
    goto cleanup;
  }
  int32_t value = *json_get_int32(json, "key");
  if (value != 25)
  {
    fprintf(stderr, "Expected value to be 25 but got %d.\n", value);
    goto cleanup;
  }

  json_set_decimal(json, "key", 3.14159);
  if (json->items[0].type != JSON_DECIMAL)
  {
    fprintf(stderr, "Expected type to be JSON_DECIMAL.\n");
    goto cleanup;
  }
  double value2 = *json_get_decimal(json, "key");
  if (fabs(value2 - 3.14159) > 0.0001)
  {
    fprintf(stderr, "Expected value to be 3.14159 but got %f.\n", value2);
    goto cleanup;
  }

  json_set_bool(json, "key", true);
  if (json->items[0].type != JSON_BOOL)
  {
    fprintf(stderr, "Expected type to be JSON_BOOL.\n");
    goto cleanup;
  }
  bool value3 = *json_get_bool(json, "key");
  if (value3 != true)
  {
    fprintf(stderr, "Expected value to be true but got false.\n");
    goto cleanup;
  }

  char* value4_str = "{ \"x\": 10, \"y\": 20 }";
  struct json_t* value4 = json_parse_from_string(value4_str);
  if (!value4)
  {
    fprintf(stderr, "Coudln't parse value4.\n");
    goto cleanup;
  }
  json_set_object(json, "key", &value4);
  if (json->items[0].type != JSON_OBJECT)
  {
    fprintf(stderr, "Expected type to be JSON_OBJECT.\n");
    goto cleanup;
  }
  struct json_t* value4_obj = json_get_object(json, "key");
  int32_t x = *json_get_int32(value4_obj, "x");
  if (x != 10)
  {
    fprintf(stderr, "Expected x to be 10 but got %d.\n", x);
    goto cleanup;
  }

  int32_t y = *json_get_int32(value4_obj, "y");
  if (y != 20)
  {
    fprintf(stderr, "Expected y to be 20 but got %d.\n", y);
    goto cleanup;
  }

  status = 0;
cleanup:
  json_free(&json);
  return status;
}
