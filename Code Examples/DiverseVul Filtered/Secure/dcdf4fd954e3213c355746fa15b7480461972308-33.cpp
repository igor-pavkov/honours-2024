ByteVector &ByteVector::operator=(char c)
{
  *this = ByteVector(c);
  return *this;
}