ostream& operator<<(ostream& stream, const JWT& jwt)
{
  stream << base64::encode_url_safe(stringify(jwt.header), false) + "."
         << base64::encode_url_safe(stringify(jwt.payload), false) + ".";

  if (jwt.signature.isSome()) {
    stream << jwt.signature.get();
  }

  return stream;
}