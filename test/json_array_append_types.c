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

  /* INSERT NEW ITEMS */

  struct json_array_t* array = json_get(json, "key");
  json_array_append_int32(array, 10);
  json_array_append_decimal(array, 3.14159);
  json_array_append_bool(array, true);
  json_array_append_null(array);
  json_array_append_string(array, strdup("hello there"));

  char* new_json_string = "{ \"a\": 1, \"b\": 2 }";
  struct json_t* new_json = json_parse_from_string(new_json_string);
  if (!new_json)
  {
    fprintf(stderr, "Failed to parse new_json\n");
    goto cleanup;
  }

  json_array_append_object(array, new_json);

  struct json_array_t* new_array = json_array_create();
  if (!new_array)
  {
    fprintf(stderr, "Failed to create new_array.\n");
    goto cleanup;
  }

  json_array_append_int32(new_array, 1);
  json_array_append_int32(new_array, 2);
  json_array_append_int32(new_array, 3);

  json_array_append_array(array, new_array);

  /* VALIDATE NEW ITEMS */

  if (*json_array_get_int32(array, 0) != 10)
  {
    fprintf(stderr, "Incorrect value for array[0].\n");
    goto cleanup;
  }

  if (fabs(*json_array_get_decimal(array, 1) - 3.14159) > 0.0001)
  {
    fprintf(stderr, "Incorrect value for array[1].\n");
    goto cleanup;
  }

  if (*json_array_get_bool(array, 2) != true)
  {
    fprintf(stderr, "Incorrect value for array[2].\n");
    goto cleanup;
  }

  if (!json_array_get_isnull(array, 3))
  {
    fprintf(stderr, "Incorrect value for array[3].\n");
    goto cleanup;
  }

  if (strcmp(json_array_get_string(array, 4), "hello there") != 0)
  {
    fprintf(stderr, "Incorrect value for array[4].\n");
    goto cleanup;
  }

  struct json_t* get_obj = json_array_get_object(array, 5);
  if (*json_get_int32(get_obj, "a") != 1)
  {
    fprintf(stderr, "Incorrect value for a in get_obj.\n");
    goto cleanup;
  }

  if (*json_get_int32(get_obj, "b") != 2)
  {
    fprintf(stderr, "Incorrect value for b in get_obj.\n");
    goto cleanup;
  }

  struct json_array_t* get_array = json_array_get_array(array, 6);
  if (*json_array_get_int32(get_array, 0) != 1)
  {
    fprintf(stderr, "Incorrect value for get_array[0].\n");
    goto cleanup;
  }

  if (*json_array_get_int32(get_array, 1) != 2)
  {
    fprintf(stderr, "Incorrect value for get_array[1].\n");
    goto cleanup;
  }

  if (*json_array_get_int32(get_array, 2) != 3)
  {
    fprintf(stderr, "Incorrect value for gt_array[2].\n");
    goto cleanup;
  }
  
  status = 0;
cleanup:
  json_free(&json);
  return status;
}
