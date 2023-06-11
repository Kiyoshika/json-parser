#include "json.h"
#include "json_array.h"
#include <stdio.h>

int main()
{
  int status = -1;

  char* json_string_1 = "{ \"key\": false, \"other_key\": true }";
  struct json_t* json_1 = json_parse_from_string(json_string_1);
  struct json_t* json_2 = NULL;
  struct json_t* json_3 = NULL;
  struct json_t* json_4 = NULL;

  if (!json_1)
  {
    fprintf(stderr, "(1/4) Failed to parse JSON.\n");
    return -1;
  }

  bool value1 = *(bool*)json_get(json_1, "key");
  if (value1 != false)
  {
    fprintf(stderr, "Expected 'key' to be false but got true.\n");
    goto cleanup;
  }

  bool value2 = *(bool*)json_get(json_1, "other_key");
  if (value2 != true)
  {
    fprintf(stderr, "Expected 'key2' to be true but got false.\n");
    goto cleanup;
  }

  // Python-style booleans prohibited
  char* json_string_2 = "{ \"key\": False }"; 
  json_2 = json_parse_from_string(json_string_2);
  if (json_2 != NULL)
  {
    fprintf(stderr, "Expected json_2 to fail parsing but it succeeded.\n");
    goto cleanup;
  }

  char* json_string_3 = "{ \"key\": True }";
  json_3 = json_parse_from_string(json_string_3);
  if (json_3 != NULL)
  {
    fprintf(stderr, "Expected json_3 to fail parsing but it succeeded.\n");
    goto cleanup;
  }

  // booleans inside arrays
  char* json_string_4 = "{ \"key\": [true, false, true] }";
  json_4 = json_parse_from_string(json_string_4);
  if (!json_4)
  {
    fprintf(stderr, "(4/4) Failed to parse JSON.\n");
    goto cleanup;
  }

  struct json_array_t* bool_arr = json_get(json_4, "key");
  bool arr_val1 = *(bool*)json_array_get(bool_arr, 0);
  if (arr_val1 != true)
  {
    fprintf(stderr, "Expected arr_val1 to be true but got false.\n");
    goto cleanup;
  }

  bool arr_val2 = *(bool*)json_array_get(bool_arr, 1);
  if (arr_val2 != false)
  {
    fprintf(stderr, "Expected arr_val2 to be false but got true.\n");
    goto cleanup;
  }

  bool arr_val3 = *(bool*)json_array_get(bool_arr, 2);
  if (arr_val3 != true)
  {
    fprintf(stderr, "Expected arr_val3 to be true but got false.\n");
    goto cleanup;
  }

  status = 0;
cleanup:
  json_free(&json_1);
  json_free(&json_2);
  json_free(&json_3);
  json_free(&json_4);
  return status;
}
