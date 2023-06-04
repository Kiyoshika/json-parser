#include "json.h"
#include "json_array.h"
#include <stdio.h>

int main()
{
  int status = -1;

  char* json_string = "{ \"array_obj\": [{\"name\": \"Jimmy\", \"age\": 10}, {\"name\": \"Bob\", \"age\": 20}] }";

  struct json_t* json = json_parse_from_string(json_string);
  if (!json)
  {
    fprintf(stderr, "Failed to parse JSON.\n");
    return -1;
  }

  struct json_array_t* array_obj = json_get(json, "array_obj");

  struct json_t* person1 = json_array_get_fixed(array_obj, 0, JSON_OBJECT);
  char*   person1_name = json_get(person1, "name");
  int32_t person1_age = *(int32_t*)json_get(person1, "age");

  if (strcmp(person1_name, "Jimmy") != 0)
  {
    fprintf(stderr, "Expected first person to have name 'Jimmy' but got '%s'.\n", person1_name);
    goto cleanup;
  }

  if (person1_age != 10)
  {
    fprintf(stderr, "Expected first person to have age 10 but got %d.\n", person1_age);
    goto cleanup;
  }




  struct json_t* person2 = json_array_get_fixed(array_obj, 1, JSON_OBJECT);
  char*   person2_name = json_get(person2, "name");
  int32_t person2_age = *(int32_t*)json_get(person2, "age");

  if (strcmp(person2_name, "Bob") != 0)
  {
    fprintf(stderr, "Expected second person to have name 'Bob' but got '%s'.\n", person2_name);
    goto cleanup;
  }

  if (person2_age != 20)
  {
    fprintf(stderr, "Expected second person to have age 20 but got %d.\n", person2_age);
    goto cleanup;
  }

  status = 0;
cleanup:
  json_free(&json);
  return status;
}
