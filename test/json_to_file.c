#include "json.h"
#include "json_array.h"
#include <stdio.h>
#include <math.h>

int main()
{
  int status = -1;

  char* json_string = "{ \"key\": 10, \"key2\": 3.141590 }";
  struct json_t* json = json_parse_from_string(json_string);
  struct json_t* json_read = NULL;
  if (!json)
  {
    fprintf(stderr, "Couldn't parse JSON.\n");
    return -1;
  }
  
  if (!json_to_file(json, "test.json"))
  {
    fprintf(stderr, "Couldn't write JSON to file.\n");
    goto cleanup;
  }

  json_read = json_parse_from_file("test.json");
  if (!json_read)
  {
    fprintf(stderr, "Couldn't read JSON from file.\n");
    goto cleanup;
  }

  if (*json_get_int32(json_read, "key") != 10)
  {
    fprintf(stderr, "Incorrect value for key.\n");
    goto cleanup;
  }

  if (fabs(*json_get_decimal(json_read, "key2") - 3.14159) > 0.0001)
  {
    fprintf(stderr, "Incorrect value for key2.\n");
    goto cleanup;
  }

status = 0;
cleanup:
  json_free(&json);
  json_free(&json_read);
  return status;
}
