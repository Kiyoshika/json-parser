#include "json.h"
#include <stdio.h>
#include <math.h>

int main()
{
	int status = -1;

  char* json_string = "{ \"key\": \"rootkey\", \"nested\": { \"nestedA\": 10, \"nestedB\": 20.20, \"nestedC\": \"val\" } }";

  struct json_t* json = json_parse_from_string(json_string);
  if (!json)
  {
    fprintf(stderr, "JSON failed to parse.\n");
    return -1;
  }

  char* value1 = json_get(json, "key");
  if (strcmp(value1, "rootkey") != 0)
  {
    fprintf(stderr, "Expected 'key' to have valule 'rootkey' but got '%s'.\n", value1);
    goto cleanup;
  }

  struct json_t* nested_json = json_get(json, "nested");

  int32_t value2 = *(int32_t*)json_get(nested_json, "nestedA");
  if (value2 != 10)
  {
    fprintf(stderr, "Expected 'nestedA' to have value 10 but got %d.\n", value2);
    goto cleanup;
  }

  double value3 = *(double*)json_get(nested_json, "nestedB");
  if (fabs(value3 - 20.20) > 0.0001)
  {
    fprintf(stderr, "Expected 'nestedB' to have value 20.20 but got %f.\n", value3);
    goto cleanup;
  }

  char* value4 = json_get(nested_json, "nestedC");
  if (strcmp(value4, "val") != 0)
  {
    fprintf(stderr, "Expected 'nestedC' to have value 'val' but got '%s'.\n", value4);
    goto cleanup;
  }

	status = 0;
cleanup:
	json_free(&json);
	return status;
}
