#include "json.h"
#include "json_array.h"
#include <stdio.h>

int main()
{
  // to_string does not add any spaces or newlines, so we will match the format
  // to make it easier to compare
  // TODO: add array here
  char* json_string = "{\"key\":1,\"key2\":3.141590,\"key3\":true,\"key4\":null,\"key5\":{\"a\":1,\"b\":2},\"key6\":\"hello there\"}";
  struct json_t* json = json_parse_from_string(json_string);
  if (!json)
  {
    fprintf(stderr, "Failed to parse JSON.\n");
    return -1;
  }
  char* to_string = json_to_string(json);
  if (!to_string)
  {
    fprintf(stderr, "Failed to write JSON to string.\n");
    json_free(&json);
    return -1;
  }

  if (strcmp(to_string, json_string) != 0)
  {
    fprintf(stderr, "Output of json_to_string does not match original.\n");
    free(to_string);
    json_free(&json);
    return -1;
  }

  free(to_string);
  json_free(&json);
  return 0;
}
