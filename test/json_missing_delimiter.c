#include "json.h"
#include <stdio.h>

int main()
{
  char* json_string = "{ \"key1\": \"a\" \"key2\": \"b\" }";
  struct json_t* json = json_parse_from_string(json_string);

  if (json != NULL)
  {
    fprintf(stderr, "JSON was expected to fail parsing but succeeded.\n");
    json_free(&json);
    return -1;
  }

  char* json_string_2 = "{ \"key\": [\"a\" \"b\"] }";
  struct json_t* json_2 = json_parse_from_string(json_string_2);

  if (json_2 != NULL)
  {
    fprintf(stderr, "JSON was expected to fail parsing but succeeded.\n");
    json_free(&json_2);
    return -1;
  }

  char* json_string_3 = "{ \"key\": [1 2] }";
  struct json_t* json_3 = json_parse_from_string(json_string_3);

  if (json_3 != NULL)
  {
    fprintf(stderr, "JSON was expected to fail parsing but succeeded.\n");
    json_free(&json_3);
    return -1;
  }

  char* json_string_4 = "{ \"key\": [1.1 2.2] }";
  struct json_t* json_4 = json_parse_from_string(json_string_4);

  if (json_4 != NULL)
  {
    fprintf(stderr, "JSON was expected to fail parsing but succeeded.\n");
    json_free(&json_4);
    return -1;
  }

  char* json_string_5 = "{ \"key\": [[1] [2]] }";
  struct json_t* json_5 = json_parse_from_string(json_string_5);

  if (json_5 != NULL)
  {
    fprintf(stderr, "JSON was expected to fail parsing but succeeded.\n");
    json_free(&json_5);
    return -1;
  }

  char* json_string_6 = "{ \"key\": [{\"a\": 1} {\"b\": 2}] }";
  struct json_t* json_6 = json_parse_from_string(json_string_6);

  if (json_6 != NULL)
  {
    fprintf(stderr, "JSON was expected to fail parsing but succeeded.\n");
    json_free(&json_6);
    return -1;
  }

  char* json_string_7 = "{ \"key\": 1 \"key2\": 2 }";
  struct json_t* json_7 = json_parse_from_string(json_string_7);
  if (json_7 != NULL)
  {
    fprintf(stderr, "JSON was expected to fail parsing but succeeded.\n");
    json_free(&json_7);
    return -1;
  }

  return 0;
}
