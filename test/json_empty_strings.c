#include "json.h"
#include <stdio.h>

int main()
{
  int status = -1;

  // empty keys should not be allowed
  char* json_string = "{ \"\": \"empty key!\" }";
  struct json_t* json = json_parse_from_string(json_string);
  struct json_t* json_2 = NULL;
  struct json_t* json_3 = NULL;
  struct json_t* json_4 = NULL;
  struct json_t* json_5 = NULL;
  if (json != NULL)
  {
    fprintf(stderr, "(1/2) Expected JSON to fail on empty key but succeeded.\n");
    goto cleanup;
  }

  char* json_string_5 = "{ \"key\": 10, \"\": 20 }";
  json_5 = json_parse_from_string(json_string_5);
  if (json_5 != NULL)
  {
    fprintf(stderr, "(2/2) Expected JSON to fail on empty key but succeeded.\n");
    goto cleanup;
  }

  char* json_string_2 = "{ \"key\": \"\" }";
  json_2 = json_parse_from_string(json_string_2);
  if (!json_2)
  {
    fprintf(stderr, "(1/3) JSON failed to parse empty value string.\n");
    goto cleanup;
  }

  char* get = json_get(json_2, "key");
  if (!get || strcmp(get, "") != 0)
  {
    fprintf(stderr, "(1/3) Expected value from 'key' to be empty but was '%s'.\n", get);
    goto cleanup;
  }

  char* json_string_3 = "{ \"key\": \"\", \"key2\": 10 }";
  json_3 = json_parse_from_string(json_string_3);
  if (!json_3)
  {
    fprintf(stderr, "(2/3) JSON failed to parse empty value string.\n");
  }
  get = json_get(json_3, "key");
  if (!get || strcmp(get, "") != 0)
  {
    fprintf(stderr, "(2/3) Expected value from 'key' to be empty but was '%s'.\n", get);
    goto cleanup;
  }

  char* json_string_4 = "{ \"key\": 10, \"key2\": \"\" }";
  json_4 = json_parse_from_string(json_string_4);
  if (!json_4)
  {
    fprintf(stderr, "(3/3) JSON failed to parse empty value string,\n");
    goto cleanup;
  }
  get = json_get(json_4, "key2");
  if (!get || strcmp(get, "") != 0)
  {
    fprintf(stderr, "(3/3) Expected value from 'key2' to be empty but was '%s'.\n", get);
    goto cleanup;
  }

  

  status = 0;
cleanup:
  json_free(&json);
  json_free(&json_2);
  json_free(&json_3);
  json_free(&json_4);
  json_free(&json_5);
  return status;
}
