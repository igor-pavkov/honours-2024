bool ByteVector::operator>(const ByteVector &v) const
{
  return v < *this;
}