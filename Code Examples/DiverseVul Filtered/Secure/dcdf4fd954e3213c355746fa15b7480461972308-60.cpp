ByteVector &ByteVector::setData(const char *data, uint length)
{
  detach();

  resize(length);

  if(length > 0)
    ::memcpy(DATA(d), data, length);

  return *this;
}