void*
json_get(
 const struct json_t* const json,
 const char* const key);

int32_t*
json_get_int32(
  const struct json_t* const json,
  const char* const key);

double*
json_get_decimal(
  const struct json_t* const json,
  const char* const key);

char*
json_get_string(
  const struct json_t* const json,
  const char* const key);

struct json_t*
json_get_object(
  const struct json_t* const json,
  const char* const key);

struct json_array_t*
json_get_array(
  const struct json_t* const json,
  const char* const key);

bool*
json_get_bool(
  const struct json_t* const json,
  const char* const key);

bool
json_get_isnull(
  const struct json_t* const json,
  const char* const key);
