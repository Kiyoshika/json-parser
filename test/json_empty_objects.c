#include "json.h"
#include <stdio.h>

int main()
{
  char* json_string = "{}";
  struct json_t* json = json_parse_from_string(json_string);
  if (!json)
  {
    fprintf(stderr, "Failed to parse JSON.\n");
    return -1;
  }

  json_free(&json);

  char* json_string_2 = "{ \"key\": {} }";
  struct json_t* json_2 = json_parse_from_string(json_string_2);
  if (!json_2)
  {
    fprintf(stderr, "Failed to parse JSON.\n");
    return -1;
  }

  json_free(&json_2);

  char* json_string_3 = "{   }";
  struct json_t* json_3 = json_parse_from_string(json_string_3);
  if (!json_3)
  {
    fprintf(stderr, "Failed to parse JSON.\n");
    return -1;
  }

  json_free(&json_3);

  char* json_string_4 = "{ \"key\": {   }   }";
  struct json_t* json_4 = json_parse_from_string(json_string_4);
  if (!json_4)
  {
    fprintf(stderr, "Failed to parse JSON.\n");
    return -1;
  }

  json_free(&json_4);

  return 0;
}
