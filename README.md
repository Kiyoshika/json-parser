# JSON Parser for C
I've been wanting to write a JSON parser for a while just for the hell of it, so here's my attempt.

NOTE: this is still under construction, but pretty usable so far.

Due to being under construction, the API is incomplete and unstable (i.e., may change).

If you notice a bug (that's not alreay mentioned in the issues) please report it! (Or fix it and submit a PR)

## Overview
### Currently supported features:
* Basic objects with the following datatypes:
  * int32, double, bool, string, object, array
* Mixed-type arrays
  * e.g., `[1, 3.14159, "hello"]`
* Nested objects
* Nested arrays
* Parsing from raw string

### Upcoming features:
* Reading/writing to file
* Writing JSON object to string (deserialization)
* `null` type
* (experimental) parsing directly into a struct
* Array iterators
* "safer" parsing from string (like `strncpy` instead of `strcpy`)

### Known issues:
* Empty string values break the parser (e.g., `{ "key": "" }`)
* Escaped quotes and other special characters *may* break keys/values

## Basic Usage
Before going over usage, here are the type maps:
* int32 - `JSON_INT32`
* double - `JSON_DECIMAL`
* bool - `JSON_BOOL`
* null - not supported yet...
* array - `JSON_ARRAY`
* object - `JSON_OBJECT`

### Important Note
All keys in JSON are stack-allocated. This is controlled via `define JSON_MAX_KEY_LEN` in `json.h`. If you expect larger keys, please change this prior to building. The default is currently `50` chars (excluding null term).

### Parsing from Raw String
This is an example of parsing the most basic form of JSON.
```c
#include "json.h"

// ...

char* json_string = "{ \"keyA\": 10, \"keyB\": 20.25, \"keyC\": \"hello\" }";
struct json_t* json = json_parse_from_string(json_string);
if (!json)
{
  // handle failure ...
}

// OLD STYLE CASTING
int32_t keyA = *(int32_t*)json_get(json, "keyA");
double keyB = *(double*)json_get(json, "keyB");
char* keyC = json_get(json, "keyC"); // no need to cast for char* type

// NEW GETTERS (introduced 09 June 2023)
// if you don't want to cast the values, you can directly dereference the function
// (or keep the pointer, but if that's the case you may as well stick to original json_get)
keyA = *json_get_int32(json, "keyA");
keyB = *json_get_double(json, "keyB");
// no need for dereferencing on pointer types (strings, objects, arrays)
keyC = json_get_string(json, "keyC");

json_free(&json);
```

### Parsing arrays
This is an example showing how to deal with arrays/nested arrays
```c
#include "json.h"
#include "json_array.h" // for json_array_... functions

// ...

char* json_string = "{ \"key\": [ [1, 2, 3], [\"a\", \"b\", \"c\"] ] }";
struct json_t* json = json_parse_from_string(json_string);
if (!json)
{
  // handle failure ...
}

// this array contains two arrays that we can fetch
struct json_array_t* array = json_get(json, "key");

// get_fixed means we are accessing an array where each
// element is the same type (i.e., "fixed" type).
// 
// see next example for dealing with mixed-type arrays
struct json_array_t* int_arr = json_array_get_fixed(array, 0, JSON_ARRAY);
struct json_array_t* str_arr = json_array_get_fixed(array, 1, JSON_ARRAY);

int32_t one = *(int32_t*)json_array_get_fixed(int_arr, 0, JSON_INT32);

// strings are stored in arrays differently than in regular JSON objects,
// so we must cast and dereference (since the array returns the address)
char* a = *(char**)json_array_get_fixed(str_arr, 0, JSON_STRING);

json_free(&json);
```

### Arrays with mixed types
This example shows how to deal with mixed-type arrays
```c
#include "json.h"
#include "json_array.h" // for json_array_... functions

// ...

char* json_string = "{ \"key\": [1, 3.14159, \"hello\"] }";
struct json_t* json = json_parse_from_string(json_string);

struct json_array_t* array = json_get(json, "key");

// each type in the array is a different size (4, 8 and 8).
// to handle this, we use the get_mixed function which computes the
// appropriate offset to fetch the correct item.
// this is slower than get_fixed because this loop is ran every time
// we call get()
int32_t value1 = *(int32_t*)json_array_get_mixed(array, 0);
double value2 = *(double*)json_array_get_mixed(array, 1);
char* valu3 = *(char**)json_array_get_mixed(array, 2);

json_free(&json);
```

### Parsing nested objects
This example shows how to fetch items in a nested object.
```c
#include "json.h"

// ...

char* json_string = "{ \"key\": \"rootkey\", \"nested\": { \"nestedA\": 10, \"nestedB\": 20.20, \"nestedC\": \"val\" } }";

struct json_t* json = json_parse_from_string(json_string);
if (!json)
{
  // handle failure ...
}

// PLEASE DO NOT free nested objects!!!!!
// everything is cleaned up in the original json_free(&json);
struct json_t* nested = json_get(json, "nested");

int32_t nestedA = *(int32_t*)json_get(nested, "nestedA");
double nestedB = *(double*)json_get(nested, "nestedB");
char* nestedC = json_get(nested, "nestedC"); // no need to cast char* types

json_free(&json);
```

### Deeply nested object
This is an example of parsing and retrieving a value from a deeply-nested object.

Consider the following JSON (which is too hard to read in a single string):

```json
{
  "browsers": {
    "firefox": {
      "name": "Firefox",
      "pref_url": "about:config",
      "releases": {
        "1": {
          "release_date": "2004-11-09",
          "status": "retired",
          "engine": "Gecko",
          "engine_version": "1.7"
        }
      }
    }
  }
}

```

Let's say we want to retrive the `engine` field:
```c
#include "json.h"

// ...

char* json_string_2 = "{\"browsers\":{\"firefox\":{\"name\":\"Firefox\",\"pref_url\":\"about:config\",\"releases\":{\"1\":{\"release_date\":\"2004-11-09\",\"status\":\"retired\",\"engine\":\"Gecko\",\"engine_version\":\"1.7\"}}}}}";

 struct json_t* json = json_parse_from_string(json_string);
 if (!json)
 {
   // handle failure ...
 }

 // dealing with nested objects can get a little hard to read, but
 // you can "chain" get calls that return objects like so:

 char* engine =  json_get(
                 json_get(
                 json_get(
                 json_get(
                 json_get(json, "browsers"), "firefox"), "releases"), "1"), "engine");

printf("Engine: %s\n", engine);
```

### Adding item to an array
Items can be added to an array by providing the type and pointer
```c
#include "json.h"
#include "json_array.h"

// ...

// assume we get this from somewhere (e.g., parsing a string)
struct json_array_t* array = ...;

// note that since JSON supports multi-type arrays, it doesn't
// matter what the other types are (assuming array is non-empty
int32_t value = 24;
if (!json_array_append(array, JSON_INT32, &item))
{
  // something went wrong (e.g., out of memory, etc)
}
```

### Adding item to an object
Items can be added to an object by providing the key, type and pointer.

Note that duplicate keys are not allowed and will return false.

```c
#include "json.h"

// ...

// assume we get this from somewhere (e.g., parsing a string)
struct json_t* json = ...;

int32_t value = 24;
if (!json_add_item(json, JSON_INT32, "myvalue", &value))
{
  // something went wrong (e.g., out of memory, etc)
}
```
