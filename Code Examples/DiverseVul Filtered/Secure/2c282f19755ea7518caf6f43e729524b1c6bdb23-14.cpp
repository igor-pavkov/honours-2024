Try<JWT, JWTError> JWT::create(
    const JSON::Object& payload,
    const string& secret)
{
  const Header header{Alg::HS256, "JWT"};

  const Try<string> hmac = generate_hmac_sha256(
      base64::encode_url_safe(stringify(header), false) + "." +
        base64::encode_url_safe(stringify(payload), false),
      secret);

  if (hmac.isError()) {
    return JWTError(
        "Failed to generate HMAC signature: " + hmac.error(),
        JWTError::Type::UNKNOWN);
  }

  return JWT(header, payload, base64::encode_url_safe(hmac.get(), false));
}