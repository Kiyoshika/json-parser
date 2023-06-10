#include "json.h"
#include <stdio.h>

int main()
{
  int status = -1;

  struct json_t* json = json_parse_from_file("sample_file.json");
  if (!json)
  {
    fprintf(stderr, "Failed to parse sample_file.json\n");
    return -1;
  }

  int32_t key = *json_get_int32(json, "key");
  if (key != 10)
  {
    fprintf(stderr, "Expected key to have value 10 but got %d.\n", key);
    goto cleanup;
  }

  char* key2 = json_get_string(json, "key2");
  if (strcmp(key2, "hey") != 0)
  {
    fprintf(stderr, "Expected key2 to have have 'hey' but got '%s'.\n", key2);
    goto cleanup;
  }

  struct json_t* obj = json_get_object(json, "key3");
  if (!obj)
  {
    fprintf(stderr, "Failed to parse key3.\n");
    goto cleanup;
  }

  int32_t a = *json_get_int32(obj, "a");
  if (a != 1)
  {
    fprintf(stderr, "Expected a to have value 1 but got %d.\n", a);
    goto cleanup;
  }

  int32_t b = *json_get_int32(obj, "b");
  if (b != 2)
  {
    fprintf(stderr, "Expected b to have value 2 but got %d.\n", b);
    goto cleanup;
  }

  struct json_array_t* arr = json_get_array(json, "key4");
  int32_t arr0 = *(int32_t*)json_array_get_fixed(arr, 0, JSON_INT32);
  if (arr0 != 1)
  {
    fprintf(stderr, "Expected key4[0] to have value 1 but got %d.\n", arr0);
    goto cleanup;
  }

  int32_t arr1 = *(int32_t*)json_array_get_fixed(arr, 1, JSON_INT32);
  if (arr1 != 2)
  {
    fprintf(stderr, "Expected key4[1] to have value 2 but got %d.\n", arr1);
    goto cleanup;
  }

  int32_t arr2 = *(int32_t*)json_array_get_fixed(arr, 2, JSON_INT32);
  if (arr2 != 3)
  {
    fprintf(stderr, "Expected key4[2] to have value 3 but got %d.\n", arr2);
    goto cleanup;
  }

  bool key5 = *json_get_bool(json, "key5");
  if (key5 != true)
  {
    fprintf(stderr, "Expected key5 to have value true but got false.\n");
    goto cleanup;
  }

  if (!json_get_isnull(json, "key6"))
  {
    fprintf(stderr, "Expected key6 to be NULL but wasn't.\n");
    goto cleanup;
  }

  // try to parse a long complicated file
  struct json_t* complex_json = json_parse_from_file("complex_file.json");
  if (!json)
  {
    fprintf(stderr, "Failed to parse complex_file.json\n");
    goto cleanup;
  }

  status = 0;
cleanup:
  json_free(&json);
  json_free(&complex_json);
  return status;
}
