Try<JSON::Object> decode(const string& component)
{
  const Try<string> decoded = base64::decode_url_safe(component);

  if (decoded.isError()) {
    return Error("Failed to base64url-decode: " + decoded.error());
  }

  const Try<JSON::Object> json = JSON::parse<JSON::Object>(decoded.get());

  if (json.isError()) {
    return Error("Failed to parse into JSON: " + json.error());
  }

  return json;
}