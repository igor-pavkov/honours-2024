ostream& operator<<(ostream& stream, const JWT::Alg& alg)
{
  switch (alg) {
    case JWT::Alg::None:
      stream << "none";
      break;
    case JWT::Alg::HS256:
      stream << "HS256";
      break;
    case JWT::Alg::RS256:
      stream << "RS256";
      break;
  }

  return stream;
}