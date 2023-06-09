# JSON Parser for C
I've been wanting to write a JSON parser for a while just for the hell of it, so here's my attempt.

If you notice a bug (that's not alreay mentioned in the issues) please report it! (Or fix it and submit a PR)

## Overview
### Currently supported features:
* Basic objects with the following datatypes:
  * int32, double, bool, string, object, array, null
* Mixed-type arrays
  * e.g., `[1, 3.14159, "hello"]`
* Nested objects
* Nested arrays
* Parsing from raw string
* Parsing from a file
* Modifying objects and arrays
* Writing JSON object and array to string or file

### Upcoming features:
* (experimental) parsing directly into a struct

### Known issues:
* Escaped quotes and other special characters *may* break keys/values

## Building and Including in Other Projects
There are two main ways to add this to your project:
1. Build separately and use `find_package`
2. Use a git submodule and add it to your build

Both methods work and are up to your preference, but here I'll quickly guide through both ways.

### Building Separately
If you wish to build separately, first clone the repo and enter the root directory. Then use:
* `mkdir build && cd build`
* `cmake -DDEBUG_MODE=OFF -DCOMPILE_TESTS=OFF ..` - of course, you can change these flags if you want to run tests (`make test`) or hack at the library
* `sudo make install` - this will build the library and add the static lib, headers and config to the global install directory

Now, in your new project's CMakeLists.txt you can use

```cmake
find_package(json 0.1.0 REQUIRED EXACT) # or whatever version you're targeting

add_executable(myexe myexe.c)
target_link_libraries(myexe json)
```

Now you should be able to use the following headers and build your project as normal.

```c
#include <json/json.h>
#include <json/json_array.h>

// ...
```

### Including into Your Build
If you'd rather include the library as part of your build, create a new folder such as `ext/` in your project's directory.

Now you can add this project as a submodule, `git submodule add [repo link]` (NOTE: it's recommended to use one of the version branches instead of master as master may be less stable)

Then in your project's CMakeLists.txt, you can add something like:

```cmake
# build libjson first
add_subdirectory(ext/json-parser)


add_executable(myexe myexe.c)
target_include_directories(myexe PUBLIC ${json_SOURCE_DIR}/include)
target_link_libraries(myexe json)
```

Now you should be able to use the headers:

```c
#include "json.h"
#include "json_array.h"

// ...
```

# Examples
Note that the examples below don't cover everything in the API; it's encouraged to peek at the [headers](include/) for a more complete API reference.

Before going over usage examples, here are the type maps:
* int32 - `JSON_INT32`
* double - `JSON_DECIMAL`
* bool - `JSON_BOOL`
* null - `JSON_NULL`
* array - `JSON_ARRAY`
* object - `JSON_OBJECT`

### Important Note
All keys in JSON are stack-allocated. This is controlled via `define JSON_MAX_KEY_LEN` in `json.h`. If you expect larger keys, please change this prior to building. The default is currently `50` chars (excluding null term).

## Contents:
* Memory:
  * [Heap vs Stack Items](#heap-vs-stack-items)
* IO:
  * [Parsing from Raw String](#parsing-from-raw-string)
  * [Parsing from File](#parsing-from-file)
  * [Writing to String](#writing-to-string)
  * [Writing to File](#writing-to-file)
* Usage:
  * [Updating Objects](#updating-objects)
  * [Handling Null Values](#handling-null-values)
* Arrays:
  * [Parsing Arrays](#parsing-arrays)
  * [Adding Item to an Array](#adding-item-to-an-array)
  * [Arrays with Mixed Types](#arrays-with-mixed-types)
* Objects:
  * [Parsing Nested Objects](#parsing-nested-objects)
  * [Deeply Nested Objects](#deeply-nested-objects)
  * [Adding Item to an Object](#adding-item-to-an-object)

## Memory
### Heap vs Stack Items
The JSON object requires certain types to be stack-allocated and others heap-allocated.

Below is the list of mandatory **stack-allocated** items:
* `JSON_INT32`
* `JSON_DECIMAL`
* `JSON_BOOL`
* `JSON_NULL`

Below is the list of mandatory **heap-allocated** items:
* `JSON_STRING`
* `JSON_OBJECT`
* `JSON_ARRAY`

Whenever you use any of the setter/append functions within the library, if it's a mandatory heap item, you must pass a pointer to the heap-allocated object (examples below show this). If it's a mandatory stack item, you can either pass a literal, e.g., `25` or a variable holding the value, e.g., `myvar`.

Any heap-allocated item you pass to JSON (via setter or append), the ownership is transferred and you must NOT free this pointer yourself. Everything will be properly cleaned up in `json_free()`.

## IO
### Parsing from Raw String
This is an example of parsing the most basic form of JSON.

There are two primary ways to read JSON objects or arrays from strings:
* `json_parse_from_string`
* `json_parse_array_from_string`
  * ONLY use this if you guaranteed to have a null terminator, otherwise this is unsafe
* `json_parse_from_string_with_length`
* `json_parse_array_from_string_with_length`
  * A safer alternative that reads at most some specified number of bytes and appends a null terminator if one wasn't found

Both methods will be shown below

```c
#include "json.h"

// ...

char* json_string = "{ \"keyA\": 10, \"keyB\": 20.25, \"keyC\": \"hello\" }";
// ** ONLY use this version if you can guarantee a null terminator **
struct json_t* json = json_parse_from_string(json_string);
// safer alternative for strings in the wild (in this case it will only read up
// to the 46th bit which is the null terminator)
struct json_t* json_safe = json_parse_from_string_with_length(json_string, 100);
if (!json)
{
  // handle failure ...
}

int32_t keyA = *json_get_int32(json, "keyA");
double keyB = *json_get_double(json, "keyB");
char* keyC = json_get_string(json, "keyC");

json_free(&json);
```

See [json getters](include/json_getters.h) for all getter functions.

### Parsing from File
Provide a file path containing JSON data (note that the extension does not have to be `.json` but used here for clarity).

Under the hood this calls `json_parse_from_string_with_length` where the length is the size of the file in bytes.

```c
#include "json.h"

// ...

struct json_t* json = json_parse_from_file("myjson.json");
if (!json)
{
  // handle error ...
}

// you can also read arrays from a file
// e.g., [{...}, {...}, {...}, ...]
struct json_array_t* array = json_parse_array_from_file("myjson_array.json");
if (!array)
{
  // handle error ...
}
```

### Writing to String
This library supports writing both an object or an array to a string.
```c
#include "json.h"
#include "json_array.h"

// ...

// assume we get these from somewhere/create them...
struct json_t* json = ...;
char* json_string = json_to_string(json);
if (!json_string)
{
  // handle error ...
}

struct json_array_t* array = ...;
char* array_string = json_array_to_string(array);
if (!array_string)
{
  // handle error ...
}
```

### Writing to File
Like writing to string, we can also write to a file.

This really just calls the `...to_string` function under the hood and handles the file business for you.

```c
#include "json.h"
#include "json_array.h"

// ...

// assume we get these from somewhere/create them...
struct json_t* json = ...;
if (!json_to_file(json, "test.json"))
{
  // handle error ...
}

struct json_array_t* array = ...;
if (!json_array_to_file(array, "test_array.json"))
{
  // handle error ...
}
```

## Usage
### Updating Objects
You can update objects by using any of the setters. The original data types do not need to match.

NOTE: any heap-allocated objects will be free()'d prior to overwriting. 

**IMPORTANT:**: DO NOT free anything you obtain from `json_get_...()` as the cleanup will happen in `json_free`.

```c
  #include "json.h"

  // ...

  char* json_string = "{ \"key\": \"value\" }";
  struct json_t* json = json_parse_from_string(json_string);

  printf("%s\n", json_get_string(json, "key"));

  // replace string with int32
  // NOTE: "value" gets deallocated
  json_set_int32(json, "key", 25);
  printf("%d\n", *json_get_int32(json, "key"));

  // replace int32 with object
  char* new_obj_str = "{ \"x\": 10, \"y\": 20 }";
  struct json_t* new_obj = json_parse_from_string(new_obj_str);
  json_set_object(json, "key", new_obj);

  struct json_t* get = json_get_object(json);
  printf("%d\n", *json_get_int32(get, "x"));
  printf("%d\n", *json_get_int32(get, "y"));

  // replace object with string
  // the previous object gets deallocated before changing pointer to string.
  // assuming this strdup doesn't fail; can do it separately to be more safe
  json_set_string(json, "key", strdup("hello there"));

  char* str = json_get_string(json, "key");
  printf("%s\n", str);

  json_free(&json);
  
```

### Handling Null Values
You can check or set null values using appropriate getters/setters.
```c
#include "json.h"

// ...

char* json_string = "{ \"key\": 10 }";
struct json_t* json = json_parse_from_string(json_string);

json_get_isnull(json, "key"); // false

json_set_null(json, "key");
json_get_isnull(json, "key"); // true

// the object now looks like: { "key": null }

json_free(&json);
```

NOTE: If you use `json_set_null()` on a heap-allocated item, it will be free()'d.
```c
#include "json.h"

// ...

char* json_string = "{ \"key\": \"my string\" }";
struct json_t* json = json_parse_from_string(json_string);

// this will deallocate "my string"
json_set_null(json, "key");

json_free(&json);
```

## Arrays
### Parsing Arrays
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

// the type at an array index is known internally and
// assumes the caller knows the type ahead of time
struct json_array_t* int_arr = json_array_get(array, 0);
struct json_array_t* str_arr = json_array_get(array, 1);

int32_t one = json_array_get(int_arr, 0);

char* a = json_array_get(str_arr, 0);

// you can also use setters to modify arrays.
// since JSON supports mixed types, the new type
// does not need to match the original
json_array_set(int_array, 1, 3.14159);
json_array_set(int_array, 2, strdup("hey"));

// note that you can fetch the number of items with the n_items property.
// this can be used for loops
size_t array_len    = array->n_items;
size_t int_arr_len  = int_arr->n_items;
size_t str_arr_len  = str_arr->n_items;


json_free(&json);
```

### Arrays with Mixed Types
This example shows how to deal with mixed-type arrays
```c
#include "json.h"
#include "json_array.h" // for json_array_... functions

// ...

char* json_string = "{ \"key\": [1, 3.14159, \"hello\"] }";
struct json_t* json = json_parse_from_string(json_string);
if (!json)
{
  // handle failure ...
}

struct json_array_t* array = json_get(json, "key");

// the type is known internally, so it assumes the caller
// knows the type ahead of time
int32_t value1  = *json_array_get(array, 0);
double value2   = *json_array_get(array, 1);
char* valu3     = json_array_get(array, 2);

json_free(&json);
```

### Adding Item to an Array
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

## Objects
### Parsing Nested Objects
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

int32_t nestedA   = *json_get_int32(nested, "nestedA");
double nestedB    = *json_get_decimal(nested, "nestedB");
char* nestedC     = json_get(nested, "nestedC"); 

json_free(&json);
```

### Deeply Nested Objects
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

### Adding Item to an Object
Items can be added to an object by providing the key, type and value/pointer (depending on the type).

For heap items, you must pass a pointer to the heap-allocated object.

For stack items, you can pass the literal or a variable. 

Please see [Heap vs Stack Items](#heap-vs-stack-items) for the list of stack/heap items.

Note that duplicate keys are not allowed and will return false.

```c
#include "json.h"

// ...

// assume we get this from somewhere (e.g., parsing a string)
struct json_t* json = ...;

if (!json_add_int32(json, "myvalue", 24))
{
  // something went wrong (e.g., out of memory, etc)
}

// assuming this strdup doesn't fail; can also do it separately to be more safe
if (!json_add_string(json, "otherval", strdup("hey")))
{
  // something went wrong
}

```
See [json.h](include/json.h) for the list of all adder functions.
