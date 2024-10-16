bool ByteVector::operator!=(const char *s) const
{
  return !operator==(s);
}