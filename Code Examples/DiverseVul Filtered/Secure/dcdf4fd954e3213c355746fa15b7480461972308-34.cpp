bool ByteVector::startsWith(const ByteVector &pattern) const
{
  return containsAt(pattern, 0);
}