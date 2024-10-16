Try<JWT, JWTError> JWT::create(const JSON::Object& payload)
{
  const Header header{Alg::None, "JWT"};

  return JWT(header, payload, None());
}