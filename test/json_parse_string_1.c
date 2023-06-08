#include "json.h"
#include <stdio.h>
#include <math.h>

int main()
{
  int status = -1;

  // simple key-value pairs for basic int and string types

  char* json_string = "{\"a\": 23.32, \"b\": -532, \"c\": \"myvalue\", \"d\": .552}";

  struct json_t* json = json_parse_from_string(json_string);
  if (!json)
  {
    fprintf(stderr, "JSON failed to parse.\n");
    return -1;
  }

  double value1 = *(double*)json_get(json, "a");
  if (fabs(value1 - 23.32) > 0.0001)
  {
    fprintf(stderr, "expected value1 to be 23 but got %f.\n", value1);
    goto cleanup;
  }

  int32_t value2 = *(int32_t*)json_get(json, "b");
  if (value2 != -532)
  {
    fprintf(stderr, "expected value2 to be -532 but got %d.\n", value2);
    goto cleanup;
  }

  char* value3 = json_get(json, "c");
  if (strcmp(value3, "myvalue") != 0)
  {
    fprintf(stderr, "expected value3 to be 'myvalue' but got '%s'.\n", value3);
    goto cleanup;
  }

  double value4 = *(double*)json_get(json, "d");
  if (fabs(value4 - 0.552) > 0.0001)
  {
    fprintf(stderr, "expected value4 to be 0.552 but got %f.\n", value4);
    goto cleanup;
  }

  // I got this dummy sample from a JSON to String Online converter
  char* json_string_2 = "{\"browsers\":{\"firefox\":{\"name\":\"Firefox\",\"pref_url\":\"about:config\",\"releases\":{\"1\":{\"release_date\":\"2004-11-09\",\"status\":\"retired\",\"engine\":\"Gecko\",\"engine_version\":\"1.7\"}}}}}";
  struct json_t* json_2 = json_parse_from_string(json_string_2);
  if (!json_2)
  {
    fprintf(stderr, "Failed to parse second JSON.\n");
    json_free(&json);
    return -1;
  }

  // not going to bother checking all values (even though I should)
  // just gonna check a random deeply nested one
  
  char* engine =  json_get(
                  json_get(
                  json_get(
                  json_get(
                  json_get(json_2, "browsers"), "firefox"), "releases"), "1"), "engine");
  if (strcmp(engine, "Gecko") != 0)
  {
    fprintf(stderr, "Expected engine to have value 'Gecko' but got '%s'.\n", engine);
    goto cleanup;
  }

  status = 0;
cleanup:
  json_free(&json);
  json_free(&json_2);
  return status;
}
