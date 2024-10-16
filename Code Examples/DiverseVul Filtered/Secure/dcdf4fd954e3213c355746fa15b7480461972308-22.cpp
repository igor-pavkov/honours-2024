ByteVector ByteVector::fromCString(const char *s, uint length)
{
  ByteVector v;

  if(length == 0xffffffff)
    v.setData(s);
  else
    v.setData(s, length);

  return v;
}