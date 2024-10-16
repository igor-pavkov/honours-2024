ByteVector &ByteVector::setData(const char *data)
{
  return setData(data, ::strlen(data));
}