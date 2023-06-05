#include "json.h"
#include <stdio.h>

int main()
{
  char* json_string = "{ \"key\": 1 2 }";

  struct json_t* json = json_parse_from_string(json_string);
  if (json != NULL)
  {
    fprintf(stderr, "(1/2) JSON was supposed to fail but parsed successfully.\n");
    json_free(&json);
    return -1;
  }

  char* json_string_2 = "{ \"key\": 23 . 25 }";
  struct json_t* json_2 = json_parse_from_string(json_string_2);
  if (json_2 != NULL)
  {
    fprintf(stderr, "(2/2) JSON was supposed to fail but parsed successfully.\n");
    json_free(&json_2);
    return -1;
  }

  return 0;
}
