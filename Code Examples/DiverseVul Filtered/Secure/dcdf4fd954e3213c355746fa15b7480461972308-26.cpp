ByteVector::ByteVector(const char *data, uint length)
{
  d = new ByteVectorPrivate;
  setData(data, length);
}