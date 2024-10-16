Try<JWT, JWTError> JWT::parse(const std::string& token)
{
  const vector<string> components = strings::split(token, ".");

  if (components.size() != 3) {
    return JWTError(
        "Expected 3 components in token, got " + stringify(components.size()),
        JWTError::Type::INVALID_TOKEN);
  }

  Try<JWT::Header> header = parse_header(components[0]);

  if (header.isError()) {
    return JWTError(header.error(), JWTError::Type::INVALID_TOKEN);
  }

  if (header->alg != JWT::Alg::None) {
    return JWTError(
        "Token 'alg' value \"" + stringify(header->alg) +
        "\" does not match, expected \"none\"",
        JWTError::Type::INVALID_TOKEN);
  }

  Try<JSON::Object> payload = parse_payload(components[1]);

  if (payload.isError()) {
    return JWTError(payload.error(), JWTError::Type::INVALID_TOKEN);
  }

  if (!components[2].empty()) {
    return JWTError(
        "Unsecured JWT contains a signature",
        JWTError::Type::INVALID_TOKEN);
  }

  return JWT(header.get(), payload.get(), None());
}