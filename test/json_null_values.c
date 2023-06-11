#include "json.h"
#include <stdio.h>

int main()
{
  int status = -1;
  
  char* json_string = "{ \"key\": null, \"key2\": 10, \"key3\": null, \"key4\": { \"a\": 1, \"b\": 2 }, \"key5\": [1, 2, 3] }";
  struct json_t* json = json_parse_from_string(json_string);
  if (!json)
  {
    fprintf(stderr, "Failed to parse JSON.\n");
    return -1;
  }

  if (!json_get_isnull(json, "key"))
  {
    fprintf(stderr, "Expected 'key' to be NULL.\n");
    goto cleanup;
  }

  if (json_get_isnull(json, "key2"))
  {
    fprintf(stderr, "'key2' is NULL when it shouldn't be.\n");
    goto cleanup;
  }

  if (!json_get_isnull(json, "key3"))
  {
    fprintf(stderr, "Expected 'key3' to be NULL.\n");
    goto cleanup;
  }

  if (json_get_isnull(json, "key4"))
  {
    fprintf(stderr, "'key4' is NULL when it shouldn't be.\n");
    goto cleanup;
  }

  if (json_get_isnull(json, "key5"))
  {
    fprintf(stderr, "'key5' is NULL when it shouldn't be.\n");
    goto cleanup;
  }

  // set an object null then reset it to a new object (to make sure we can still retrieve
  // items back after nulling them
  if (!json_set_null(json, "key4"))
  {
    fprintf(stderr, "There was a problem NULLing key4.\n");
    goto cleanup;
  }

  char* new_obj = "{ \"x\": 10, \"y\": 20 }";
  struct json_t* new_json = json_parse_from_string(new_obj);
  if (!new_json)
  {
    fprintf(stderr, "Couldn't parse new_json.\n");
    goto cleanup;
  }
  json_set_object(json, "key4", new_json);
  if (json_get_isnull(json, "key4"))
  {
    fprintf(stderr, "Expected key4 to no longer be NULL after set_object.\n");
    goto cleanup;
  }
  int32_t x = *json_get_int32(json_get_object(json, "key4"), "x");
  if (x != 10)
  {
    fprintf(stderr, "Expected 'x' in 'key4' to be 10 but got %d.\n", x);
    goto cleanup;
  }

  int32_t y = *json_get_int32(json_get_object(json, "key4"), "y");
  if (y != 20)
  {
    fprintf(stderr, "Expected 'y' in 'key4' to be 20 but got %d.\n", y);
    goto cleanup;
  }



  status = 0;
cleanup:
  json_free(&json);
  return status;

}
