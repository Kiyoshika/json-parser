bool
json_set_int32(
  const struct json_t* const json,
  const char* const key,
  const int32_t value);

bool
json_set_decimal(
  const struct json_t* const json,
  const char* const key,
  const double value);

bool
json_set_string(
  const struct json_t* const json,
  const char* const key,
  const char* const value);

bool
json_set_object(
  const struct json_t* const json,
  const char* const key,
  struct json_t** value);

bool
json_set_array(
  const struct json_t* const json,
  const char* const key,
  struct json_array_t** value);

bool
json_set_bool(
  const struct json_t* const json,
  const char* const key,
  const bool value);
