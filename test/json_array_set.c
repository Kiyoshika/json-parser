#include "json.h"
#include "json_array.h"
#include <stdio.h>
#include <math.h>

int main()
{
  int status = -1;

  char* json_string = "{ \"key\": [1, 2, 3] }";
  struct json_t* json = json_parse_from_string(json_string);
  struct json_t* new_json = NULL;
  if (!json)
  {
    fprintf(stderr, "Failed to parse JSON.\n");
    return -1;
  }

  struct json_array_t* arr = json_get(json, "key");

  // set index 1 to a double
  double value = 3.14159;
  json_array_set(arr, 1, JSON_DECIMAL, &value);

  // set index 2 to an object
  char* new_json_str = "{ \"a\": 1, \"b\": 2 }";
  new_json = json_parse_from_string(new_json_str);
  if (!new_json)
  {
    fprintf(stderr, "Failed to parse new_json.\n");
    goto cleanup;
  }
  json_array_set(arr, 2, JSON_OBJECT, new_json);

  // validate new array

  int32_t arr0 = *(int32_t*)json_array_get(arr, 0);
  if (arr0 != 1)
  {
    fprintf(stderr, "Incorrect value for arr0.\n");
    goto cleanup;
  }

  double arr1 = *(double*)json_array_get(arr, 1);
  if (fabs(arr1 - 3.14159) > 0.0001)
  {
    fprintf(stderr, "Incorrect value for arr1.\n");
    goto cleanup;
  }

  struct json_t* arr2 = json_array_get(arr, 2);
  int32_t a = *(int32_t*)json_get(arr2, "a");
  if (a != 1)
  {
    fprintf(stderr, "Incorrect value for a in arr2.\n");
    goto cleanup;
  }

  int32_t b = *(int32_t*)json_get(arr2, "b");
  if (b != 2)
  {
    fprintf(stderr, "Incorrect value for b in arr2.\n");
    goto cleanup;
  }

  status = 0;
cleanup:
  json_free(&json);
  return status;
}
