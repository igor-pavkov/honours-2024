bool ByteVector::operator==(const char *s) const
{
  if(d->size != ::strlen(s))
    return false;

  return ::memcmp(data(), s, d->size) == 0;
}