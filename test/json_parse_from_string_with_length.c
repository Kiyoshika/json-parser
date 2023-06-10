#include "json.h"
#include <stdio.h>

int main()
{
  // a "safer" alternative to the regular JSON parse where
  // we specify the MAX number of bytes to read until we hit
  // a null terminator, otherwise one is appended

  char no_term_string[] = "{ \"key\": 10 }";
  struct json_t* json = json_parse_from_string_with_length(no_term_string, 13);
  if (!json)
  {
    fprintf(stderr, "Failed to parse JSON with no null term.\n");
    return -1;
  }
  int32_t value = *json_get_int32(json, "key");
  if (value != 10)
  {
    fprintf(stderr, "Expected value to be 10 but got %d.\n", value);
    json_free(&json);
    return -1;
  }

  json_free(&json);

  char early_term_string[] = "{ \"key\": 20 }\0some random garbage";
  json = json_parse_from_string_with_length(early_term_string, 33);
  if (!json)
  {
    fprintf(stderr, "Failed to parse JSON with early null term.\n");
    return -1;
  }
  value = *json_get_int32(json, "key");
  if (value != 20)
  {
    fprintf(stderr, "Expected value to be 20 but got %d.\n", value);
    json_free(&json);
    return -1;
  }

  json_free(&json);
  return 0;
}
