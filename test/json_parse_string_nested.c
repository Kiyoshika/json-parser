#include "json.h"
#include <stdio.h>

int main()
{
	int status = -1;

  char* json_string = "{ \"key\": \"some interesting value\", \"nested\": { \"nested A\": 10, \"nested B\": 20.20, \"nested C\": \"val\" } }";

  struct json_t* json = json_parse_from_string(json_string);
  if (!json)
  {
    fprintf(stderr, "JSON failed to parse.\n");
    return -1;
  }

  double get_value = *(double*)json_get(json_get(json, "nested"), "nestedB");

  printf("Value: %f\n", get_value);

	status = 0;
cleanup:
	json_free(&json);
	return status;
}
