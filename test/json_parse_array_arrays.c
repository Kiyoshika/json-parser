#include "json.h"
#include "json_array.h"
#include <stdio.h>

int main()
{
  int status = -1;

  char* json_string = "{ \"nested_arrays\": [ [1, 2, 3], [\"a\", \"b\", \"c\"] ] }";
  
  struct json_t* json = json_parse_from_string(json_string);
  if (!json)
  {
    fprintf(stderr, "Failed to parse JSON.\n");
    return -1;
  }

  struct json_array_t* nested_arrays = json_get(json, "nested_arrays");

  struct json_array_t* int_array = json_array_get(nested_arrays, 0);
  int32_t expected[3] = { 1, 2, 3 };
  for (size_t i = 0; i < int_array->n_items; ++i)
  {
    int32_t value = *(int32_t*)json_array_get(int_array, i);
    if (value != expected[i])
    {
      fprintf(stderr, "Expected int array at index %zu to be %d but got %d.\n", i, expected[i], value);
      goto cleanup;
    }
  }

  struct json_array_t* string_array = json_array_get(nested_arrays, 1);
  char* expected2[3] = { "a", "b", "c" };
  for (size_t i = 0; i < string_array->n_items; ++i)
  {
    char* value = json_array_get(string_array, i);
    if (strcmp(value, expected2[i]) != 0)
    {
      fprintf(stderr, "Expected string array at index %zu to be '%s' but got '%s'.\n", i, expected2[i], value);
      goto cleanup;
    }
  }


  status = 0;
cleanup:
  json_free(&json);
  return status;
}
