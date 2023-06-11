#include "json.h"
#include "json_array.h"
#include <stdio.h>
#include <math.h>

int main()
{
  int status = -1;
  char* json_string
    = "{"
      "   \"int\": 0,"
      "   \"decimal\": 0.00,"
      "   \"string\": \"a\","
      "   \"bool\": false,"
      "   \"array\": [],"
      "   \"object\": {}"
      "}";

  struct json_t* json = json_parse_from_string(json_string);

  if (!json)
  {
    fprintf(stderr, "JSON failed to parse.\n");
    return -1;
  }

  json_set_int32(json, "int", 10);
  json_set_decimal(json, "decimal", 3.14159);
  json_set_string(json, "string", "hello there");
  json_set_bool(json, "bool", true);
  
  struct json_array_t* arr = json_array_create();
  int32_t item = 1;
  json_array_append(arr, JSON_INT32, &item);
  item = 2;
  json_array_append(arr, JSON_INT32, &item);
  item = 3;
  json_array_append(arr, JSON_INT32, &item);
  
  json_set_array(json, "array", arr);

  // too lazy to create the object from scratch so just going to parse it
  char* obj_string = "{ \"a\": 1, \"b\": 2 }";
  struct json_t* obj = json_parse_from_string(obj_string);
  
  json_set_object(json, "object", obj);

  int32_t valint = *json_get_int32(json, "int");
  if (valint != 10)
  {
    fprintf(stderr, "Expected int to be 10 but got %d.\n", valint);
    goto cleanup;
  }

  double valdecimal = *json_get_decimal(json, "decimal");
  if (fabs(valdecimal - 3.14159) > 0.0001)
  {
    fprintf(stderr, "Expected decimal to be 3.14159 but got %f.\n", valdecimal);
    goto cleanup;
  }

  char* valstring = json_get_string(json, "string");
  if (strcmp(valstring, "hello there") != 0)
  {
    fprintf(stderr, "Expected string to be 'hello there' but got '%s'.\n", valstring);
    goto cleanup;
  }

  bool valbool = *json_get_bool(json, "bool");
  if (valbool != true)
  {
    fprintf(stderr, "Expected bool to be true but got false.\n");
    goto cleanup;
  }

  // this is tied to the root JSON, so it is NOT freed separately
  struct json_t* valobject = json_get_object(json, "object");
  int32_t a = *json_get_int32(valobject, "a");
  if (a != 1)
  {
    fprintf(stderr, "Expected a to be 1 but got %d.\n", a);
    goto cleanup;
  }

  int32_t b = *json_get_int32(valobject, "b");
  if (b != 2)
  {
    fprintf(stderr, "Expected b to be 2 but got %d.\n", b);
    goto cleanup;
  }

  // this is tied to the root JSON, so it is NOT freed separately
  struct json_array_t* valarray = json_get_array(json, "array");
  int32_t i0 = *(int32_t*)json_array_get(valarray, 0);
  if (i0 != 1)
  {
    fprintf(stderr, "Expected array[0] to be 1 but got %d.\n", i0);
    goto cleanup;
  }

  int32_t i1 = *(int32_t*)json_array_get(valarray, 1);
  if (i1 != 2)
  {
    fprintf(stderr, "Expected array[1] to be 2 but got %d.\n", i1);
    goto cleanup;
  }

  int32_t i2 = *(int32_t*)json_array_get(valarray, 2);
  if (i2 != 3)
  {
    fprintf(stderr, "Expected array[2] to be 3 but got %d.\n", i2);
    goto cleanup;
  }

  status = 0;
cleanup:
  json_free(&json);
  return status;
}
