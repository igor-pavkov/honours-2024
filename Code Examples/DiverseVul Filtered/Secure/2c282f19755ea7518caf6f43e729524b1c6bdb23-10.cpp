ostream& operator<<(ostream& stream, const JWT::Header& header)
{
  JSON::Object json;

  json.values["alg"] = stringify(header.alg);
  if (header.typ.isSome()) {
    json.values["typ"] = header.typ.get();
  }

  stream << stringify(json);
  return stream;
}