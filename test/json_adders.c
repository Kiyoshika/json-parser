#include "json.h"
#include <stdio.h>
#include <math.h>

int main()
{
  int status = -1;

  // use new adder functions for each data type (including NULLs)
  struct json_t* json = json_create();
  if (!json)
  {
    fprintf(stderr, "Failed to create JSON.\n");
    return 0;
  }

  if (!json_add_int32(json, "key1", 10))
  {
    fprintf(stderr, "Failed to add key1.\n");
    goto cleanup;
  }

  if (*json_get_int32(json, "key1") != 10)
  {
    fprintf(stderr, "Incorrect value for key1.\n");
    goto cleanup;
  }

  if (!json_add_decimal(json, "key2", 3.14159))
  {
    fprintf(stderr, "Failed to add key2.\n");
    goto cleanup;
  }

  if (fabs(*json_get_decimal(json, "key2") - 3.14159) > 0.0001)
  {
    fprintf(stderr, "Incorrect value for key2.\n");
    goto cleanup;
  }

  if (!json_add_string(json, "key3", "hello there"))
  {
    fprintf(stderr, "Failed to add key3.\n");
    goto cleanup;
  }

  if (strcmp(json_get_string(json, "key3"), "hello there") != 0)
  {
    fprintf(stderr, "Incorrect value for key3.\n");
    goto cleanup;
  }

  char* json_string = "{ \"a\": 1, \"b\": 2 }";
  struct json_t* new_obj = json_parse_from_string(json_string);
  if (!new_obj)
  {
    fprintf(stderr, "Failed to parse new_obj.\n");
    goto cleanup;
  }

  if (!json_add_object(json, "key4", new_obj))
  {
    fprintf(stderr, "Failed to add key4.\n");
    goto cleanup;
  }

  struct json_t* json_get = json_get_object(json, "key4");
  if (*json_get_int32(json_get, "a") != 1)
  {
    fprintf(stderr, "Incorrect value for 'a'.\n");
    goto cleanup;
  }

  if (*json_get_int32(json_get, "b") != 2)
  {
    fprintf(stderr, "Incorrect value for 'b'.\n");
    goto cleanup;
  }

  struct json_array_t* arr = json_array_create();
  if (!arr)
  {
    fprintf(stderr, "Failed to create arr.\n");
    goto cleanup;
  }
  int32_t item = 1;
  json_array_append(arr, JSON_INT32, &item);
  item = 2;
  json_array_append(arr, JSON_INT32, &item);
  item = 3;
  json_array_append(arr, JSON_INT32, &item);

  if (!json_add_array(json, "key5", arr))
  {
    fprintf(stderr, "Failed to add key5.\n");
    goto cleanup;
  }

  struct json_array_t* arr_get = json_get_array(json, "key5");
  int32_t arr_val = *(int32_t*)json_array_get_fixed(arr_get, 0, JSON_INT32);
  if (arr_val != 1)
  {
    fprintf(stderr, "Incorrect value for key5[0].\n");
    goto cleanup;
  }

  arr_val = *(int32_t*)json_array_get_fixed(arr_get, 1, JSON_INT32);
  if (arr_val != 2)
  {
    fprintf(stderr, "Incorrect value for key5[1].\n");
    goto cleanup;
  }

  arr_val = *(int32_t*)json_array_get_fixed(arr_get, 2, JSON_INT32);
  if (arr_val != 3)
  {
    fprintf(stderr, "Incorrect value for key5[2].\n");
    goto cleanup;
  }

  if (!json_add_bool(json, "key6", true))
  {
    fprintf(stderr, "Failed to add key6.\n");
    goto cleanup;
  }

  if (*json_get_bool(json, "key6") != true)
  {
    fprintf(stderr, "Incorrect value for key6.\n");
    goto cleanup;
  }

  if (!json_add_null(json, "key7"))
  {
    fprintf(stderr, "Failed to add key7.\n");
    goto cleanup;
  }

  if (!json_get_isnull(json, "key7"))
  {
    fprintf(stderr, "Incorrect value for key7.\n");
    goto cleanup;
  }

  status = 0;
cleanup:
  json_free(&json);
  return status;
}
