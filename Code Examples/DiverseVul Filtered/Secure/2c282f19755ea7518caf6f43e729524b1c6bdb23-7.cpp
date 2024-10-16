Try<JSON::Object> parse_payload(const string& component)
{
  Try<JSON::Object> payload = decode(component);

  if (payload.isError()) {
    return Error("Failed to decode token payload: " + payload.error());
  }

  // Validate standard claims.

  const Result<JSON::Value> exp_json = payload->find<JSON::Value>("exp");

  if (exp_json.isError()) {
    return Error(
        "Error when extracting 'exp' field from token JSON payload: " +
        exp_json.error());
  }

  if (exp_json.isSome()) {
    if (!exp_json->is<JSON::Number>()) {
      return Error("JSON payload 'exp' field is not a number");
    }

    const int64_t exp = exp_json->as<JSON::Number>().as<int64_t>();
    const int64_t now = Clock::now().secs();

    if (exp < now) {
      return Error(
          "Token has expired: exp(" +
          stringify(exp) + ") < now(" + stringify(now) + ")");
    }
  }

  // TODO(nfnt): Validate other standard claims.
  return payload;
}