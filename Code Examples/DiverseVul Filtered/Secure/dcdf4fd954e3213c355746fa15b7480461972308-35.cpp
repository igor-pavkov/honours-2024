ByteVector::ByteVector(const char *data)
{
  d = new ByteVectorPrivate;
  setData(data);
}