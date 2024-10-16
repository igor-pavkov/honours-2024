ByteVector &ByteVector::operator=(const char *data)
{
  *this = ByteVector(data);
  return *this;
}