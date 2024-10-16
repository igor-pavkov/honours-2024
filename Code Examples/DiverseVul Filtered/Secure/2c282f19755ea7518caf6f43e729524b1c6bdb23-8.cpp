Try<JWT, JWTError> JWT::parse(const string& token, shared_ptr<RSA> publicKey)
{
  CHECK_NOTNULL(publicKey.get());

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

  if (header->alg != JWT::Alg::RS256) {
    return JWTError(
        "Token 'alg' value \"" + stringify(header->alg) +
        "\" does not match, expected \"RS256\"",
        JWTError::Type::INVALID_TOKEN);
  }

  Try<JSON::Object> payload = parse_payload(components[1]);

  if (payload.isError()) {
    return JWTError(payload.error(), JWTError::Type::INVALID_TOKEN);
  }

  const Try<string> signature = base64::decode_url_safe(components[2]);

  if (signature.isError()) {
    return JWTError(
        "Failed to base64url-decode token signature: " + signature.error(),
        JWTError::Type::INVALID_TOKEN);
  }

  // Validate RSA SHA-256 signature.

  const Try<Nothing> valid = verify_rsa_sha256(
      components[0] + "." + components[1], signature.get(), publicKey);

  if (valid.isError()) {
    return JWTError(
        "Failed to verify token: " + valid.error(),
        JWTError::Type::INVALID_TOKEN);
  }

  return JWT(header.get(), payload.get(), signature.get());
}