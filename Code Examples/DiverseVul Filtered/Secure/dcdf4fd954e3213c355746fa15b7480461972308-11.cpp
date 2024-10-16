std::ostream &operator<<(std::ostream &s, const ByteVector &v)
{
  for(TagLib::uint i = 0; i < v.size(); i++)
    s << v[i];
  return s;
}