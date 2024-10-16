Try<JWT::Header> parse_header(const string& component)
{
  Try<JSON::Object> header = decode(component);

  if (header.isError()) {
    return Error("Failed to decode token header: " + header.error());
  }

  // Validate JOSE header.

  Option<string> typ = None();

  const Result<JSON::Value> typ_json = header->find<JSON::Value>("typ");

  if (typ_json.isSome()) {
    if (!typ_json->is<JSON::String>()) {
      return Error("Token 'typ' is not a string");
    }

    typ = typ_json->as<JSON::String>().value;
  }

  const Result<JSON::Value> alg_json = header->find<JSON::Value>("alg");

  if (alg_json.isNone()) {
    return Error("Failed to locate 'alg' in token JSON header");
  }

  if (alg_json.isError()) {
    return Error(
        "Error when extracting 'alg' field from token JSON header: " +
        alg_json.error());
  }

  if (!alg_json->is<JSON::String>()) {
    return Error("Token 'alg' field is not a string");
  }

  const string alg_value = alg_json->as<JSON::String>().value;

  JWT::Alg alg;

  if (alg_value == "none") {
    alg = JWT::Alg::None;
  } else if (alg_value == "HS256") {
    alg = JWT::Alg::HS256;
  } else if (alg_value == "RS256") {
    alg = JWT::Alg::RS256;
  } else {
    return Error("Unsupported token algorithm: " + alg_value);
  }

  const Result<JSON::Value> crit_json = header->find<JSON::Value>("crit");

  // The 'crit' header parameter indicates extensions that must be understood.
  // As we're not supporting any extensions, the JWT header is deemed to be
  // invalid upon the presence of this parameter.
  if (crit_json.isSome()) {
    return Error("Token 'crit' field is unsupported");
  }

  return JWT::Header{alg, typ};
}