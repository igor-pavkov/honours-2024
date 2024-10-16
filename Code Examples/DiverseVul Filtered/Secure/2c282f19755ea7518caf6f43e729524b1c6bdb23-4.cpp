Try<JWT, JWTError> JWT::create(
    const JSON::Object& payload,
    shared_ptr<RSA> privateKey)
{
  CHECK_NOTNULL(privateKey.get());

  const Header header{Alg::RS256, "JWT"};

  const string message = base64::encode_url_safe(stringify(header), false) +
    "." + base64::encode_url_safe(stringify(payload), false);

  const Try<string> signature = sign_rsa_sha256(message, privateKey);

  if (signature.isError()) {
    return JWTError(
        "Failed to generate RSA signature: " + signature.error(),
        JWTError::Type::UNKNOWN);
  }

  return JWT(
    header,
    payload,
    base64::encode_url_safe(signature.get(), false));
}