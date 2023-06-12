#include "json.h"
#include "json_array.h"
#include <stdio.h>
#include <math.h>

int main()
{
  int status = -1;

  // test both the to & from file functionality for arrays
  char* array_string = "[1,3.141590,true,null,\"hello there\",{\"a\":1,\"b\":2},[1,2,3]]";
  struct json_array_t* array = json_parse_array_from_string(array_string);
  struct json_array_t* array_read = NULL;
  char* new_array_string = NULL;
  if (!array)
  {
    fprintf(stderr, "Failed to parse array.\n");
    return -1;
  }

  if (array->n_items != 7)
  {
    fprintf(stderr, "Expected 7 items.\n");
    goto cleanup;
  }

  if (!json_array_to_file(array, "test_json_array.json"))
  {
    fprintf(stderr, "Failed to write array to file.\n");
    goto cleanup;
  }

  array_read = json_parse_array_from_file("test_json_array.json");
  if (!array_read)
  {
    fprintf(stderr, "Failed to read array from file.\n");
    goto cleanup;
  }

  new_array_string = json_array_to_string(array_read);
  if (!new_array_string)
  {
    fprintf(stderr, "Failed to write array_read to string.\n");
    goto cleanup;
  }

  if (strcmp(new_array_string, array_string) != 0)
  {
    fprintf(stderr, "array read from file does not match original.\n");
    goto cleanup;
  }

  status = 0;
cleanup:
  free(new_array_string);
  json_array_free(&array);
  json_array_free(&array_read);
  return status;
}
